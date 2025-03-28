# fuzzbench
<br>
Я установил и запустил эксперимент над фаззерами при помощи fuzzbench. <br>
Я для этого выбрал 2 фаззера: afl и afl++ <br>
Бенчмарки: libpng_libpng_read_fuzzer и bloaty_fuzz_target <br>
Колличество испытаний: 1 <br>
Время одного испытания: 1 час <br>
<br><br>

Все файлы отчета можно увидеть в директории [afl_x_aflplusplus](docs/fuzzbench/afl-x-aflplusplus)


### Демонстрация работы фаззеров 

![image](https://github.com/user-attachments/assets/e2d7301a-988f-45fb-b429-c63a2af0608c)
<br><br>

## РЕЗЮМЕ ИСПЫТАНИЯ НА БЕНЧМАРКЕ BLOATY_FUZZ_TARGET 
<br>

### Рейтинг по медианному охвату <br>

![image](docs/fuzzbench/afl-x-aflplusplus/bloaty_fuzz_target_ranking.svg)

<br>

### Средний рост покрытия с течением времени <br>

![image](docs/fuzzbench/afl-x-aflplusplus/bloaty_fuzz_target_coverage_growth.svg)

<br>

## РЕЗЮМЕ ИСПЫТАНИЯ НА БЕНЧМАРКЕ LIBPNG_LIBPNG_READ_FUZZER 
<br>

### Рейтинг по медианному охвату <br>

![image](docs/fuzzbench/afl-x-aflplusplus/libpng_libpng_read_fuzzer_ranking.svg)

<br>

### Средний рост покрытия с течением времени <br>

![image](docs/fuzzbench/afl-x-aflplusplus/libpng_libpng_read_fuzzer_coverage_growth.svg)

<br>


## ВЫВОД
<br>
Такое колличество испытаний и времени потраченого на тестирование фаззера не является объективным показателем эффективности фаззеров,
на основе полученных результатов сложно сделать вывод об эффективности afl++ по сравнению с afl. В bloaty_fuzz_target afl++ показал себя хуже чем afl (однако, как показывают отчеты, на больше промежутке afl++ выйграет), а в libpng_libpng_read_fuzzer наоборот. 
