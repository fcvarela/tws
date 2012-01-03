#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include <string.h>

uint32_t width, offset;
char *rgb_filename, *spec_filename;
FILE *rgb, *spec;
uint8_t sample_rgb[4];
uint8_t sample_spec[3];

int main(int argc, char **argv) {
    char usage[] = "Usage: -w <width> -r <rgb_file> -s <spec_file> -o <offset>";
    uint32_t opt, i;

    if ((opt = getopt(argc, argv, "w:")) == -1) {
        fprintf(stderr, "%s\n", usage);
        return 0;
    }
    width = atoi(optarg);

    if ((opt = getopt(argc, argv, "r:")) == -1) {
        fprintf(stderr, "%s\n", usage);
        return 0;
    }
    rgb_filename = optarg;

    if ((opt = getopt(argc, argv, "s:")) == -1) {
        fprintf(stderr, "%s\n", usage);
        return 0;
    }
    spec_filename = optarg;

    if ((opt = getopt(argc, argv, "o:")) == -1) {
        fprintf(stderr, "%s\n", usage);
        return 0;
    }
    offset = atoi(optarg);

    if ((rgb = fopen(rgb_filename, "rb")) == NULL) {
        fprintf(stderr, "Could not open input file %s\n", rgb_filename);
        return;
    }

    if ((spec = fopen(spec_filename, "rb")) == NULL) {
        fprintf(stderr, "Could not open input file %s\n", spec_filename);
        return;
    }

    // position both cursors at offset
    fseek(rgb, offset, SEEK_SET);
    fseek(spec, offset, SEEK_SET);

    // write header
    fprintf(stdout, "P6\n%u %u\n255\n", width, width);

    for (i=0; i<width*width; i++) {
        fread(sample_rgb, 3, 1, rgb);
        fread(sample_spec, 3, 1, spec);
        sample_rgb[3] = sample_spec[0];

        fwrite(sample_rgb, 4, 1, stdout);
    }

    fclose(rgb);
    fclose(spec);

    return 0;
}

