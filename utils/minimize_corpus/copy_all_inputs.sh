#! /bin/bash

ROOT=/fuzzing_lodepng
OUTPUT=${ROOT}/output
UTILS=${ROOT}/utils
INPUTS=${UTILS}/minimize_corpus/all_inputs

cd ${OUTPUT}
OUT_DIRS=$(ls -d */)
OUT_DIRS=${OUT_DIRS//'/'}
OUT_DIRS=${OUT_DIRS//"cov"}
OUT_DIRS=${OUT_DIRS//"  "}


for dir in $OUT_DIRS
do
    echo copy $dir
    cd ${OUTPUT}/$dir/queue
    for file in $(ls *)  
    do
        cp "${file}" "${INPUTS}/${dir}_${file}";
    done
done
