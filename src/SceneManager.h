/*
 *  SceneManager.h
 *
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 *
 */

#ifndef _TWS_SCENEMANAGER
#define _TWS_SCENEMANAGER

#include <GL.h>
#include <Shader.h>
#include <Window.h>
#include <pthread.h>
#include <Node.h>
#include <FrameBuffer.h>

namespace TWS {
    typedef struct {
        uint16_t vert_count;
        GLdouble *verts;
        char *name;
        char code[3];
    } country_t;

    class SceneManager {
    private:
        FrameBuffer _fb;
        Shader *_shader;
        Window m_Window;

        unsigned int m_Keys[SDLK_LAST];
        unsigned int m_mouseButtons[2];
        unsigned short m_mousePosition[2];
        pthread_t frame_dispatcher_thread;

        void draw();
        void drawPlanetScene(Planet *p);
        void drawSunScene();
        void processKeys();
        bool processEvents();
        void processClicks();

        uint8_t *screenshot();
    public:
        SceneManager();
        ~SceneManager();

        bool init();
        void run();

        country_t *countries;
    };
};

#endif
