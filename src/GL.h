/*
 *  GL.h
 *  TWS
 *
 *  Created by Filipe Varela on 08/11/05.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 *
 */

#ifndef _TWS_GL
#define _TWS_GL

#ifdef macosx
#include <GL/glew.h>
#include <SDL/SDL.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#endif

#ifdef linux
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <AL/al.h>
#include <AL/alc.h>
#endif

#include <SDL/SDL.h>

#endif

