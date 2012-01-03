/*
 * TerrainNode.cpp
 *
 *
 * Created by Filipe Varela on 08/10/13.
 * Copyright 2008 Filipe Varela. All rights reserved.
 */

#include <iostream>
#include <vector>

#include <Factory.h>
#include <TerrainNode.h>
#include <FrustumTools.h>
#include <BlueMarbleTopo.h>
#include <TerrainManager.h>

namespace TWS {
    TerrainNode::TerrainNode(Planet *planet, TerrainNode *parent, size_t level, size_t position, size_t cubeface, Vector3d &center, Vector3d &dx, Vector3d &dy) {
        _planet = planet;
        _level = level;
        _position = position;
        _cubeface = cubeface;

        if (parent) _qtindex = parent->_qtindex * 10 + position;
        else _qtindex = 0;

        // parent
        _parent = parent;

        // registered in render queue
        _registered = false;

        // reset draw date
        struct timeval now;
        gettimeofday(&now, 0);
        _lastDrawn = now.tv_sec + (now.tv_usec/1000000.0);

        // copy direction, center vectors
        _center = center;
        _dx = dx;
        _dy = dy;

        // cleanup children
        children[0] = NULL;
        children[1] = NULL;
        children[2] = NULL;
        children[3] = NULL;

        gl_ready = false;
        initialized = false;

        // make sure everything is clean
        _vbo_verts = 0;
        _vbo_texcoords = 0;
        _indexes = NULL;
        _texture = 0;
        _normalmap = 0;

        for (uint8_t i=0; i<4; i++) {
            _skirts[i] = NULL;
            _skirt_tcoords[i] = NULL;
        }

        _scaled_center = _center;
        _scaled_center.normalize();
        _scaled_center = _scaled_center * _planet->_radius;
        
        // add to load list
        std::cerr << "TWS::TerrainNode loading: " << _cubeface << "/" << _qtindex << std::endl;
        TWS::getTerrainManager()->requestLoad(this);
    }

    void TerrainNode::asynch_init() {
        // load topography for our vertices
        uint16_t tex_size = min(32*pow(2, _level), 512);
        size_t bytes_per_sample = temp_bps = (_planet->_maxlod == 8) ? 3 : 4;

        _rawTopo = (uint8_t *)malloc(sizeof(uint8_t)*tex_size*tex_size*2);
        _rawTexture = (uint8_t *)malloc(sizeof(uint8_t)*tex_size*tex_size*bytes_per_sample);
        _rawNormals = (uint8_t *)malloc(sizeof(uint8_t)*tex_size*tex_size*3);

        BlueMarbleTopo::readTopography(_planet, _cubeface, _qtindex, _level, _rawTopo);
        BlueMarbleTopo::readTextures(_planet, _cubeface, _qtindex, _level, _rawTexture);
        BlueMarbleTopo::readNormals(_planet, _cubeface, _qtindex, _level, _rawNormals);

        initialized = true;
    }

    void TerrainNode::gl_init() {
        uint16_t tex_size = min(32*pow(2, _level), 512);
        size_t bytes_per_sample = temp_bps;
        std::cerr << "TWS::TerrainNode bytes per sample is: " << bytes_per_sample << std::endl;
        
        // BUILD TEXTURE
        glGenTextures(1, &_texture);
        glBindTexture(GL_TEXTURE_2D, _texture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        glTexImage2D(GL_TEXTURE_2D, 0, bytes_per_sample==4?GL_RGBA:GL_RGB, tex_size, tex_size, 0, bytes_per_sample==4?GL_RGBA:GL_RGB, GL_UNSIGNED_BYTE, _rawTexture);

        // BUILD NORMALS
        glGenTextures(1, &_normalmap);
        glBindTexture(GL_TEXTURE_2D, _normalmap);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_size, tex_size, 0, GL_RGB, GL_UNSIGNED_BYTE, _rawNormals);

        // BUILD TOPOGRAPHY
        glGenTextures(1, &_heightmap);
        glBindTexture(GL_TEXTURE_2D, _heightmap);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA16, tex_size, tex_size, 0, GL_ALPHA, GL_UNSIGNED_SHORT, _rawTopo);

        // initialize vertices
        GLdouble *vertices = (GLdouble *)malloc(_gridSizep1*_gridSizep1*3*sizeof(GLdouble));
        GLfloat *texcoords = (GLfloat *)malloc(_gridSizep1*_gridSizep1*3*sizeof(GLfloat));

