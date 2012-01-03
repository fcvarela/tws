/*
   ppm2topo
   This program remaps each rgb sample in a file created by topo2ppm back
   into a topo sample. Ideally each file is already a cubemap face of 16k/16K
   */

#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include <stdio.h>

int main(int argc, char **argv) {
    uint32_t opt;

    if ((opt = getopt(argc, argv, "l:")) == -1) {
        fprintf(stderr, "Usage: -l <ppm header len> -w <width> -h <height>\n");
        return 0;
    }

    uint32_t header_len = atoi(optarg);
    if ((opt = getopt(argc, argv, "w:")) == -1) {
        fprintf(stderr, "Usage: -l <ppm header len> -w <width> -h <height>\n");
        return 0;
    }

    uint32_t width = atoi(optarg);

    if ((opt = getopt(argc, argv, "h:")) == -1) {
        fprintf(stderr, "Usage: -l <ppm header len> -w <width> -h <height>\n");
        return 0;
    }

    uint32_t height = atoi(optarg);

    unsigned char *rowin = (unsigned char *)malloc(width*3);
    unsigned char *rowout = (unsigned char *)malloc(width*2);
    uint64_t i = 0;
    uint64_t row;
    size_t len;

    // skip header
    fread(rowin, 1, header_len, stdin);

    for (row=0; row<height; row++) {
        if (row % 1024 == 0)
            fprintf(stderr, "[%lu]\n", row);

        // read an entire row of 3bytes per pixel: width*3
        len = fread(rowin, 3, width, stdin);
        for (i=0; i<width; i++) {
            rowout[i*2+0] = rowin[i*3+0];
            rowout[i*2+1] = rowin[i*3+1];
        }
        fwrite(rowout, 2, width, stdout);
    }
    return 0;
}
