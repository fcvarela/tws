#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>


uint32_t width, height, spp, offset, stride, start, samples;
char border;
char *filename;
char outfilename[1024];
FILE *in;
FILE *out;

void extract_border(void);

int main(int argc, char **argv) {
    char usage[] = "Usage: -w <width> -h <height> -s <samplesperpixel> -f <file> -o <offset> -b 0|1|2|3 (top|left|right|bottom";
    uint32_t opt;

    if ((opt = getopt(argc, argv, "w:")) == -1) {
        fprintf(stderr, "%s\n", usage);
        return 0;
    }
    width = atoi(optarg);

    if ((opt = getopt(argc, argv, "h:")) == -1) {
        fprintf(stderr, "%s\n", usage);
        return 0;
    }
    height = atoi(optarg);

    if ((opt = getopt(argc, argv, "s:")) == -1) {
        fprintf(stderr, "%s\n", usage);
        return 0;
    }
    spp = atoi(optarg);

    if ((opt = getopt(argc, argv, "f:")) == -1) {
        fprintf(stderr, "%s\n", usage);
        return 0;
    }
    filename = optarg;

    if ((opt = getopt(argc, argv, "o:")) == -1) {
        fprintf(stderr, "%s\n", usage);
        return 0;
    }
    offset = atoi(optarg);

    if ((opt = getopt(argc, argv, "b:")) == -1) {
        fprintf(stderr, "%s\n", usage);
        return 0;
    }
    border = atoi(optarg);

    // a column or row is just a set of values from a given position with a certain stride
    // rows are zero stride, columns are width strided

    switch (border) {
        case 0: 
            start = 0;
            stride = 0;
            samples = width;
            break;
        case 1:
            start = 0;
            stride = width;
            samples = height;
            break;
        case 2:
            start = width-1;
            stride = width;
            samples = height;
            break;
        case 3:
            start = (height-1)*width;
            stride = 0;
            samples = width;
            break;
    }

    extract_border();
    return 0;
}

void extract_border(void) {
    uint32_t sample;
    uint8_t sample_val[spp];

    if ((in = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "Could not open input file %s\n", filename);
        return;
    }

    fprintf(stdout, "P6\n");
    if (stride == 0) fprintf(stdout, "%u 1\n", width);
    else fprintf(stdout, "1 %u\n", height);
    fprintf(stdout, "255\n");

    for (sample = 0; sample < samples; sample++) {
        if (stride == 0)
            fseek(in, offset + start*spp + sample*spp, SEEK_SET);
        else
            fseek(in, offset + start*spp + sample*stride*spp, SEEK_SET);

        fread(sample_val, spp, sizeof(uint8_t), in);
        fwrite(sample_val, spp, sizeof(uint8_t), stdout);
    }

    fclose(in);
}

