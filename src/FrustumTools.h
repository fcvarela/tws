/*
 *  FrustumTools.h
 *  TWS
 *
 *  Created by Filipe Varela on 08/10/18.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 *
 */

#include <GL.h>

namespace TWS {
    struct plane_t
    {
        GLfloat A, B, C, D;
    };

    union frustum_t
    {
        struct
        {
            plane_t t, b, l, r, n, f;
        };
        plane_t planes[6];
    };

    void lookAt(GLdouble eyex, GLdouble eyey, GLdouble eyez, GLdouble centerx, GLdouble centery, GLdouble centerz, GLdouble upx, GLdouble upy, GLdouble upz);
    void extractPlane(plane_t &plane, GLfloat *mat, int row);
    void calculateFrustum();
    bool sphereInFrustum(double *sphere);
    bool boxInFrustum(double *boundingBox);
}
