/*
 *  Shader.cpp
 *  
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <Shader.h>

namespace TWS {
	// needs serious error checking
	Shader::Shader(const char *shader_file) {
		_program = glCreateProgram();
		
		char *shader_source = loadShader(shader_file);
		if (shader_source == NULL)
			return;
		
		_vertex_shader = createShader(shader_source, GL_VERTEX_SHADER);
		_fragment_shader = createShader(shader_source, GL_FRAGMENT_SHADER);
		//_geometry_shader = createShader(shader_source, GL_GEOMETRY_SHADER_EXT);
		free(shader_source);
		link();
	}
	
	Shader::~Shader() {
		glDeleteShader(_vertex_shader);
		glDeleteShader(_fragment_shader);
		//glDeleteShader(_geometry_shader);
		glDeleteProgram(_program);
		
		std::cerr << "TWS::Shader dealloc" << std::endl;
	}
	
	char *Shader::loadShader(const char *program_file) {
		FILE *fd;
		long len, r;
		char *program_source;

		if (!(fd = fopen(program_file, "r"))) {
			std::cerr << "TWS::Shader error opening program file: " << program_file << std::endl;
			return NULL;
		}

		fseek(fd, 0, SEEK_END);
		len = ftell(fd)+1;
		fseek(fd, 0, SEEK_SET);
		
		if (!(program_source = (char *)malloc(len * sizeof(char)))) {
			std::cerr << "TWS::Shader error allocating memory for program file: " << program_file << std::endl;
			return NULL;
		}
		memset(program_source, 0, len);
		r = fread(program_source, sizeof(char), len, fd);
		fclose(fd);
		return program_source;
	}
	
	GLuint Shader::createShader(const char *program_string, GLenum type) {
		if (strlen(program_string) < 1)
			return 0;
		
		char prefix[128] = "";
		switch (type) {
			case GL_VERTEX_SHADER: sprintf(prefix, "#define _VERTEX_"); break;
			case GL_FRAGMENT_SHADER: sprintf(prefix, "#define _FRAGMENT_"); break;
			//case GL_GEOMETRY_SHADER_EXT: sprintf(prefix, "#define _GEOMETRY_"); break;
		}
		
		char *final_source = (char *)malloc(strlen(prefix)+1+strlen(program_string)+1);
		memset(final_source, 0, sizeof(final_source));
		sprintf(final_source, "%s\n%s", prefix, program_string);
		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, (const GLchar**)&final_source, NULL);
		glCompileShader(shader);
		free(final_source);
		
		// check compile status
		GLint compile_log;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_log);
		if (!compile_log) {
			// errored. get log
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &compile_log);
			char buffer[compile_log];
			glGetShaderInfoLog(shader, compile_log, NULL, buffer);
			std::string error_log(buffer);
			std::cerr << "TWS::Shader error loading shader" << std::endl;
			std::cerr << "--------------------------------" << std::endl;
			std::cerr << error_log << std::endl;
			std::cerr << "--------------------------------" << std::endl;
			
			return false;
		}
		
		// all went well
		glAttachShader(_program, shader);
		return shader;
	}
	
	bool Shader::link() {
		glLinkProgram(_program);
		// get status
		GLint link_log;
		glGetProgramiv(_program, GL_LINK_STATUS, &link_log);
		if (!link_log) {
			glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &link_log);
			char buffer[link_log];
			glGetProgramInfoLog(_program, link_log, NULL, buffer);
			std::string error_log(buffer);
			std::cerr << "TWS::Shader error linking shader" << std::endl;
			std::cerr << "--------------------------------" << std::endl;
			std::cerr << error_log << std::endl;
			std::cerr << "--------------------------------" << std::endl;
			
			return false;
		}
		return true;
	}
	
	void Shader::bind() {glUseProgram(_program);}
	void Shader::unbind() {glUseProgram(0);}
	GLuint Shader::getProgram() {return _program;}
	
	bool Shader::uniformf(const char *name, size_t dimension, size_t count, GLfloat *values) {
		switch (dimension) {
			case 1: glUniform1f(glGetUniformLocation(_program, name), *values); break;
			case 2: glUniform2fv(glGetUniformLocation(_program, name), count, values); break;
			case 3: glUniform3fv(glGetUniformLocation(_program, name), count, values); break;
			case 4: glUniform4fv(glGetUniformLocation(_program, name), count, values); break;
			default: return false; break;
		}
		
		return true;
	}
	
	bool Shader::uniformi(const char *name, size_t dimension, size_t count, GLint *values) {
		switch (count) {
			case 1: glUniform1i(glGetUniformLocation(_program, name), *values); break;
			case 2: glUniform2iv(glGetUniformLocation(_program, name), count, values); break;
			case 3: glUniform3iv(glGetUniformLocation(_program, name), count, values); break;
			case 4: glUniform4iv(glGetUniformLocation(_program, name), count, values); break;
			default: return false; break;
		}
		
		return true;
	}
	
	bool Shader::uniform_matrix4f(const char *name, bool transpose, GLfloat *mat) {
		int8_t transpose_fnc = (transpose?1:0);
		glUniformMatrix4fv(glGetUniformLocation(_program, name), 1, transpose_fnc, mat);
		return true;
	}
};
