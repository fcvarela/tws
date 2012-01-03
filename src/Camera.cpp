/*
 *  Camera.cpp
 *
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 */

#include <Factory.h>
#include <Camera.h>
#include <FrustumTools.h>
#include <AudioNode.h>

namespace TWS {
    Camera::Camera() {
        Vector3d initialVelocity = Vector3d(0.0, 0.0, 0.0) * 7706.6; // avg ISS velocity in m/s
        Vector3d initialPosition = Vector3d(0.0, 0.0, 1.0) * (12000000.0); // 347 Km above surface

        setPosition(initialPosition);
        setVelocity(initialVelocity);
    }

    void Camera::setPerspective() {
        GLdouble mat[16];

        rotation().glMatrix(mat);
        glMultMatrixd(mat);
        glTranslatef(-position().x(), -position().y(), -position().z());

        TWS::calculateFrustum();

        // reposition OpenAL global listener
        alListener3f(AL_POSITION, position().x(), position().y(), position().z());
        alListener3f(AL_VELOCITY, 0.0, 0.0, 0.0);
        ALfloat v[6];

        v[0] = mat[2];
        v[1] = mat[6];
        v[2] = mat[10];
        v[3] = mat[1];
        v[4] = mat[5];
        v[5] = mat[9];

        alListenerfv(AL_ORIENTATION, v);
    }
}
