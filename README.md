# fuzzing_lodepng
## ВВЕДЕНИЕ


Это проект по фаззингу!
Выполнение немного задержалось возникли небольшие (большие) проблемы с поиском бага в программе мутатора, поиск вызывал большие сложности, так как приходилось использовать мало знакомые библиотеки)
 <br><br><br>

 
## УСТАНОВКА


Для установки будут нужны установленная библиотека libpng и сам фаззер AFL++

Для того чтобы скомпилировать проект досточно будет сделать 2 make в корневой папке проекта, на возможные Warnings не стоит обращать внимания)
AFL_HEADERS должна быть устанослена и указывать на директорию с header файлами AFL++, путь должен быть ПРЯМЫМ, а НЕ ОТНОСИТЕЛЬНЫМ

```bash
AFL_HEADERS=/path/to/AFLplusplus/include make customs_libs
make all

```

(надеюсь работает корректно)<br><br><br>



## О ОБЪЕКТЕ ФАЗЗИНГА

Проект был выбран следующий: https://github.com/lvandeve/lodepng (В моем проекте [lodepng.c] (src/lodepng/lodepng.cpp))
Это по сути своей библиотека для работы с png файлами. (Изначально была выбрана библиотека libpng для ее фаззинга, но уже после реализации фаззинга я с ужасом понял, что ее нет в списке проектов github в том файле. С этим тоже связана задержка выполнения задания)
Сам объект для фаззинга был взят как пример из данной библиотеки, демонстрирующий ее работу.
Я выбирал из всех примеров тот, что больше всего связан непосредственно с чтением файлов, так как функции чтения и парсинга наиболее приоритетные цели при фаззинге.
<br><br><br>


## О МОЕМ МУТАТОРЕ 

Был реализован кастомный мутатор на языке Си с использованием библиотеки libpng.
Бибилиотека была выбрана достаточно низкоуровневая для получения большего простора для манипуляций с png файлами.
 
 Немного о png формате: <br>
 Картинка png состоит из различных сегментов - чанков, хранящих в себе какую-то информацию. Она может быть сжата, а может быть и нет. 
 В начале каждого чанка есть его индетификатор, и информация о его структуре и размерах. В конце каждого чанка есть контрольная сумма. 

  
В ходе пробных запусков фаззера (без мутатора) и анализа функций были найдены "узкие места". Это как правило не основные чанки, хранящие данные необязательные для корректного открытия и чтения png.
В этом можно убедится взглянув на результаты почти 3ех часового фаззинга (файл 2h_without_custom_mutator) на в данных о покрытии кода. 

Поадания в необязательные чанки: cHRM, tIME, pHYs, sRGB, bKGD, tRNS, PLTE, sBIT, tEXt, iTXt, iCCP и т.д.

![Попадания в необязательные чанки](docs/2_hours_without_custom_mutator_report/pics/2h_fuzz_hits.jpg)


Поэтому было решено реализовать мутатор случайным образом выбирающий допустимые и поддерживаемые libpng чанки и корректно записывающий их в буффер.

[Полный список реализованых чанков](include/png_processing.h)
<br><br><br>

## О РЕАЛИЗАЦИИ МУТАТОРА


Сам по себе [мутатор](src/custom_mutators/afl_custom_mutator.c) имеет простую реализацию. Я использую только функцию afl_custom_fuzz, это связано с тем, что единственная "стоющая" мутация (которую я придумал) связана с формированием произвольного корректного png файла, остальные функции мутации не очень уместны. Содержимое чанка IDAT не столь важно и может быть (почти всегда) случайным, и только у этого чанка оно может быть совсем уж произвольным. Данные полученные от фаззера все (или частично) попадают в него. Остальные же чанки имеют достаточно четкую структуру и ограничения на содержимое, поэтому создаются в отдельной библиотеке.

Основная библиотека, реализующая добаление чанков случайным образом находится в [lib_png_random](src/lib_png_random)
<br><br><br>

