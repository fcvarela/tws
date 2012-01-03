/*
 *  Model.h
 *  
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 *
 */

#ifndef _TWS_MODEL
#define _TWS_MODEL

#include <Shader.h>

// models are simple mesh definitions. a drawable scene node may contain an object
// objects have no business knowing owner properties (position, velocity, whatnot)

namespace TWS
{
	typedef struct {
		uint32_t component_count;
		float *normals;
		float *vertices;
		float *texcoords;
	} ModelFace_t;

	class Model {
	protected:
		uint32_t _facecount;
		ModelFace_t *_faces;
		
	public:
		Model();
		~Model();
		Shader *shader;

		void draw();
		void drawMarker();
		virtual bool load(const char *filename) = 0;
	};
};

#endif
