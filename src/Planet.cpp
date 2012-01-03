/*
 *  Planet.cpp
 *
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 */

#include <iostream>
#include <algorithm>
#include <Factory.h>
#include <sys/time.h>
#include <Planet.h>
#include <SOIL.h>

namespace TWS{
    Planet::Planet(const char *name, Vector3d &position, double radius, double mass, size_t maxlod) {
        std::cerr << "TWS::Planet initializing" << std::endl;

        _position = position;
        _radius = radius;
        _at_radius = radius * 1.025;
        _mass = mass;
        _maxlod = maxlod;

        strcpy(_label, name);

        // get our atmospheric rayleigh wavelength
        _rayleightWL = Vector3f(
                pow(strtod(TWS::getConfig(_label, "rayleigh.wavelength.r").c_str(), NULL), 4),
                pow(strtod(TWS::getConfig(_label, "rayleigh.wavelength.g").c_str(), NULL), 4),
                pow(strtod(TWS::getConfig(_label, "rayleigh.wavelength.b").c_str(), NULL), 4));

        strcpy(_cachedir, TWS::getConfig(_label, "cachedir").c_str());

        // temp vectors needed to initialize each cube face
        Vector3d zero = Vector3d(0.0, 0.0, 0.0);
        Vector3d center = zero;
        Vector3d dx = zero;
        Vector3d dy = zero;

        // face 0 (top)
        center = Vector3d(0.0, 0.5, 0.0);
        dx = Vector3d(1.0, 0.0, 0.0);
        dy = Vector3d(0.0, 0.0, -1.0);
        top = new TerrainNode(this, NULL, _maxlod, 0, 0, center, dx, dy);

        // face 1 (left)
        center = Vector3d(-0.5, 0.0, 0.0);
        dx = Vector3d(0.0, 0.0, 1.0);
        dy = Vector3d(0.0, 1.0, 0.0);
        left = new TerrainNode(this, NULL, _maxlod, 0, 1, center, dx, dy);

        // face 2 (front)
        center = Vector3d(0.0, 0.0, 0.5);
        dx = Vector3d(1.0, 0.0, 0.0);
        dy = Vector3d(0.0, 1.0, 0.0);
        front = new TerrainNode(this, NULL, _maxlod, 0, 2, center, dx, dy);

        // face 3 (right)
        center = Vector3d(0.5, 0.0, 0.0);
        dx = Vector3d(0.0, 0.0, -1.0);
        dy = Vector3d(0.0, 1.0, 0.0);
        right = new TerrainNode(this, NULL, _maxlod, 0, 3, center, dx, dy);

        // face 4 (back)
        center = Vector3d(0.0, 0.0, -0.5);
        dx = Vector3d(-1.0, 0.0, 0.0);
        dy = Vector3d(0.0, 1.0, 0.0);
        back = new TerrainNode(this, NULL, _maxlod, 0, 4, center, dx, dy);

        // face 5 (bottom)
        center = Vector3d(0.0, -0.5, 0.0);
        dx = Vector3d(1.0, 0.0, 0.0);
        dy = Vector3d(0.0, 0.0, 1.0);
        bottom = new TerrainNode(this, NULL, _maxlod, 0, 5, center, dx, dy);

        // load shader
        _shader = new TWS::Shader("data/shaders/planet.glsl");
        _atmosphere_shader = new TWS::Shader("data/shaders/atmosphere.glsl");

        // load detail texture
        _detail = SOIL_load_OGL_texture("data/earth/detail.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
        glBindTexture(GL_TEXTURE_2D, _detail);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        // initialize atmosphere display list
        GLUquadric *atmosphere = gluNewQuadric();
        gluQuadricNormals(atmosphere, GL_SMOOTH);
        m_atmosphereList = glGenLists(1);
        glNewList(m_atmosphereList, GL_COMPILE);
        gluSphere(atmosphere, _at_radius, 300, 300);
        glEndList();
        gluDeleteQuadric(atmosphere);

        // make sure we initialize our time delta with a nice low value
        struct timeval now;
        gettimeofday(&now, 0);
        double a = now.tv_sec + (now.tv_usec/1000000.0);
        TWS::setTime(a);
        TWS::setTime(a); // yep, dup so dt = 0;

        std::cerr << "TWS::Planet initialized" << std::endl;
    }

    Vector3d Planet::carthesianToGeocentric(Vector3d &position) {
        double longitude = atan2(position.x(), position.z()) * 180.0/M_PI;
        double latitude = M_PI/2.0 - atan2(sqrt(position.z()*position.z() + position.x()*position.x()), position.y());
        latitude *= 180.0/M_PI;
        double altitude = sqrt(position.x()*position.x() + position.y()*position.y() + position.z()*position.z()) - _radius;
        Vector3d out = Vector3d(latitude, longitude, altitude);
        return out;
    }

    Vector3d Planet::geocentricToCarthesian(float latitude, float longitude, float altitude) {
        double position[3];

        double lat = latitude;
        double lon = longitude;
        double alt = altitude + _radius;

        position[2] = cos(lon * 0.0174532925) * cos(lat * 0.0174532925) * alt;
        position[0] = sin(lon * 0.0174532925) * cos(lat * 0.0174532925) * alt;
        position[1] = sin(lat * 0.0174532925) * alt;

        Vector3d out = Vector3d(position[0], position[1], position[2]);
        return out;
    }

    Planet::~Planet() {
        delete top;
        delete bottom;
        delete left;
        delete right;
        delete front;
        delete back;
        std::cerr << "TWS::Planet dealloc" << std::endl;
    }

    void Planet::drawTerrain() {
        glPushMatrix();
        glTranslated(_position.x(), _position.y(), _position.z());

        // bind the detail texture to unit 5
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, _detail);

        // bind the shader
        _shader->bind();

        // get camera and sun positions
        Vector3d cpd(TWS::getCamera()->position() - _position);
        Vector3f cpf(cpd.x(), cpd.y(), cpd.z());

        Vector3<double> spd = TWS::getSun()->_position3d.normalized();
        Vector3f spf(spd.x(), spd.y(), spd.z());

        Vector3f iwl(1.0/_rayleightWL.x(), 1.0/_rayleightWL.y(), 1.0/_rayleightWL.z());

        // pass data do shader
        glUniform1i(glGetUniformLocation(_shader->getProgram(), "terrainDetailTexture"), 3);
        glUniform3fv(glGetUniformLocation(_shader->getProgram(), "v3CameraPos"), 1, cpf);
        glUniform3fv(glGetUniformLocation(_shader->getProgram(), "v3LightPos"), 1, spf);
        glUniform3fv(glGetUniformLocation(_shader->getProgram(), "v3InvWavelength"), 1, iwl);
        glUniform1f(glGetUniformLocation(_shader->getProgram(), "fInnerRadius"), _radius);
        glUniform1f(glGetUniformLocation(_shader->getProgram(), "fOuterRadius"), _at_radius);
        glUniform1f(glGetUniformLocation(_shader->getProgram(), "fInnerRadius2"), _radius*_radius);
        glUniform1f(glGetUniformLocation(_shader->getProgram(), "fOuterRadius2"), _at_radius*_at_radius);
        glUniform1f(glGetUniformLocation(_shader->getProgram(), "fCameraHeight"), cpf.length());
        glUniform1f(glGetUniformLocation(_shader->getProgram(), "fCameraHeight2"), cpf.length()*cpf.length());
        glUniform1f(glGetUniformLocation(_shader->getProgram(), "fKrESun"), 0.0025f * 15.0f);
        glUniform1f(glGetUniformLocation(_shader->getProgram(), "fKmESun"), 0.0015f * 15.0f);
        glUniform1f(glGetUniformLocation(_shader->getProgram(), "fKr4PI"), 0.0025f*4.0f*M_PI);
        glUniform1f(glGetUniformLocation(_shader->getProgram(), "fKm4PI"), 0.0015f*4.0f*M_PI);
        glUniform1f(glGetUniformLocation(_shader->getProgram(), "fScale"), 1.0f / (_at_radius - _radius));
        glUniform1f(glGetUniformLocation(_shader->getProgram(), "fScaleDepth"), 0.25f);
        glUniform1f(glGetUniformLocation(_shader->getProgram(), "fScaleOverScaleDepth"), (1.0f / (_at_radius - _radius)) / 0.25f);
        glUniform1f(glGetUniformLocation(_shader->getProgram(), "fSamples"), 2.0f);
        glUniform1i(glGetUniformLocation(_shader->getProgram(), "nSamples"), 2);

        // process each quadtree
        top->analyse();
        left->analyse();
        front->analyse();
        right->analyse();
        back->analyse();
        bottom->analyse();

        _shader->unbind();
        glPopMatrix();
    }

