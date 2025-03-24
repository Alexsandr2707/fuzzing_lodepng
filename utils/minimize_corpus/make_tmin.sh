#! /bin/bash

CMD=/fuzzing_lodepng/bin/worker

cd /fuzzing_lodepng/utils/minimize_corpus
rm -f tmin_inputs/*
mkdir -p tmin_inputs cmin_inputs


cd cmin_inputs
for file in $(ls *) 
do
    afl-tmin -i $file -o ../tmin_inputs/${file} -- ${CMD} @@
done
