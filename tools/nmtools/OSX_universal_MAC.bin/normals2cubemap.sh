#! /bin/sh

sourcefile=~/Desktop/mars/normals32k.ppm

project gnomonic -scale 2 -lat 90 -long 0 -w 8192 -h 8192 -f $sourcefile > ~/Desktop/normals0.ppm
project gnomonic -scale 2 -lat 0 -long -90 -w 8192 -h 8192 -f $sourcefile > ~/Desktop/normals1.ppm
project gnomonic -scale 2 -lat 0 -long 0 -w 8192 -h 8192 -f $sourcefile > ~/Desktop/normals2.ppm
project gnomonic -scale 2 -lat 0 -long 90 -w 8192 -h 8192 -f $sourcefile > ~/Desktop/normals3.ppm
project gnomonic -scale 2 -lat 0 -long 180 -w 8192 -h 8192 -f $sourcefile > ~/Desktop/normals4.ppm
project gnomonic -scale 2 -lat -90 -long 0 -w 8192 -h 8192 -f $sourcefile > ~/Desktop/normals5.ppm

