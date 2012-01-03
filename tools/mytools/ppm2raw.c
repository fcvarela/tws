#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
	unsigned char *rowin = (unsigned char *)malloc(16384*3);
	unsigned char *rowout = (unsigned char *)malloc(16384*3);
	int i = 0;
	int row;
	
	int headerlen = 19;
	fprintf(stderr, "Skipping %d bytes of ppm header\n", headerlen);
	unsigned char *header = (unsigned char *)malloc(headerlen);
	fread(header, 1, headerlen, stdin);
	
	for (row=0; row<16384; row++) {
		if (row % 1024 == 0) 
			fprintf(stderr, "[%d]", row);
		
		fread(rowin, 1, 16384*3, stdin);
	
		for (i=0; i<16384; i++) {
			rowout[i*3+0] = rowin[i*3+0];
			rowout[i*3+1] = rowin[i*3+1];
			rowout[i*3+2] = rowin[i*3+2];
		}
	
		fwrite(rowout, 1, 16384*3, stdout);
	}
	return 0;
}