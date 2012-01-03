#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define PI 3.14159265
#define d2r(x) (x*0.0174532925)
#define r2d(x) (x/0.0174532925)

#define cubewidth 8192
#define origwidth 16384

void cylindric_latlon2xy(int64_t width, double lat, double lon, int64_t *x, int64_t *y);
void gnomonic_xy2latlon(int64_t width, int64_t x, int64_t y, double *lat, double *lon);
void gnomonic_polar_xy2latlon(int64_t width, int64_t x, int64_t y, double lat0, double lon0, double *lat, double *lon);
void parse_file(const char *filename, double lat0, double lon0);
void parse_polar_file(const char *filename, double lat0, double lon0);

typedef struct {
    uint8_t red, green, blue;
} pixel_t;

int main(int argc, char **argv) {
    parse_polar_file("0.pbm", 90.0, 0.0);
    parse_file("1.pbm", 0.0, -90.0);
    parse_file("2.pbm", 0.0, 0.0);
    parse_file("3.pbm", 0.0, 90.0);
    parse_file("4.pbm", 0.0, 180.0);
    parse_polar_file("5.pbm", -90.0, 0.0);

    return 0;
}

void parse_polar_file(const char *filename, double lat0, double lon0) {
    double lat, lon;
    int64_t x, y;
    int64_t temp_x, temp_y;

    fprintf(stderr, "Parsing file: %s\n", filename);

    pixel_t *bmp_cyl = (pixel_t *)malloc(sizeof(pixel_t) * origwidth * (origwidth/2));
    pixel_t *bmp_gno = (pixel_t *)malloc(sizeof(pixel_t) * cubewidth * cubewidth);

    FILE *sample = fopen("earth.ppm", "rb");
    FILE *out = fopen(filename, "wb");

    fseek(sample, 57L, SEEK_SET);

    // read entire cylindric projected image
    fread(bmp_cyl, sizeof(pixel_t), origwidth*(origwidth/2), sample);

    // try north pole gnomonic projection
    for (y=0; y<cubewidth; y++) {
        for (x=0; x<cubewidth; x++) {

            // figure out latitude and lontitude for this point
            gnomonic_polar_xy2latlon(cubewidth, x, y, lat0, lon0, &lat, &lon);

            // figure out xy for this latlon on cylindric projection
            cylindric_latlon2xy(origwidth, lat, lon, &temp_x, &temp_y);

            // copy pixel
            memcpy(&bmp_gno[y*cubewidth+x], &bmp_cyl[temp_y*origwidth + temp_x], sizeof(pixel_t));
        }
    }

    fprintf(out, "P6 %d %d 255\n", cubewidth, cubewidth);
    fwrite(bmp_gno, sizeof(pixel_t), cubewidth*cubewidth, out);

    free(bmp_cyl);
    free(bmp_gno);

    fclose(sample);
    fclose(out);

}

void parse_file(const char *filename, double lat0, double lon0) {
    double lat, lon;
    int64_t x, y;
    int64_t temp_x, temp_y;

    fprintf(stderr, "Parsing file: %s\n", filename);

    pixel_t *bmp_cyl = (pixel_t *)malloc(sizeof(pixel_t) * origwidth * (origwidth/2));
    pixel_t *bmp_gno = (pixel_t *)malloc(sizeof(pixel_t) * cubewidth * cubewidth);

    FILE *sample = fopen("earth.ppm", "rb");
    FILE *out = fopen(filename, "wb");

    fseek(sample, 57L, SEEK_SET);

    // read entire cylindric projected image
    fread(bmp_cyl, sizeof(pixel_t), origwidth*(origwidth/2), sample);

    // try north pole gnomonic projection
    for (y=0; y<cubewidth; y++) {
        for (x=0; x<cubewidth; x++) {
            // figure out latitude and lontitude for this point
            gnomonic_xy2latlon(cubewidth, x, y, &lat, &lon);

            // figure out xy for this latlon on cylindric projection
            cylindric_latlon2xy(origwidth, lat0+lat, lon0+lon, &temp_x, &temp_y);

            // copy pixel
            memcpy(&bmp_gno[y*cubewidth+x], &bmp_cyl[temp_y*origwidth + temp_x], sizeof(pixel_t));
        }
    }

    fprintf(out, "P6 %d %d 255\n", cubewidth, cubewidth);
    fwrite(bmp_gno, sizeof(pixel_t), cubewidth*cubewidth, out);

    free(bmp_cyl);
    free(bmp_gno);

    fclose(sample);
    fclose(out);
}

