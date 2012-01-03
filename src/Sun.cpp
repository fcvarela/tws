/*
 *  Sun.cpp
 *
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 */

#include <iostream>
#include <sys/time.h>

#include <Factory.h>
#include <Astro.h>

namespace TWS{
    Sun::Sun() {
        std::cerr << "TWS::Sun initializing" << std::endl;

        // enable lighting
        GLfloat sunLightAmbientColor[4], sunLightDiffuseColor[4], sunLightSpecularColor[4];
        GLfloat sac[4] = {0.1,0.1,0.1,1.0}; memcpy(sunLightAmbientColor, sac, sizeof(sac));
        GLfloat sdc[4] = {1.0,1.0,1.0,1.0}; memcpy(sunLightDiffuseColor, sdc, sizeof(sdc));
        GLfloat ssc[4] = {1.0,1.0,1.0,1.0}; memcpy(sunLightSpecularColor, ssc, sizeof(ssc));

        double _radius = 6995.9E5;
        // initialize our sphere
        GLUquadric *sun = gluNewQuadric();
        gluQuadricNormals(sun, GL_SMOOTH);
        m_sphereList = glGenLists(1);
        glNewList(m_sphereList, GL_COMPILE);
        gluSphere(sun, _radius, 300, 300);
        glEndList();
        gluDeleteQuadric(sun);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glLightfv(GL_LIGHT0, GL_AMBIENT, sunLightAmbientColor);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, sunLightDiffuseColor);
        glLightfv(GL_LIGHT0, GL_SPECULAR, sunLightSpecularColor);

        double suncoords[2];
        TWS_Astro::getsuncoords(suncoords);
        float latitude = suncoords[1]+90.0;
        float longitude = -suncoords[0];
        double altitude = 149598E6;

        _position3d = Vector3d(
            cos(longitude * 0.0174532925) * cos(latitude * 0.0174532925) * altitude,
            sin(longitude * 0.0174532925) * cos(latitude * 0.0174532925) * altitude,
            sin(latitude * 0.0174532925) * altitude);

        _position[0] = _position3d.x();
        _position[1] = _position3d.y();
        _position[2] = _position3d.z();

        glLightfv(GL_LIGHT0, GL_POSITION, _position);

        _audioNode = new AudioNode("data/audio/a.wav");
        _audioNode->play();
        std::cerr << "TWS::Sun initialized" << std::endl;
    }

    Sun::~Sun() {std::cerr << "TWS::Sun dealloc" << std::endl;}

    void Sun::reposition() {
        double suncoords[2];
        TWS_Astro::getsuncoords(suncoords);

        float latitude = suncoords[1]+90.0;
        float longitude = -suncoords[0];
        double altitude = 149598E6;

        _position3d = Vector3d(
            cos(longitude * 0.0174532925) * cos(latitude * 0.0174532925) * altitude,
            sin(longitude * 0.0174532925) * cos(latitude * 0.0174532925) * altitude,
            sin(latitude * 0.0174532925) * altitude);

        _position[0] = _position3d.x();
        _position[1] = _position3d.y();
        _position[2] = _position3d.z();

        glLightfv(GL_LIGHT0, GL_POSITION, _position);

        Vector3d audioVec = Vector3d(0.0, 0.0, 12000000.0);
        _audioNode->setPosition(audioVec);
    }

    void Sun::draw() {
        glDisable(GL_LIGHTING);
        glPushMatrix();
        glTranslated(_position[0], _position[1], _position[2]);
        glColor4f(192.0, 191.0, 173.0, 1.0);
        glCallList(m_sphereList);
        glPopMatrix();
        glEnable(GL_LIGHTING);
    }
};
