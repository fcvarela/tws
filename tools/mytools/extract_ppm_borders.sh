#!/bin/bash

planet=mars
content=rgbspec
facesize=8192
ppmheader=17

for i in {0..5}
do
	./ppmborder -w $facesize -h $facesize -s 3 -f ../../data/$planet/$content$i.ppm -o $ppmheader -b 0 > ../../data/$planet/borders/$content/$i-top.ppm
	./ppmborder -w $facesize -h $facesize -s 3 -f ../../data/$planet/$content$i.ppm -o $ppmheader -b 1 > ../../data/$planet/borders/$content/$i-left.ppm
	./ppmborder -w $facesize -h $facesize -s 3 -f ../../data/$planet/$content$i.ppm -o $ppmheader -b 2 > ../../data/$planet/borders/$content/$i-right.ppm
	./ppmborder -w $facesize -h $facesize -s 3 -f ../../data/$planet/$content$i.ppm -o $ppmheader -b 3 > ../../data/$planet/borders/$content/$i-bottom.ppm
done