        uint8_t u = 0;
        uint8_t v = 0;

        // vars for bounding box for this node
        double maxx=0, maxy=0, maxz=0;
        double minx=0, miny=0, minz=0;

        for (u=0; u<(_gridSizep1); u++) {
            for (v=0; v<(_gridSizep1); v++) {
                Vector3d coord = _center +
                    (_dx/(double)_gridSize) * ((double)v - (double)_gridSize / 2.0) +\
                    (_dy/(double)_gridSize) * ((double)_gridSize / 2.0 - (double)u);

                coord.normalize();

                float scale = 1.0/(tex_size-2.0);
                texcoords[u*_gridSizep1*3 + v*3 + 0] = scale + (float)v * ((float)tex_size-2.0) / 32.0 / (float)tex_size;
                texcoords[u*_gridSizep1*3 + v*3 + 1] = scale + (float)u * ((float)tex_size-2.0) / 32.0 / (float)tex_size;

                coord *= _planet->_radius;
                vertices[u*_gridSizep1*3 + v*3 + 0] = coord.x();
                vertices[u*_gridSizep1*3 + v*3 + 1] = coord.y();
                vertices[u*_gridSizep1*3 + v*3 + 2] = coord.z();

                coord += _planet->_position;

                if (u == 0 && v == 0) _topleftvertex = coord;
                if (u == 0 && v == _gridSize) _toprightvertex = coord;
                if (u == _gridSize/2 && v == _gridSize/2) _centervertex = coord;
                if (u == _gridSize && v == 0) _botleftvertex = coord;
                if (u == _gridSize && u == _gridSize) _botrightvertex = coord;

                if (u == 0 && v == 0) {
                    maxx = minx = coord.x();
                    maxy = miny = coord.y();
                    maxz = minz = coord.z();
                }

                maxx = max(maxx, coord.x()); maxy = max(maxy, coord.y()); maxz = max(maxz, coord.z());
                minx = min(minx, coord.x()); miny = min(miny, coord.y()); minz = min(minz, coord.z());
            }
        }

        // calculate our boundingsphere
        _size = (_topleftvertex - _centervertex).length();
        _boundingSphere[0] = _centervertex.x();
        _boundingSphere[1] = _centervertex.y();
        _boundingSphere[2] = _centervertex.z();
        _boundingSphere[3] = _size;

        // calculate our boundingbox
        _boundingBox[0][0] = minx; _boundingBox[0][1] = maxy; _boundingBox[0][2] = minz;
        _boundingBox[1][0] = minx; _boundingBox[1][1] = maxy; _boundingBox[1][2] = maxz;

        _boundingBox[2][0] = maxx; _boundingBox[2][1] = maxy; _boundingBox[2][2] = minz;
        _boundingBox[3][0] = maxx; _boundingBox[3][1] = maxy; _boundingBox[3][2] = maxz;

        _boundingBox[4][0] = minx; _boundingBox[4][1] = miny; _boundingBox[4][2] = minz;
        _boundingBox[5][0] = minx; _boundingBox[5][1] = miny; _boundingBox[5][2] = maxz;

        _boundingBox[6][0] = maxx; _boundingBox[6][1] = miny; _boundingBox[6][2] = minz;
        _boundingBox[7][0] = maxx; _boundingBox[7][1] = miny; _boundingBox[7][2] = maxz;

        // alloc triangle strip buffer to pass to OpenGL
        int vertcount = _gridSizep1*_gridSize*2;
        _indexes = (GLuint *)malloc(sizeof(GLuint) * vertcount);

        int coord_index = 0;

        // build triangle strips from vertex array
        for (uint8_t i=0; i<_gridSize; i++) {
            for (uint8_t j=0; j<_gridSizep1; j++) {
                // select vertices for this offset
                _indexes[coord_index++] = i*_gridSizep1 + j;
                _indexes[coord_index++] = (i+1)*_gridSizep1 + j;
            }
        }

        // build skirts
        coord_index = 0;
        for (int8_t i=0; i<4; i++) {
            _skirts[i] = (GLdouble *)malloc(sizeof(GLdouble) * (_gridSizep1+1) * 3);
            _skirt_tcoords[i] = (float *)malloc(sizeof(float) * (_gridSizep1+1) * 3);
            _skirts[i][coord_index++] = 0.0;
            _skirts[i][coord_index++] = 0.0;
            _skirts[i][coord_index++] = 0.0;
            _skirt_tcoords[i][coord_index-3] = 0.0;
            _skirt_tcoords[i][coord_index-3] = 0.0;
        }

