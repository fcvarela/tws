/*
 *  TerrainNode.h
 *  
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 *
 */

#ifndef _TWS_TERRAINNODE
#define _TWS_TERRAINNODE

#include <sys/time.h>

#include <list>
#include <pthread.h>

#include <Vector3.h>
#include <BlueMarbleTopo.h>
#include <Planet.h>

void *asyncInit(void *theNode);

namespace TWS {
    class Planet;

    class TerrainNode {
    private:
        // temp data
        GLuint *_indexes;
        GLdouble *_skirts[4];
        float *_skirt_tcoords[4];

        // properties
        bool _registered;

        // raw data
        uint8_t *_rawTopo;
        uint8_t *_rawTexture;
        uint8_t *_rawNormals;

        // texture data
        GLuint _texture;
        GLuint _normalmap;
        GLuint _heightmap;

        Planet *_planet;
        size_t temp_bps;

        void drawBoundingBox();
    public:
        // properties
        size_t _level, _cubeface;
        static const size_t _gridSize = 32;
        static const size_t _gridSizep1 = 33;
        Vector3d _center, _dx, _dy, _centervertex, _topleftvertex, _toprightvertex, _botleftvertex, _botrightvertex;
        double _boundingSphere[4], _boundingBox[8][3], _size;

        // vbos
        GLuint _vbo_verts;
        GLuint _vbo_texcoords;

        TerrainNode *_parent, *children[4];
        size_t _qtindex, _position;
        Vector3d _scaled_center;
        double _distance;
        double _lastDrawn;
        bool gl_ready;
        bool initialized;
        bool should_draw;

        TerrainNode(Planet *planet, TerrainNode *parent, size_t level, size_t position, size_t cubeface, Vector3d &center, Vector3d &dx, Vector3d &dy);
        ~TerrainNode();
        void asynch_init();
        void gl_init();

        static bool compareByTime(TerrainNode *a, TerrainNode *b);
        static bool compareByDistance(TerrainNode *a, TerrainNode *b);

        bool analyse();
        void draw();
        void loadChild(uint8_t childIndex);

        static std::list<TerrainNode *> nodeQueue;
    };
};

#endif
