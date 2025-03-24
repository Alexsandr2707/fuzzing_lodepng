# fuzzing_lodepng
Fuzzing of lodepng library

Это проект по фаззингу!
Выполнение немного задержалось возникли небольшие (большие) проблемы с поиском бага в программе мутатора, поиск вызывал большие сложности, так как приходилось использовать мало знакомые библиотеки)

Я выполняю задание по фаззингу. 

О ОБЪЕКТЕ ФАЗЗИНГА

Проект был выбран следующий: https://github.com/lvandeve/lodepng
Это по сути своей библиотека для работы с png файлами. (Изначально была выбрана библиотека libpng для ее фаззинга, но уже после реализации фаззинга я с ужасом понял, что ее нет в списке проектов github в том файле. С этим тоже связана задержка выполнения задания)
Сам объект для фаззинга был взят как пример из данной библиотеки, демонстрирующий ее работу.
Я выбирал из всех примеров тот, что больше всего связан непосредственно с чтением файлов, так как функции чтения и парсинга наиболее приоритетные цели при фаззинге.

О МОЕМ МУТАТОРЕ 

Был реализован кастомный мутатор на языке Си с использованием библиотеки libpng.
Бибилиотека была выбрана достаточно низкоуровневая для получения большего простора для манипуляций с png файлами.

{ 
  немного о png формате: 
  картинка png состоит из различных сегментов - чанков, хранящих в себе какую-то информацию. Она может быть сжата, а может быть и нет. 
  В начале каждого чанка есть его индетификатор, и информация о его структуре и размерах. В конце каждого чанка есть контрольная сумма. 
}
  
В ходе пробных запусков фаззера (без мутатора) и анализа функций были найдены "узкие места". Это как правило не основные чанки, хранящие данные необязательные для корректного открытия и чтения png.
В этом можно убедится взглянув на результаты почти 3ех часового фаззинга (файл 2h_without_custom_mutator) на в данных о покрытии кода. 

Поадания в необязательные чанки: cHRM, tIME, pHYs, sRGB, bKGD, tRNS, PLTE, sBIT, tEXt, iTXt, iCCP и т.д.

![Попадания в необязательные чанки](docs/2_hours_without_custom_mutator_report/pics/2h_fuzz_hits.jpg)


Поэтому было решено реализовать мутатор случайным образом выбирающий допустимые и поддерживаемые libpng чанки и корректно записывающий их в буффер.

[Полный список реализованых чанков](include/png_processing.h)


Основная библиотека, реализующая добаление чанков случайным образом находится в [png_random.c](src/lib_png_random/png_random.c)

Реализация чуть более детально будет расписана здесь.


О РЕЗУЛЬТАТАХ ФАЗЗИНГА

Подробные отчеты о фаззинге без мутатора и с мутатором можно найти в файлах [Статистика по линиям фаззинга без мутатора](https://raw.githack.com/Alexsandr2707/fuzzing_lodepng/blob/main/docs/2_hours_without_custom_mutator_report/index.html), [Статистика по функциям фаззинга без мутатора](docs/2_hours_without_custom_mutator_report/fuzzing_lodepng/utils/coverage/source/main.cpp.func-sort-c.html)
