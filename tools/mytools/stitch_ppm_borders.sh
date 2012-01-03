#!/bin/bash

planet=mars
ppmoffset=17
borderoffset=14
width=8192
kind=rgbspec
for i in {0..5}
do
    ./stitchborders -w $width -s 3 -f ../../data/$planet/$kind$i.ppm -o $ppmoffset -u $borderoffset -b ../../data/$planet/borders/$kind -c $i > $kind$i-big.ppm
done

