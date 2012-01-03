/*
 *  FrustumTools.cpp
 *  TWS
 *
 *  Created by Filipe Varela on 08/10/18.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 *
 */

#include <iostream>
#include <cstdlib>
#include <stdint.h>
#include <math.h>

#include <FrustumTools.h>

namespace TWS {
	frustum_t globalFrustum;

	void extractPlane(plane_t &plane, GLfloat *mat, int row) {
		int scale = (row < 0) ? -1 : 1;
		row = abs(row) - 1;

		// calculate plane coefficients from the matrix
		plane.A = mat[3] + scale * mat[row];
		plane.B = mat[7] + scale * mat[row + 4];
		plane.C = mat[11] + scale * mat[row + 8];
		plane.D = mat[15] + scale * mat[row + 12];

		// normalize the plane
		double length = sqrtf(plane.A * plane.A + plane.B * plane.B + plane.C * plane.C);
		plane.A /= length;
		plane.B /= length;
		plane.C /= length;
		plane.D /= length;
	}

	// determines the current view frustum
	void calculateFrustum() {
		// get the projection and modelview matrices
		GLfloat projection[16];
		GLfloat modelview[16];

		glGetFloatv(GL_PROJECTION_MATRIX, projection);
		glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

		// use OpenGL to multiply them
		glPushMatrix();
		glLoadMatrixf(projection);
		glMultMatrixf(modelview);
		glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
		glPopMatrix();

		// extract each plane
		extractPlane(globalFrustum.l, modelview, 1);
		extractPlane(globalFrustum.r, modelview, -1);
		extractPlane(globalFrustum.b, modelview, 2);
		extractPlane(globalFrustum.t, modelview, -2);
		extractPlane(globalFrustum.n, modelview, 3);
		extractPlane(globalFrustum.f, modelview, -3);
	}

	// double *sphere is double[4]: xyz radius
	bool sphereInFrustum(double *sphere) {
		double dist;
		for (int i=0; i<6; i++) {
			dist = globalFrustum.planes[i].A * sphere[0] +
				globalFrustum.planes[i].B * sphere[1] +
				globalFrustum.planes[i].C * sphere[2] +
				globalFrustum.planes[i].D;
			if (dist < -sphere[3])
				return false;

			if (fabs(dist) < sphere[3])
				return true;
		}
		return true;
	}

	bool boxInFrustum(double *boundingBox) {
		double dist;
		for (uint8_t i=0; i<6; i++) {
			int InCount = 8;
			for (uint8_t k=0; k<8; k++) {
				// if box point outside plane, its out of frustum
				dist = globalFrustum.planes[i].A * boundingBox[k*3+0] +
					   globalFrustum.planes[i].B * boundingBox[k*3+1] +
					   globalFrustum.planes[i].C * boundingBox[k*3+2] +
					   globalFrustum.planes[i].D;

				if (dist < 0) --InCount;
				if (InCount == 0) return false;
			}
		}
		return true;
	}

	void lookAt(GLdouble eyex, GLdouble eyey, GLdouble eyez, GLdouble centerx, GLdouble centery, GLdouble centerz, GLdouble upx, GLdouble upy, GLdouble upz) {
		GLdouble m[16];
		GLdouble x[3], y[3], z[3];
		GLdouble mag;

		/* Make rotation matrix */
		/* Z vector */
		z[0] = eyex - centerx;
		z[1] = eyey - centery;
		z[2] = eyez - centerz;
		mag = sqrt(z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);
		if (mag) {
			/* mpichler, 19950515 */
			z[0] /= mag;
			z[1] /= mag;
			z[2] /= mag;
		}

		/* Y vector */
	    y[0] = upx;
		y[1] = upy;
		y[2] = upz;
		
 		/* X vector = Y cross Z */
		x[0] = y[1] * z[2] - y[2] * z[1];
		x[1] = -y[0] * z[2] + y[2] * z[0];
		x[2] = y[0] * z[1] - y[1] * z[0];
		
		/* Recompute Y = Z cross X */
		y[0] = z[1] * x[2] - z[2] * x[1];
		y[1] = -z[0] * x[2] + z[2] * x[0];
		y[2] = z[0] * x[1] - z[1] * x[0];
		
		/* mpichler, 19950515 */
		/* cross product gives area of parallelogram, which is < 1.0 for
		 * non-perpendicular unit-length vectors; so normalize x, y here
		*/
		mag = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
		if (mag) {
			x[0] /= mag;
			x[1] /= mag;
			x[2] /= mag;
		}
		
		mag = sqrt(y[0] * y[0] + y[1] * y[1] + y[2] * y[2]);
		if (mag) {
			y[0] /= mag;
			y[1] /= mag;
			y[2] /= mag;
		}

		#define M(row,col)  m[col*4+row]
		M(0, 0) = x[0];
		M(0, 1) = x[1];
		M(0, 2) = x[2];
		M(0, 3) = 0.0;
		M(1, 0) = y[0];
		M(1, 1) = y[1];
		M(1, 2) = y[2];
		M(1, 3) = 0.0;
		M(2, 0) = z[0];
		M(2, 1) = z[1];
		M(2, 2) = z[2];
		M(2, 3) = 0.0;
		M(3, 0) = 0.0;
		M(3, 1) = 0.0;
		M(3, 2) = 0.0;
		M(3, 3) = 1.0;
		#undef M

		glMultMatrixd(m);
		/* Translate Eye to Origin */
		glTranslatef(-eyex, -eyey, -eyez);
	}
}

