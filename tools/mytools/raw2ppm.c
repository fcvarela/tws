#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
	unsigned char *rowin = (unsigned char *)malloc(512*3);
	unsigned char *rowout = (unsigned char *)malloc(512*3);
	int i = 0;
	int row;
	
	fprintf(stdout, "P6\n");
    fprintf(stdout, "%d %d\n255\n", 512, 512);
	
	for (row=0; row<512; row++) {
		if (row % 512 == 0) 
			fprintf(stderr, "[%d]", row);
		// read an entire row of 2bytes per pixel: 65536*2
		
		fread(rowin, 1, 512*3, stdin);
		fwrite(rowout, 1, 512*3, stdout);
/*		for (i=0; i<65536; i++) {
			rowout[i*3+0] = rowin[i*2+0];
			rowout[i*3+1] = rowin[i*2+1];
			rowout[i*3+2] = rowin[i*2+2];
		}
*/	
		
	}
	return 0;
}
