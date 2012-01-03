/*
*  Planet.h
*  
*
*  Created by Filipe Varela on 08/10/13.
*  Copyright 2008 Filipe Varela. All rights reserved.
*
*/

#ifndef _TWS_PLANET
#define _TWS_PLANET

#include <vector>
#include <Shader.h>
#include <TerrainNode.h>

namespace TWS {
    class Planet {
    private:
        Shader *_shader;
        Shader *_atmosphere_shader;

        TerrainNode *top;
        TerrainNode *bottom;
        TerrainNode *left;
        TerrainNode *front;
        TerrainNode *right;
        TerrainNode *back;

        GLuint _detail;
        GLuint m_atmosphereList;

        Vector3f _rayleightWL;

    public:
        double _radius, _at_radius, _mass;
        size_t _maxlod;
        char _label[32];
        char _cachedir[255];
        Vector3d _position;

        Planet(const char *name, Vector3d &position, double radius, double mass, size_t maxlod);
        ~Planet();

        Shader *getShader() {return _shader;}

        void drawTerrain(void);
        void drawAtmosphere(void);

        Vector3d carthesianToGeocentric(Vector3d &position);
        Vector3d geocentricToCarthesian(float latitude, float longitude, float altitude);
    };
};

#endif
