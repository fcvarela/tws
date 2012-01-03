/*
 *  Shader.h
 *  
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 *
 */

#ifndef _TWS_SHADER
#define _TWS_SHADER

#include <GL.h>

namespace TWS {
	class Shader {
	private:
		GLint _program, _vertex_shader, _fragment_shader/*, _geometry_shader*/;
		
		char *loadShader(const char *program_file);
		GLuint createShader(const char *program_string, GLenum type);
		bool link();
		
	public:
		Shader(const char *shader_file);
		~Shader();
		
		void bind();
		void unbind(); // static behaviour
		GLuint getProgram();
		bool uniformf(const char *name, size_t dimension, size_t count, GLfloat *values);
		bool uniformi(const char *name, size_t dimension, size_t count, GLint *values);
		bool uniform_matrix4f(const char *name, bool transpose, GLfloat *mat);
	};
};

#endif
