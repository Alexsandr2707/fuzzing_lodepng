#! /bin/bash

make all test

while true; do
    clear && ./test && pngcheck -v output_creat.png output.png
    if [ $? -ne 0 ]; then
        echo "The program terminated with an error."
        break
    fi

    sleep 0.1
done
