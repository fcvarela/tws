/*
 *  Astro.h
 *  TWS
 *
 *  Created by Filipe Varela on 08/10/12.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 *
 */

#include <cstdlib>
#include <Vector3.h>

namespace TWS_Astro
{
    #define JulianCentury 36525.0
    #define J2000 2451545.0
    #define fixangle(a) ((a) - 360.0 * (floor((a) / 360.0)))
    #define rtd(x) ((x) / (M_PI / 180.0))
    #define dtr(x) ((x) * (M_PI / 180.0))

    extern double G;

    void getsuncoords(double *coords);
    TWS::Vector3d getmarscoords(double *coords);
    void sunpos(double jd, int apparent, double *ra, double *dec, double *rv, double *slong);
    double juliandate();
    double ucttoj(long year, int mon, int mday, int hour, int min, int sec);
    double gmst(double jd);
}
