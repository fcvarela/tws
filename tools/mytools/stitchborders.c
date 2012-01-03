#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include <string.h>

uint32_t width, spp, offset, border_offset, stride, start, samples, cubeface;
uint32_t top_rev=0, left_rev=0, right_rev=0, bottom_rev=0;
char *filename, *border_directory, top_filename[1024], left_filename[1024], right_filename[1024], bottom_filename[1024];
FILE *in, *top, *left, *right, *bottom;
FILE *out;
uint8_t *buff, *top_buff, *left_buff, *right_buff, *bottom_buff;

void reverse_pixels(uint8_t *pixels);

int main(int argc, char **argv) {
    char usage[] = "Usage: -w <width> -s <samplesperpixel> -f <file> -o <offset> -u <border_offset> -b <borderdir> -c <cubeface>";
    uint32_t opt, i;

    if ((opt = getopt(argc, argv, "w:")) == -1) {
        fprintf(stderr, "%s\n", usage);
        return 0;
    }
    width = atoi(optarg);

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

    if ((opt = getopt(argc, argv, "u:")) == -1) {
        fprintf(stderr, "%s\n", usage);
        return 0;
    }
    border_offset = atoi(optarg);

    if ((opt = getopt(argc, argv, "b:")) == -1) {
        fprintf(stderr, "%s\n", usage);
        return 0;
    }
    border_directory = optarg;

    if ((opt = getopt(argc, argv, "c:")) == -1) {
        fprintf(stderr, "%s\n", usage);
        return 0;
    }
    cubeface = atoi(optarg);

    if ((in = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "IN - Could not open input file %s\n", filename);
        return;         
    } 

    // switch cubeface. prepare columns/rows (borders) and assign rotations needed for each face
    sprintf(top_filename, "%s/", border_directory);
    strcpy(left_filename, top_filename);
    strcpy(right_filename, top_filename);
    strcpy(bottom_filename, top_filename);

    switch (cubeface) {
        case 0:
            strcat(top_filename, "4-top.ppm");
            top_rev = 1;
            strcat(left_filename, "1-top.ppm");
            strcat(right_filename, "3-top.ppm");
            right_rev = 1;
            strcat(bottom_filename, "2-top.ppm");
            break;
        case 1:
            strcat(top_filename, "0-left.ppm");
            strcat(left_filename, "4-right.ppm");
            strcat(right_filename, "2-left.ppm");
            strcat(bottom_filename, "5-left.ppm");
            bottom_rev = 1;
            break;
        case 2:
            strcat(top_filename, "0-bottom.ppm");
            strcat(left_filename, "1-right.ppm");
            strcat(right_filename, "3-left.ppm");
            strcat(bottom_filename, "5-top.ppm");
            break;
        case 3:
            strcat(top_filename, "0-right.ppm");
            top_rev = 1;
            strcat(left_filename, "2-right.ppm");
            strcat(right_filename, "4-left.ppm");
            strcat(bottom_filename, "5-right.ppm");
            break;
        case 4:
            strcat(top_filename, "0-top.ppm");
            top_rev = 1;
            strcat(left_filename, "3-right.ppm");
            strcat(right_filename, "1-left.ppm");
            strcat(bottom_filename, "5-bottom.ppm");
            bottom_rev = 1;
            break;
        case 5:
            strcat(top_filename, "2-bottom.ppm");
            strcat(left_filename, "1-bottom.ppm");
            left_rev = 1;
            strcat(right_filename, "3-bottom.ppm");
            strcat(bottom_filename, "4-bottom.ppm");
            bottom_rev=1;
            break;
    }

    // open all files
    if ((top = fopen(top_filename, "rb")) == NULL) {
        fprintf(stderr, "Could not open input file %s\n", top_filename);
        return;         
    } 

    if ((left = fopen(left_filename, "rb")) == NULL) {
        fprintf(stderr, "Could not open input file %s\n", left_filename);
        return;         
    } 

    if ((right = fopen(right_filename, "rb")) == NULL) {
        fprintf(stderr, "Could not open input file %s\n", right_filename);
        return;         
    } 

    if ((bottom = fopen(bottom_filename, "rb")) == NULL) {
        fprintf(stderr, "Could not open input file %s\n", bottom_filename);
        return;         
    } 

    // theory of operation:
    // 1-read top border, write 0 border 0
    // 2-for each row read left border pixel n, write border pixel, read source row n, write right border pixel
    // 3-read bot border write 0 border 0

    // write header
    fprintf(stdout, "P6\n%u %u\n255\n", width+2, width+2);

    // init dummy sample
    uint8_t empty[spp];
    memset(empty, 255, 3);

    // init source file row buffer
    buff = (uint8_t *)malloc(spp*width);
    //position source file cursor
    fseek(in, offset, SEEK_SET);

    // read top and reverse if necessary
    top_buff = (uint8_t *)malloc(spp*width);
    fseek(top, border_offset, SEEK_SET);
    fread(top_buff, spp*width, 1, top);
    if (top_rev) reverse_pixels(top_buff);

    //read left and reverse if necessary
    left_buff = (uint8_t *)malloc(spp*width);
    fseek(left, border_offset, SEEK_SET);
    fread(left_buff, spp*width, 1, left);
    if (left_rev) reverse_pixels(left_buff);

    //read right and reverse if necessary
    right_buff = (uint8_t *)malloc(spp*width);
    fseek(right, border_offset, SEEK_SET);
    fread(right_buff, spp*width, 1, right);
    if (right_rev) reverse_pixels(right_buff);

    //read bottom and reverse if necessary
    bottom_buff = (uint8_t *)malloc(spp*width);
    fseek(bottom, border_offset, SEEK_SET);
    fread(bottom_buff, spp*width, 1, bottom);
    if (bottom_rev) reverse_pixels(bottom_buff);


    // FIRST ROW: empty border empty
    // write empty pixel
    fwrite(empty, spp, 1, stdout);
    // write top border
    fwrite(top_buff, spp*width, 1, stdout);
    // write empty pixel
    fwrite(empty, spp, 1, stdout);

    // BODY: left border, source row, right border
    for (i=0; i<width; i++) {
        // left border pixel
        fwrite(&left_buff[i*spp], 3, 1, stdout);

        // source file row
        fread(buff, spp*width, 1, in);
        fwrite(buff, spp*width, 1, stdout);

        // right border pixel
        fwrite(&right_buff[i*spp], 3, 1, stdout);
    }

    // bottom row
    fwrite(empty, spp, 1, stdout);
    fwrite(bottom_buff, spp*width, 1, stdout);
    fwrite(empty, spp, 1, stdout);

    // for every row in source file, write left sample, row, right sample
    fclose(in);
    fclose(top);
    fclose(left);
    fclose(right);
    fclose(bottom);
    return 0;
}

void reverse_pixels(uint8_t *pixels) {
    size_t i;

    uint8_t *temp = (uint8_t *)malloc(spp * width);
    for (i=0; i<width; i++)
        memcpy(&temp[(width-i-1) * spp], &pixels[i * spp], spp);

    memcpy(pixels, temp, spp*width);
}