Начну пояснения дополнительных библиотек (все они реализованы мной). 

### Библиотека [vector](src/lib_vector)
Изначально была реализована для реализации записи png изображения в память, а не в файл (но сейчас не используется для этого). Используется для удобства записи, хранения и освобождения данных структуры png_processing_t.
<br><br>

### Библиотека [afl_vector](src/lib_afl_vector)
Была реализована для записи png файла в память. Использует не просто массивы, а реализованные в alloc-inl.h - структуры afl_buf. Как выяснилось, при передаче фаззеру измененного буффера он должен соответствовать этой структуре. В противном случае велик риск (а при фаззинге он почти 100%), что фаззер попытается использовать ее поля и произойдет сбой. <br>
Отличие этой структуры от массива в том, что перед ней размещается ее размер.
<br><br>

### Библиотека [png_processing](src/lib_png_processing)
Реализует создание, уничтожение, сброс полей структуры png_processing_t отвечающей за хранение всей информации о создании случайного png файла, а также вектор png_out, хранящий уже записанный корректный png файл.
<br><br>

### Библиотека [png_write](src/lib_png_write)
Реализует запись png файла из корректных приготовленных данных структуры png_processing_t в ее же поле - вектор png_out.
<br><br>

### Библиотека [png_random](src/lib_png_random)
Реализует формирование случайных корректных данных структуры png_processing_t, а также записывает результат. Основная функция которой я буду пользоваться в мутаторе это <br>
```C
// pic - данные которые будут записаны в чанк IDAT - картинка
int make_random_png(png_processing_t *png_prc, uint8_t *pic, size_t pic_size);
```

Алгоритм ее работы примерно следующий:
1) Случаным образом выбираются дополнительные чанки (те что мы хотим добавить)
2) Вызывается функция png_config_chunks, которая формирует все данные чанков случайным обзазом, но сразу корректно
3) Запись в png_out полученного png
<br>

Алгоритм формирования чанков:
1) Идет формирование необходимой информации для png - IHDR чанк (длина, ширина, глубина цвета, тип цвета и прочее)
2) Формирование дополнительных чанков: bKGD, tRNS, sPLT, cHRM, iCCP, pHYs, sBIT, sRGB, tEXt, iTXt, tIME, cSTM
<br>

Стоит обратить внимание, что средм них есть один чанк, который не считается унифицированным: cSTM - custom chunk
Содержащий какие-то данные (в моем случае - текст для удобства отладки) и который размещается случайным образом среди всех чанков
<br>

В этой же директории лежат файлы [test.c](src/lib_png_random/test.c) и [infinity_testing.sh](src/lib_png_random/infinity_testing.sh) позволяющие наглядно протестиировать работоспобность генерации случайных изображений (можно открыть выходное изображение и смотреть как оно меняется) при помощи реализованного мной вывода данных о сформированном изображении и утилиты pngcheck, подтверждающей корректность изображения.

(Нужно просто запустить [infinity_testing.sh](src/lib_png_random/infinity_testing.sh))
<br><br>

### Библиотека [png_print](src/lib_png_print)
Библиотека необходима для вывода информации о png_processing_t и вообщем о структурах png. Побольшому счету необходима для отладки. 
<br><br>

### Библиотека [lodepng](src/lodepng), [fuzz_program](src/fuzz_program)
Библиотека и сама программа фаззинг которой и производится
<br><br>

### Библиотека [custom_mutator](src/custom_mutators)
Реализующая пользовательский мутатор для фаззинга
<br><br>

ПРО UTILS не быть !!!

## ДЕМОНСТРАЦИЯ РАБОТЫ ФАЗЗЕРОВ

### Подключение фаззера
(См. [Makefile](Makefile))

