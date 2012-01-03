/*
 *  untitled.h
 *  Orbits
 *
 *  Created by Filipe Varela on 10/12/12.
 *  Copyright 2010 Filipe Varela. All rights reserved.
 *
 */

typedef struct {
	double N, i, w, a, e, M, d;
} orbital_elements_t;

#define J2000 2451545.0

class CelestialBody {
public:
	orbital_elements_t orbital;

	double juliandate();
	double ucttoj(long year, int mon, int mday, int hour, int min, int sec);
	void position(double *pos);
};
