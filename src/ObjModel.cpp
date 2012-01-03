/*
 *  ObjModel.cpp
 *
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 */

#include <iostream>
#include <ObjModel.h>
#include <stdio.h>
#include <Vector3.h>
#include <stdlib.h>

namespace TWS {
	ObjModel::ObjModel() {
	}

	ObjModel::~ObjModel() {}

	bool ObjModel::load(const char *filename) {
		// open model file
		FILE *model = fopen(filename, "rb");
		if (model == NULL)
			return false;

		// read face count
		size_t len = fread(&_facecount, sizeof(_facecount), 1, model);
		std::cerr << "TWS::ObjModel got " << _facecount << " faces" << std::endl;

		// alloc memory for these faces
		_faces = (ModelFace_t *)malloc(sizeof(ModelFace_t) * _facecount);

		// read face data
		for (uint32_t i=0; i<_facecount; i++) {
			// get component count
			len = fread(&_faces[i].component_count, sizeof(GLuint), 1, model);

			// alloc memory for components
			_faces[i].vertices = (float *)malloc(sizeof(float) * _faces[i].component_count * 3);
			_faces[i].normals = (float *)malloc(sizeof(float) * _faces[i].component_count * 3);
			_faces[i].texcoords = (float *)malloc(sizeof(float) * _faces[i].component_count * 2);

			// read vertices, normals, texcoords
			len = fread(_faces[i].vertices, sizeof(float), 3*_faces[i].component_count, model);
			len = fread(_faces[i].normals, sizeof(float), 3*_faces[i].component_count, model);
			len = fread(_faces[i].texcoords, sizeof(float), 2*_faces[i].component_count, model);
		}

		fclose(model);
		return true;
	}
}

