#! /bin/bash

make all test

while true; do
    clear && ./test && pngcheck -v output.png
    if [ $? -ne 0 ]; then
        echo "Программа завершилась с ошибкой. Выход из цикла."
        break
    fi

    sleep 0.5
done
