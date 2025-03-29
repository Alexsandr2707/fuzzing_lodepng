# fuzzbench
<br>
Я установил и запустил эксперимент над фаззерами при помощи fuzzbench. <br>
Я для этого выбрал 2 фаззера: afl и afl++ <br>
Бенчмарки: libpng_libpng_read_fuzzer и bloaty_fuzz_target <br>
Количество испытаний: 1 <br>
Время одного испытания: 1 час <br>

Все файлы отчета можно увидеть в директории [afl_x_aflplusplus](docs/fuzzbench/afl-x-aflplusplus)
<br><br>

!!! Добавлены новые результаты 5 часового эксперимента в директории [5h_afl_aflplusplus](docs/fuzzbench/5h_afl_x_aflplusplus)!!!  <br>
Я для этого выбрал 2 фаззера: afl и afl++ <br>
Бенчмарки: libpng_libpng_read_fuzzer и bloaty_fuzz_target <br>
Количество испытаний: 1 <br>
Время одного испытания: 5 час <br>
<br><br>

### Демонстрация работы фаззеров 

![image](https://github.com/user-attachments/assets/e2d7301a-988f-45fb-b429-c63a2af0608c)
<br><br>

## РЕЗЮМЕ ИСПЫТАНИЯ НА БЕНЧМАРКЕ BLOATY_FUZZ_TARGET (1 час)
<br>

### Рейтинг по медианному охвату <br>

![image](docs/fuzzbench/afl-x-aflplusplus/bloaty_fuzz_target_ranking.svg)

<br>

### Средний рост покрытия с течением времени <br>

![image](docs/fuzzbench/afl-x-aflplusplus/bloaty_fuzz_target_coverage_growth.svg)

<br>

## РЕЗЮМЕ ИСПЫТАНИЯ НА БЕНЧМАРКЕ LIBPNG_LIBPNG_READ_FUZZER (1 час)
<br>

### Рейтинг по медианному охвату <br>

![image](docs/fuzzbench/afl-x-aflplusplus/libpng_libpng_read_fuzzer_ranking.svg)

<br>

### Средний рост покрытия с течением времени <br>

![image](docs/fuzzbench/afl-x-aflplusplus/libpng_libpng_read_fuzzer_coverage_growth.svg)

<br>

## РЕЗЮМЕ ИСПЫТАНИЯ НА БЕНЧМАРКЕ BLOATY_FUZZ_TARGET (5 часов)
<br>

### Рейтинг по медианному охвату <br>

![image](docs/fuzzbench/5h_afl_x_aflplusplus/bloaty_fuzz_target_ranking.svg)

<br>

### Средний рост покрытия с течением времени <br>

![image](docs/fuzzbench/5h_afl_x_aflplusplus/bloaty_fuzz_target_coverage_growth.svg)

<br>

## РЕЗЮМЕ ИСПЫТАНИЯ НА БЕНЧМАРКЕ LIBPNG_LIBPNG_READ_FUZZER (5 часов)
<br>

### Рейтинг по медианному охвату <br>

![image](docs/fuzzbench/5h_afl_x_aflplusplus/libpng_libpng_read_fuzzer_ranking.svg)

<br>

### Средний рост покрытия с течением времени <br>

![image](docs/fuzzbench/5h_afl_x_aflplusplus/libpng_libpng_read_fuzzer_coverage_growth.svg)

<br>




## ВЫВОД
<br>
Такое количество испытаний и времени потраченного на тестирование фаззера не является объективным показателем эффективности фаззеров,
на основе полученных результатов сложно сделать вывод об эффективности afl++ по сравнению с afl. По результам 1 часового эксперимента на bloaty_fuzz_target afl++ показал себя хуже чем afl однако, как показывают отчеты и результаты 5 часового эксперимента, на большем промежутке afl++ выиграет. На libpng_libpng_read_fuzzer 
в точности наоборот. 
<br>

Изходя из результатов 5 часового эксперимента, можно сказать, что afl++ выигрывает afl (по крайней мере в 1ом испытании).
