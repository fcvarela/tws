#!/bin/bash

for i in {0..5}
do
    ./merge_spec_rgb -w 16386 -r ../../data/earth/texture$i.pbm -s ../../data/earth/spec$i-big.ppm -o 19 > ../../data/earth/rgbspec$i.ppm
done

