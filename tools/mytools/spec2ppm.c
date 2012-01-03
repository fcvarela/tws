#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <getopt.h>

int main(int argc, char **argv) {
	uint32_t opt;

	if ((opt = getopt(argc, argv, "w:")) == -1) {
		fprintf(stderr, "Usage: -w <width> -h <height>\n");
		return 0;
    }

	uint32_t width = atoi(optarg);

	if ((opt = getopt(argc, argv, "h:")) == -1) {
		fprintf(stderr, "Usage: -w <width> -h <height>\n");
		return 0;
	}

	uint32_t height = atoi(optarg);

	unsigned char *rowin = (unsigned char *)malloc(width*1);
	unsigned char *rowout = (unsigned char *)malloc(width*3);
	int i = 0;
	int row;
	
	fprintf(stdout, "P6\n");
    fprintf(stdout, "%d %d\n255\n", width, height);
	
	for (row=0; row<height; row++) {
		if (row % 1024 == 0) 
			fprintf(stderr, "Row [%d]\n", row);
		
		fread(rowin, 1, width*1, stdin);
		for (i=0; i<width; i++) {
			rowout[i*3+0] = rowin[i];
			rowout[i*3+1] = rowin[i];
			rowout[i*3+2] = rowin[i];
		}
		fwrite(rowout, 1, width*3, stdout);
	}
	return 0;
}