        // left column
        coord_index = 3;
        for (int8_t y=_gridSize; y>=0; y--) {
            _skirts[0][coord_index++] = vertices[y*_gridSizep1*3 + 0];
            _skirts[0][coord_index++] = vertices[y*_gridSizep1*3 + 1];
            _skirts[0][coord_index++] = vertices[y*_gridSizep1*3 + 2];
            _skirt_tcoords[0][coord_index-3] = texcoords[y*_gridSizep1*3 + 0];
            _skirt_tcoords[0][coord_index-2] = texcoords[y*_gridSizep1*3 + 1];
        }

        // right column
        coord_index = 3;
        for (int8_t y=0; y<(int)_gridSizep1; y++) {
            _skirts[1][coord_index++] = vertices[y*_gridSizep1*3 + _gridSize*3 + 0];
            _skirts[1][coord_index++] = vertices[y*_gridSizep1*3 + _gridSize*3 + 1];
            _skirts[1][coord_index++] = vertices[y*_gridSizep1*3 + _gridSize*3 + 2];
            _skirt_tcoords[1][coord_index-3] = texcoords[y*_gridSizep1*3 + _gridSize*3 + 0];
            _skirt_tcoords[1][coord_index-2] = texcoords[y*_gridSizep1*3 + _gridSize*3 + 1];
        }

        // top row
        coord_index = 3;
        for (int8_t y=0; y<(int)_gridSizep1; y++) {
            _skirts[2][coord_index++] = vertices[y*3 + 0];
            _skirts[2][coord_index++] = vertices[y*3 + 1];
            _skirts[2][coord_index++] = vertices[y*3 + 2];
            _skirt_tcoords[2][coord_index-3] = texcoords[y*3 + 0];
            _skirt_tcoords[2][coord_index-2] = texcoords[y*3 + 1];
        }

        // bottom row
        coord_index = 3;
        for (int8_t y=_gridSize; y>=0; y--) {
            _skirts[3][coord_index++] = vertices[32*_gridSizep1*3 + y*3 + 0];
            _skirts[3][coord_index++] = vertices[32*_gridSizep1*3 + y*3 + 1];
            _skirts[3][coord_index++] = vertices[32*_gridSizep1*3 + y*3 + 2];
            _skirt_tcoords[3][coord_index-3] = texcoords[32*_gridSizep1*3 + y*3 + 0];
            _skirt_tcoords[3][coord_index-2] = texcoords[32*_gridSizep1*3 + y*3 + 1];
        }

        // init VBO
        glGenBuffers(1, &_vbo_verts);
        glGenBuffers(1, &_vbo_texcoords);

        // alloc vram for vbo
        GLuint vertex_count = (_gridSizep1 * _gridSizep1);
        GLuint vert_buff_size = vertex_count * 3 * sizeof(GLdouble);
        GLuint tex_buff_size = vertex_count * 3 * sizeof(GLfloat);

        glBindBuffer(GL_ARRAY_BUFFER, _vbo_verts);
        glBufferData(GL_ARRAY_BUFFER, vert_buff_size, vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, _vbo_texcoords);
        glBufferData(GL_ARRAY_BUFFER, tex_buff_size, texcoords, GL_STATIC_DRAW);

        // cleanup temp stuff
        free(vertices);
        free(texcoords);
        free(_rawTexture);
        free(_rawNormals);
        free(_rawTopo);

