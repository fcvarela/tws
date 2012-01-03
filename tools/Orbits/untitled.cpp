/*
 *  untitled.cpp
 *  Orbits
 *
 *  Created by Filipe Varela on 10/12/12.
 *  Copyright 2010 Filipe Varela. All rights reserved.
 *
 */

#include <time.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>

#include "untitled.h"

static double add = 0.0;

double CelestialBody::juliandate() {
	time_t now;
	struct tm *ts;
	now = time(NULL);
	ts = localtime(&now);
	
	double y = ts->tm_year+1900;
	double m = ts->tm_mon+1;
	double D = ts->tm_mday;
	
	double result = 367*y-7*(y+(m+9)/12)/4+275*m/9+D-730530;
	result += (ts->tm_hour + ts->tm_min/60.0 + ts->tm_sec/60.0/60.0)/24.0;
	add += 0.01;
	printf("Date: %f\n", result+add);
	return result+add;
}

void CelestialBody::position(double *pos) {
	double prev_E = 0.0;
	double final_E = 0.0;
	double diff = 1.0;
	
	prev_E = orbital.M + orbital.e * sin(orbital.M) * (1.0 + orbital.e * cos(orbital.M));
	while (diff > 0.001) {
		final_E = prev_E - (prev_E - orbital.e * sin(prev_E) - orbital.M) / (1 - orbital.e * cos(prev_E));
		diff = fabs(final_E - prev_E);
		prev_E = final_E;
	}
	
	double E = final_E;
	double xv = orbital.a * (cos(E) - orbital.e);
	double yv = orbital.a * (sqrt(1.0 - orbital.e*orbital.e) * sin(E));
	double v = atan2(yv, xv);
	double r = sqrt(xv*xv + yv*yv);
	
	double xh = r * (cos(orbital.N) * cos(v+orbital.w) - sin(orbital.N) * sin(v+orbital.w) * cos(orbital.i));
	double yh = r * (sin(orbital.N) * cos(v+orbital.w) + cos(orbital.N) * sin(v+orbital.w) * cos(orbital.i));
	double zh = r * (sin(v+orbital.w) * sin(orbital.i));
	
	pos[0] = xh;
	pos[1] = yh;
	pos[2] = zh;
}