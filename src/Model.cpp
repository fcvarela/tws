/*
 *  Model.cpp
 *
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 */

#include <iostream>
#include <Model.h>
#include <Factory.h>

namespace TWS {
    Model::Model() {
        std::cerr << "TWS::Model intialized" << std::endl;
        // reset everything for sanity checks
        _faces = NULL;
        _facecount = 0;
        shader = new TWS::Shader("data/shaders/iss.glsl");
    }

    Model::~Model() {
        std::cerr << "TWS::Model dying" << std::endl;

        for (uint32_t i=0; i<_facecount; i++) {
            ModelFace_t *f = &_faces[i];
            free(f->vertices);
            free(f->normals);
            free(f->texcoords);
        }
    }

    void Model::draw() {
        shader->bind();
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);

        for (uint32_t i=0; i<_facecount; i++) {
            glVertexPointer(3, GL_FLOAT, 0, _faces[i].vertices);
            glNormalPointer(GL_FLOAT, 0, _faces[i].normals);
            glDrawArrays(GL_POLYGON, 0, _faces[i].component_count);
        }

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        shader->unbind();
    }

    void Model::drawMarker() {
        glPointSize(10.0);
        glColor4f(1.0, 0.0, 0.0, 1.0);
        glBegin(GL_POINTS);
        glVertex3fv(_faces[0].vertices);
        glEnd();
        glPointSize(1.0);
    }
}