void cylindric_latlon2xy(int64_t width, double lat, double lon, int64_t *x, int64_t *y) {
    if (lat > 90.0) lat = 90.0 - (lat-90.0);
    if (lat <-90.0) lat = -90.0 + (fabs(lat)-90.0);
    if (lon > 180.0) lon = -180.0 + (lon-180.0);
    if (lon <-180.0) lon = 180.0 - (fabs(lon)-180.0);

    double lat1 = lat/180.0 + 0.5;
    double lon1 = lon/360.0 + 0.5;

    *y = lat1 * (width/2);
    *x = lon1 * width;
}

// convert a latitude longitude to a point in a gnomonic projection
void gnomonic_xy2latlon(int64_t width, int64_t xi, int64_t yi, double *lon, double *lat) {
    double y = 2.0*(yi-width/2.0) / (width);
    double x = 2.0*(xi-width/2.0) / (width);
    double lat0 = 0.0, lon0 = 0.0;

    if (xi == width/2 && yi == width/2) {
        *lat = lat0;
        *lon = lon0;

        return;
    }

    double dist = sqrt(x*x + y*y);
    double c = atan(dist);

    lat0 = d2r(lat0);
    lon0 = d2r(lon0);

    *lat = lat0 + atan((x*sin(c)) / (dist*cos(lon0)*cos(c)-y*sin(lon0)*sin(c)));
    *lon = asin(cos(c)*sin(lon0)+((y*sin(c)*cos(lon0))/dist));

    *lat = r2d(*lat);
    *lon = r2d(*lon);

    if (*lat > 90.0) *lat = 90.0 - (*lat-90.0);
    if (*lat <-90.0) *lat = -90.0 + (fabs(*lat)-90.0);
    if (*lon > 180.0) *lon = -180.0 + (*lon-180.0);
    if (*lon <-180.0) *lon = 180.0 - (fabs(*lon)-180.0);
}

// convert a latitude longitude to a point in a gnomonic projection
void gnomonic_polar_xy2latlon(int64_t width, int64_t xi, int64_t yi, double lon0, double lat0, double *lon, double *lat) {
    double y = 2.0*(yi-width/2.0) / (width);
    double x = 2.0*(xi-width/2.0) / (width);

    if (lon0 == 90.0) {
        if (yi <= width/2)
            x = -x;
    }
    if (lon0 ==-90.0) {
        if (yi < width/2)
            x = -x;
    }

    if (xi == width/2 && yi == width/2) {
        *lat = lat0;
        *lon = lon0;

        return;
    }

    double dist = sqrt(x*x + y*y);
    double c = atan(dist);

    lat0 = d2r(lat0);
    lon0 = d2r(lon0);

    *lat = lat0 + atan((x*sin(c)) / (dist*cos(lon0)*cos(c)-y*sin(lon0)*sin(c)));
    *lon = asin(cos(c)*sin(lon0)+((y*sin(c)*cos(lon0))/dist));

    *lat = r2d(*lat);
    *lon = r2d(*lon);

    lat0 = r2d(lat0);
    lon0 = r2d(lon0);

    if (lon0 == 90.0) {
        if (yi < width/2)
            *lat = 180.0 - *lat;
    }
    if (lon0 ==-90.0) {
        if (yi < width/2)
            *lat = 180.0 - *lat;
    }

}
