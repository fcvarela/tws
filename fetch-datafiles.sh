#!/bin/bash

for p in mars earth
do
    for i in {0..5}
    do
        for f in rgbspec topo normals
        do
            if [ ! -f data/$p/$f$i.ppm ]
            then
                echo Fetching $p/$f$i.ppm
                curl -f -o data/$p/$f$i.ppm http://softwarelivre.sapo.pt/tws/$p/$f$i.ppm
            fi
        done
    done
    if [ ! -d data/$p/cache ]
    then
        mkdir data/$p/cache
    fi
done