        gl_ready = true;
    }

    bool TerrainNode::analyse() {
        if (!initialized)
            return false;

        if (!gl_ready)
            gl_init();

        // node in frustum?
        if (!TWS::boxInFrustum(&_boundingBox[0][0]))
            return false;

        Camera *camera = TWS::getCamera();
        double distance1 = (_centervertex - camera->position()).length();
        double distance2 = (_topleftvertex - camera->position()).length();
        double distance3 = (_toprightvertex - camera->position()).length();
        double distance4 = (_botleftvertex - camera->position()).length();
        double distance5 = (_botrightvertex - camera->position()).length();
        _distance = min(distance1, min(distance2, min(distance3, min(distance4, distance5))));

        // delta: our geometric error delta is pow(2, lod)
        // the screen space error is simply: rho = (delta/distance) * K
        // K = viewportwidth/(2tan(horiz_fov/2))
        double delta = pow(2.0, _level);
        double k = TWS::hsiz / (2.0 * tan(TWS::hfov*0.5 * (M_PI/180.0)));
        double rho = k * delta / _distance;
        if (rho > (1.0/25.0) && _level > 0) {
            // request residency of our children
            if (children[0] == NULL) loadChild(0);
            if (children[1] == NULL) loadChild(1);
            if (children[2] == NULL) loadChild(2);
            if (children[3] == NULL) loadChild(3);

            // if they're _ALL_ ready to draw, go ahead. else draw ourself
            if (children[0]->initialized && children[1]->initialized && children[2]->initialized && children[3]->initialized) {
                children[0]->analyse();
                children[1]->analyse();
                children[2]->analyse();
                children[3]->analyse();
                return false;
            }
        }
        
        // got here? do not draw children, right? then check if they're meant to be deallocated
        for (uint i=0; i<4; i++) {
            if (children[i] != NULL && children[i]->_lastDrawn + 60.0 < TWS::dtime) {
                // and only on bottom of tree
                if (children[i]->children[0] == NULL && children[i]->children[1] == NULL && children[i]->children[2] == NULL && children[i]->children[3] == NULL) {
                    delete children[i];
                    children[i] = NULL;
                }
            }
        }

        // if cam is inside atmosphere and planet curvature occludes node, return
        double cull_distance = (camera->position() - _centervertex).length() - _boundingSphere[3];
        if (cull_distance < 0.0) cull_distance = 0.0;
        if (cull_distance > TWS::fardist && camera->position().length() < _planet->_radius*1.025)
            return false;

        draw();
        return true;
    }

    void TerrainNode::draw() {
        // insert last drawn timestamp here
        _lastDrawn = TWS::dtime;
        
        // draw self
        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, _texture);
        glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, _normalmap);
        glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, _heightmap);

        GLuint shaderProgram = _planet->getShader()->getProgram();
        glUniform1i(glGetUniformLocation(shaderProgram, "skirts"), 0);
        glUniform1i(glGetUniformLocation(shaderProgram, "lod"), _level);
        glUniform1i(glGetUniformLocation(shaderProgram, "terrainTexture"), 0);
        glUniform1i(glGetUniformLocation(shaderProgram, "normalTexture"), 1);
        glUniform1i(glGetUniformLocation(shaderProgram, "heightTexture"), 2);

        glEnableClientState(GL_VERTEX_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo_verts);
        glVertexPointer(3, GL_DOUBLE, 0, 0);

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo_texcoords);
        glTexCoordPointer(3, GL_FLOAT, 0, 0);

        for (uint8_t i=0; i<_gridSize; i++) {
            int begin = i*2*_gridSizep1;
            glDrawElements(GL_TRIANGLE_STRIP, _gridSizep1*2, GL_UNSIGNED_INT, &_indexes[begin]);
        }

        // unbind vbos
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // draw strips
        // left column
        glUniform1i(glGetUniformLocation(shaderProgram, "skirts"), 1);
        for (uint8_t i=0; i<4; i++) {
            glVertexPointer(3, GL_DOUBLE, 0, _skirts[i]);
            glTexCoordPointer(2, GL_FLOAT, 12, _skirt_tcoords[i]);
            glDrawArrays(GL_TRIANGLE_FAN, 0, _gridSizep1+1);
        }

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    void TerrainNode::drawBoundingBox() {
        // draw bounding boxes
        glBegin(GL_LINE_STRIP);
        glVertex3f(_boundingBox[0][0], _boundingBox[0][1], _boundingBox[0][2]);
        glVertex3f(_boundingBox[1][0], _boundingBox[1][1], _boundingBox[1][2]);
        glVertex3f(_boundingBox[3][0], _boundingBox[3][1], _boundingBox[3][2]);
        glVertex3f(_boundingBox[2][0], _boundingBox[2][1], _boundingBox[2][2]);
        glVertex3f(_boundingBox[0][0], _boundingBox[0][1], _boundingBox[0][2]);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(_boundingBox[6][0], _boundingBox[6][1], _boundingBox[6][2]);
        glVertex3f(_boundingBox[7][0], _boundingBox[7][1], _boundingBox[7][2]);
        glVertex3f(_boundingBox[3][0], _boundingBox[3][1], _boundingBox[3][2]);
        glVertex3f(_boundingBox[2][0], _boundingBox[2][1], _boundingBox[2][2]);
        glVertex3f(_boundingBox[6][0], _boundingBox[6][1], _boundingBox[6][2]);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(_boundingBox[3][0], _boundingBox[3][1], _boundingBox[3][2]);
        glVertex3f(_boundingBox[1][0], _boundingBox[1][1], _boundingBox[1][2]);
        glVertex3f(_boundingBox[5][0], _boundingBox[5][1], _boundingBox[5][2]);
        glVertex3f(_boundingBox[7][0], _boundingBox[7][1], _boundingBox[7][2]);
        glVertex3f(_boundingBox[3][0], _boundingBox[3][1], _boundingBox[3][2]);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(_boundingBox[0][0], _boundingBox[0][1], _boundingBox[0][2]);
        glVertex3f(_boundingBox[1][0], _boundingBox[1][1], _boundingBox[1][2]);
        glVertex3f(_boundingBox[5][0], _boundingBox[5][1], _boundingBox[5][2]);
        glVertex3f(_boundingBox[4][0], _boundingBox[4][1], _boundingBox[4][2]);
        glVertex3f(_boundingBox[0][0], _boundingBox[0][1], _boundingBox[0][2]);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(_boundingBox[4][0], _boundingBox[4][1], _boundingBox[4][2]);
        glVertex3f(_boundingBox[6][0], _boundingBox[6][1], _boundingBox[6][2]);
        glVertex3f(_boundingBox[7][0], _boundingBox[7][1], _boundingBox[7][2]);
        glVertex3f(_boundingBox[5][0], _boundingBox[5][1], _boundingBox[5][2]);
        glVertex3f(_boundingBox[4][0], _boundingBox[4][1], _boundingBox[4][2]);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(_boundingBox[0][0], _boundingBox[0][1], _boundingBox[0][2]);
        glVertex3f(_boundingBox[2][0], _boundingBox[2][1], _boundingBox[2][2]);
        glVertex3f(_boundingBox[6][0], _boundingBox[6][1], _boundingBox[6][2]);
        glVertex3f(_boundingBox[4][0], _boundingBox[4][1], _boundingBox[4][2]);
        glVertex3f(_boundingBox[0][0], _boundingBox[0][1], _boundingBox[0][2]);
        glEnd();
    }

    void TerrainNode::loadChild(uint8_t childIndex) {
        Vector3d center, dx, dy;

        dx = _dx/2.0;
        dy = _dy/2.0;

        if (children[childIndex] != NULL)
            delete(children[childIndex]);

        switch (childIndex) {
            case 0: center = _center - _dx/4.0 + _dy/4.0; break;
            case 1: center = _center + _dx/4.0 + _dy/4.0; break;
            case 2: center = _center - _dx/4.0 - _dy/4.0; break;
            case 3: center = _center + _dx/4.0 - _dy/4.0; break;
        }

        children[childIndex] = new TerrainNode(_planet, this, _level-1, childIndex+1, _cubeface, center, dx, dy);
    }

    TerrainNode::~TerrainNode() {
        std::cerr << "TWS::TerrainNode dealloc: " << _cubeface << "/" << _qtindex << std::endl;

        // dealloc our data IF we're inited and it has actually been alloc'd
        if (_indexes != NULL)
            free(_indexes);
        
        for (uint8_t i=0; i<4; i++) {
            free(_skirts[i]);
            free(_skirt_tcoords[i]);
        }

        // not sure about this one
        glDeleteTextures(1, &_texture);
        glDeleteTextures(1, &_normalmap);
        glDeleteTextures(1, &_heightmap);
        glDeleteBuffers(1, &_vbo_verts);
        glDeleteBuffers(1, &_vbo_texcoords);
    }

    // implement comparison for node list sorting
    bool TerrainNode::compareByTime(TerrainNode *a, TerrainNode *b) {
        return (a->_lastDrawn > b->_lastDrawn);
    }

    bool TerrainNode::compareByDistance(TerrainNode *a, TerrainNode *b) {
        Camera *c = TWS::getCamera();
        double distance_a = (c->position() - a->_scaled_center).length();
        double distance_b = (c->position() - b->_scaled_center).length();

        a->_distance = distance_a;
        b->_distance = distance_b;

        return (a->_distance < b->_distance);
    }
};