    void Planet::drawAtmosphere() {
        glPushMatrix();
        glTranslated(_position.x(), _position.y(), _position.z());

        // we need to draw the back facing face of the sphere
        glFrontFace(GL_CW);

        // bind the shader
        _atmosphere_shader->bind();

        // get camera and sun positions
        Vector3d cpd(TWS::getCamera()->position() - _position);
        Vector3f cpf(cpd.x(), cpd.y(), cpd.z());

        Vector3d spd = TWS::getSun()->_position3d.normalized();
        Vector3f spf(spd.x(), spd.y(), spd.z());

        Vector3f wl(powf(0.650f, 4.0), powf(0.570f, 4.0), powf(0.475f, 4.0));
        Vector3f iwl(1.0/wl[0], 1.0/wl[1], 1.0/wl[2]);

        // pass data to shader
        glUniform3fv(glGetUniformLocation(_atmosphere_shader->getProgram(), "v3CameraPos"), 1, cpf);
        glUniform3fv(glGetUniformLocation(_atmosphere_shader->getProgram(), "v3LightPos"), 1, spf);
        glUniform3fv(glGetUniformLocation(_atmosphere_shader->getProgram(), "v3InvWavelength"), 1, iwl);
        glUniform1f(glGetUniformLocation(_atmosphere_shader->getProgram(), "fInnerRadius"), _radius);
        glUniform1f(glGetUniformLocation(_atmosphere_shader->getProgram(), "fOuterRadius"), _at_radius);
        glUniform1f(glGetUniformLocation(_atmosphere_shader->getProgram(), "fInnerRadius2"), _radius*_radius);
        glUniform1f(glGetUniformLocation(_atmosphere_shader->getProgram(), "fOuterRadius2"), _at_radius*_at_radius);
        glUniform1f(glGetUniformLocation(_atmosphere_shader->getProgram(), "fCameraHeight"), cpf.length());
        glUniform1f(glGetUniformLocation(_atmosphere_shader->getProgram(), "fCameraHeight2"), cpf.length()*cpf.length());
        glUniform1f(glGetUniformLocation(_atmosphere_shader->getProgram(), "fKrESun"), 0.0025f * 15.0f);
        glUniform1f(glGetUniformLocation(_atmosphere_shader->getProgram(), "fKmESun"), 0.0015f * 15.0f);
        glUniform1f(glGetUniformLocation(_atmosphere_shader->getProgram(), "fKr4PI"), 0.0025f*4.0f*M_PI);
        glUniform1f(glGetUniformLocation(_atmosphere_shader->getProgram(), "fKm4PI"), 0.0015f*4.0f*M_PI);
        glUniform1f(glGetUniformLocation(_atmosphere_shader->getProgram(), "fScale"), 1.0f / (_at_radius - _radius));
        glUniform1f(glGetUniformLocation(_atmosphere_shader->getProgram(), "fScaleDepth"), 0.25f);
        glUniform1f(glGetUniformLocation(_atmosphere_shader->getProgram(), "fScaleOverScaleDepth"), (1.0f / (_at_radius - _radius)) / 0.25f);
        glUniform1f(glGetUniformLocation(_atmosphere_shader->getProgram(), "fSamples"), 2.0f);
        glUniform1i(glGetUniformLocation(_atmosphere_shader->getProgram(), "nSamples"), 2);

        // run the display list
        glCallList(m_atmosphereList);
        // unbind the shader
        _atmosphere_shader->unbind();

        // reset front face
        glFrontFace(GL_CCW);
        glPopMatrix();
    }
};