Компилировал программу я обычным способом (но используя компилятор afl-clang-fast++), без особых флагов компиляции (за исключением cmploger), санитайзеры не подключались в угоду скорости тестирования, подразумавая, что данный проект уже подлежал фаззингу и работает корректно (при необходимости можно включить). Программы run_main и run_worker не имели особых флагов компиляции, но cmploger да, включены флаги:
1) AFL_LLVM_CMPLOG=1 - включает Comparison Logging, что помогает проходить сложные условия (на if навешиваются хуки дающие знание фаззеру о аргументах операций сравнения),
2) AFL_LLVM_LAF_ALL=1 - включает Split Switches, помогающий работе предыдущего разбивая сложные условия на несколько простых

Остльные флаги компиляции:
-O3 -g -Wall -I. -Iinclude -Wextra -pedantic -ansi


При запуске использовались флаги
1) -t 100  я знаю, что программа не должна работать долго, так как я сам ограничивал размер входного файла, 100мс вполне хватило бы (и хватало)
2) -i $(INPUT) - входная директория
3) -o $(OUTPUT) - выходная директория
4) -x $(DICTIONARY) - использовал [словарь](dictionaries) предлагаемый afl++ для фаззинга png
5) -с 0 - (только для cmploger) для включения Comparison Logging
6)  AFL_CUSTOM_MUTATOR_LIBRARY=$(CUSTOM_MUTATOR_LIBRARY) - для включения своего мутатора (работал хорошо, поэтому включил для всех)
7)  AFL_FINAL_SYNC=1 - (только для main) для включения синхронизации его с остальными процессами

   
я просто указывал флаг пути к скомпилированной динамической библиотеке при запуске фаззера. 

## О РЕЗУЛЬТАТАХ ФАЗЗИНГА


К сожалению, чтобы подробнее посмотреть отчеты их придется загрузить открыть при помощи браузера (они в html формате и другого способа я не нашел)


Подробные отчеты о фаззинге БЕЗ МУТАТОРА можно найти в файлах [Статистика по линиям фаззинга без мутатора](docs/2_hours_without_custom_mutator_report/index.html), 
[Статистика по функциям фаззинга без мутатора](docs/2_hours_without_custom_mutator_report/fuzzing_lodepng/utils/coverage/source/main.cpp.func-sort-c.html)

Статистика по линиям:
![Статистика без мутатора](docs/2_hours_without_custom_mutator_report/pics/2h_line_statistic.jpg)



Подробные отчеты о фаззинге С МУТАТОРОМ можно найти в файлах [Статистика по линиям фаззинга с мутатором](docs/1_hour_custom_fuzzing_report/web/index.html), 
[Статистика по функциям фаззинга с мутатором](docs/1_hour_custom_fuzzing_report/web/fuzzing_lodepng/utils/coverage/source/lodepng.cpp.func-sort-c.html)

Статистака по линиям:
![Статистика с мутатором](docs/1_hour_custom_fuzzing_report/pics/1h_statistic.jpg)


Сразу же заметен прирост колличества линий и функций. Однако это только первый взгляд на результаты. При детальном рассмотрении все становится еще лучше!

Рассмотрим теперь те же функции чтения чанков:

![Статистика по функциям чтения чанков](docs/1_hour_custom_fuzzing_report/pics/1h_func_statistic.jpg)


Виден огромный припрост в колличестве их вызов, а следственно можно утверждать, что фаззер лучше справляется с тестированием программы. Больше попаданий == программа лучше протестирована. 

Аналогичные результаты можно наблюдать в функциях, отвечающих за чтение и анализ основных чанков. Результаты показывают, что попадание даже в основные чанки возросло неменее чем в 2 раза (это функции чтения info и idat). <br><br><br>


## ВЫВОДЫ


Можно говорить, что мутатор хорошо справляется со своей задачей. Наверника его можно еще немного улучшить добавив случайный порядок записи чанков (достаточно сложно, необходимо углубиться в документацию о чанках и переписать большое колличество кода) или например добавить больше поддерживаемых языков в чанке iTXt (это задача чисто механическая и нетрудно выполнимая).
