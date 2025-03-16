#! /bin/bash

cd /fuzzing_lodepng/output
OUT_DIRS=$(ls -d */)
OUT_DIRS=${OUT_DIRS//'/'}
OUT_DIRS=${OUT_DIRS//"cov"}
OUT_DIRS=${OUT_DIRS//"  "}


for dir in $OUT_DIRS
do
    echo copy $dir
    cd /fuzzing_lodepng/output/$dir/queue
    for file in $(ls *)  
    do
        cp "${file}" "/fuzzing_lodepng/minimize_corpus/all_inputs/${dir}_${file}";
    done
done
