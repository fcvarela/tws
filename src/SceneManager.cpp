/*
 *  SceneManager.cpp
 *
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 *
 */

#include <iostream>
#include <sys/time.h>
#include <arpa/inet.h>

#include <Factory.h>
#include <SceneManager.h>
#include <WebServer.h>
#include <ObjModel.h>

#include <FrameBuffer.h>

namespace TWS {
    SceneManager::SceneManager() {
        std::cerr << "TWS::SceneManager initialized..." << std::endl;
    }

    SceneManager::~SceneManager() {
        std::cerr << "TWS::SceneManager will dealoc children" << std::endl;
        std::cerr << "TWS::SceneManager dying" << std::endl;
    }

    bool SceneManager::processEvents() {
        SDL_Event event;
        SDLKey sym;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    return false; break;

                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT)
                        m_mouseButtons[0] = 1;
                    if (event.button.button == SDL_BUTTON_RIGHT)
                        m_mouseButtons[1] = 1;
                    break;

                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == SDL_BUTTON_LEFT)
                        m_mouseButtons[0] = 0;
                    if (event.button.button == SDL_BUTTON_RIGHT)
                        m_mouseButtons[1] = 0;
                    break;

                case SDL_MOUSEMOTION:
                    m_mousePosition[0] = event.motion.x;
                    m_mousePosition[1] = event.motion.y;
                    break;

                case SDL_KEYDOWN:
                    sym = event.key.keysym.sym;
                    if (sym == SDLK_ESCAPE)
                        return false;

                    if (sym == SDLK_TAB) {
                        getConsole()->toggle();
                        return true;
                    }

                    if (getConsole()->active())
                        getConsole()->parseKey(event.key.keysym.sym);
                    else
                        m_Keys[sym] = 1;

                    break;

                case SDL_KEYUP:
                    sym = event.key.keysym.sym;
                    m_Keys[sym] = 0;
                    break;
            }
        }
        return true;
    }

    void SceneManager::run() {
        while (processEvents()) {
            processClicks();
            processKeys();
            draw();
            SDL_GL_SwapBuffers();
        }
    }

    void SceneManager::processKeys() {
        if (getConsole()->active() == true)
            return;
        Camera *cam = TWS::getCamera();

        // 10m/s
        double distance_to_closest = (cam->position() - TWS::getClosestPlanet()->_position).length();
        double mov = (distance_to_closest - TWS::getClosestPlanet()->_radius) * TWS::getDt()/2.0;
        float rot = 90.0 * TWS::getDt();

        bool shiftOn = (m_Keys[SDLK_LSHIFT] || m_Keys[SDLK_RSHIFT]);

        if (shiftOn) {
            mov /= 20.0;
            rot /= 20.0;
        }

        bool accelerate = !m_Keys[SDLK_LALT];

        // stop
        if (m_Keys[SDLK_SPACE])
            cam->stop();

        // pitch, yaw
        if (m_Keys[SDLK_RIGHT])
            cam->rotatey(-rot);

        if (m_Keys[SDLK_LEFT])
            cam->rotatey(rot);

        if (m_Keys[SDLK_UP])
            cam->rotatex(-rot);

        if (m_Keys[SDLK_DOWN])
            cam->rotatex(rot);

        if (m_Keys[SDLK_d])
            cam->straferight(mov, accelerate);

        if (m_Keys[SDLK_a])
            cam->straferight(-mov, accelerate);

        if (m_Keys[SDLK_w])
            cam->moveforward(-mov, accelerate);

        if (m_Keys[SDLK_s])
            cam->moveforward(mov, accelerate);

        if (m_Keys[SDLK_q])
            cam->moveupward(mov, accelerate);

        if (m_Keys[SDLK_z])
            cam->moveupward(-mov, accelerate);

        if (m_Keys[SDLK_x])
            cam->rotatez(rot);

        if (m_Keys[SDLK_c])
            cam->rotatez(-rot);
    }

    void SceneManager::processClicks() {
        if (!m_mouseButtons[0])
            return;

        GLint viewport[4];
        GLdouble modelview[16];
        GLdouble projection[16];
        GLfloat winX, winY, winZ;
        GLdouble posX, posY, posZ;

        glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
        glGetDoublev(GL_PROJECTION_MATRIX, projection);
        glGetIntegerv(GL_VIEWPORT, viewport);

        winX = (GLfloat)m_mousePosition[0];
        winY = (GLfloat)viewport[3] - (GLfloat)m_mousePosition[1];

        // bottom left
        glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
        gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
        Vector3d newposition = Vector3d(posX, posY, posZ);
        newposition.normalize();
        newposition *= (TWS::getClosestPlanet()->_radius + 1000000.0);
        TWS::getCamera()->setPosition(newposition);
        std::cerr << "Mouse clicked at (zval = " << winZ << "): " << newposition << std::endl;
        m_mouseButtons[0] = 0;
    }

    bool SceneManager::init() {
        uint16_t width = atoi(TWS::getConfig("Renderer", "width").c_str());
        double videoheightcoeff = strtod(TWS::getConfig("Renderer", "heightscale").c_str(), NULL);
        std::string fullscreen = TWS::getConfig("Renderer", "fullscreen");
        bool fs = (fullscreen == "true") ? true : false;

        if (!m_Window.createWindow(width, (double)width/(double)videoheightcoeff, 24, fs, "TWS")) {
            std::cerr << "TWS::SceneManager init failed - could not create window" << std::endl;
            return false;
        }

        TWS::hsiz = m_Window.getWidth();
        TWS::vsiz = m_Window.getHeight();

        TWS::vfov = 35.0;
        TWS::hfov = m_Window.getWidth() / m_Window.getHeight() * TWS::vfov;

        // Initialize glew
        glewInit();

        // And init our framebuffer (GL_RGBA16F?)
        _fb.init(TWS::hsiz, TWS::vsiz, true, false);
        _shader = new Shader("data/shaders/hdr.glsl");

        // spawn command handler
        TWS::WebServer::spawnRequestHandlerThread();

        // specify shading model
        glShadeModel(GL_SMOOTH);

        // enable blending and specify how to blend
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // specify line antialias
        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

        // clear color
        glClearColor(0.0, 0.0, 0.0, 1.0);

        // line and point width
        glLineWidth(1.0);
        glPointSize(1.0);

        // specify culling method and enable
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);

        // perspective correction
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

        // wireframe for debug (GL_LINE)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glPolygonMode(GL_FRONT, GL_FILL);

        // depth testing
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_DEPTH_TEST);
        glClearDepth(1.0);

        // clean keyboard input
        memset(m_Keys, 0, sizeof(m_Keys));
        memset(m_mouseButtons, 0, sizeof(m_mouseButtons));

        TWS::getCamera();
        TWS::getClosestPlanet();
        TWS::getSun();
        TWS::getConsole();

        return true;
    }

    void SceneManager::draw() {
        if (getConsole()->wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else glPolygonMode(GL_FRONT, GL_FILL);

        // do frame timer stuff (est dt, etc)
        struct timeval now;
        gettimeofday(&now, 0);
        double a = now.tv_sec + (now.tv_usec/1000000.0);
        TWS::setTime(a);

        // step the camera
        TWS::getCamera()->step();

        // draw planets
        Planet *planets[2];
        if (
                (TWS::getCamera()->position() - TWS::getEarth()->_position).length() <
                (TWS::getCamera()->position() - TWS::getMars()->_position).length()
           ) {
            planets[1] = TWS::getMars();
            planets[0] = TWS::getEarth();
        } else {
            planets[0] = TWS::getMars();
            planets[1] = TWS::getEarth();
        }

        if (getConsole()->hdr)
            _fb.enable();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawSunScene();

        for (uint8_t p = 0; p<2; p++)
            drawPlanetScene(planets[p]);

        // reset and draw objects
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glClear(GL_DEPTH_BUFFER_BIT);
        gluPerspective(TWS::vfov, (float)(TWS::hsiz/TWS::vsiz), 1.0, TWS::fardist);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        TWS::getCamera()->setPerspective();
        TWS::getSun()->reposition();

        // testbed for models
        /*
        glDisable(GL_LIGHTING);
        TWS::getISS()->step();
        glPushMatrix();
        glTranslatef(TWS::getISS()->position().x(), TWS::getISS()->position().y(), TWS::getISS()->position().z());
        TWS::getISS()->model()->draw();
        TWS::getISS()->model()->drawMarker();
        glPopMatrix();
        glEnable(GL_LIGHTING);
        glEnable(GL_DEPTH_TEST);
        */
        if (getConsole()->hdr) {
            glPushMatrix();
            _fb.disable();

            // Now draw the framebuffer texture to a quad
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(TWS::vfov, (float)(TWS::hsiz/TWS::vsiz), 1.0, 10000.0);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);

            _fb.enableTexture();
            _shader->bind();

            GLfloat exposure = getConsole()->exposure;
            glUniform1f(glGetUniformLocation(_shader->getProgram(), "fExposure"), exposure);
            glUniform1i(glGetUniformLocation(_shader->getProgram(), "s2Test"), 0);

            glTranslatef(-m_Window.getWidth()/2.0f, -m_Window.getHeight()/2.0, -794.0);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0, 0.0); glVertex2i(0, 0);
            glTexCoord2f(1.0, 0.0); glVertex2i(m_Window.getWidth(), 0.0);
            glTexCoord2f(1.0, 1.0); glVertex2i(m_Window.getWidth(), m_Window.getHeight());
            glTexCoord2f(0.0, 1.0); glVertex2i(0, m_Window.getHeight());
            glEnd();

            _shader->unbind();
            _fb.disableTexture();

            glEnable(GL_DEPTH_TEST);
            glPopMatrix();
        }

        getConsole()->draw();
    }

    void SceneManager::drawPlanetScene(Planet *p) {
        // init some variables
        double cam_height = (TWS::getCamera()->position() - p->_position).length();
        double radius = p->_radius;
        double altitude = cam_height - radius;
        double diameter = 2.0 * radius;
        TWS::fardist = sqrt(altitude * (diameter + altitude));
        double hipot_len = sqrt(pow(cam_height, 2.0) + pow(radius*1.025, 2));
        GLfloat near = cam_height > radius*1.025 ? max(cam_height - radius*3.0, 1000.0) : 30.0;
        GLfloat far = hipot_len;
        far = cam_height * 3.0;

        // reset and draw
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        gluPerspective(TWS::vfov, (float)(TWS::hsiz/TWS::vsiz), near, far);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        TWS::getCamera()->setPerspective();
        TWS::getSun()->reposition();

        p->drawAtmosphere();
        p->drawTerrain();
    }

    void SceneManager::drawSunScene() {
        // init some variables
        double cam_height = (TWS::getCamera()->position() - TWS::getSun()->_position3d).length();
        double radius = TWS::getSun()->_radius;
        double altitude = cam_height - radius;
        double diameter = 2.0 * radius;
        TWS::fardist = sqrt(altitude * (diameter + altitude));
        double hipot_len = sqrt(pow(cam_height, 2.0) + pow(radius*1.025, 2));
        //GLfloat near = cam_height > radius*1.025 ? max(cam_height - radius*3.0, 1000.0) : 30.0;
        GLfloat far = hipot_len;

        // reset and draw
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        gluPerspective(TWS::vfov, (float)(TWS::hsiz/TWS::vsiz), 1.0, far*2.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        TWS::getCamera()->setPerspective();
        TWS::getSun()->reposition();
        TWS::getSun()->draw();

        glDisable(GL_LIGHTING);
        glBegin(GL_LINES);
        glVertex3d(TWS::getMars()->_position.x(), TWS::getMars()->_position.y(), TWS::getMars()->_position.z());
        glVertex3d(TWS::getEarth()->_position.x(), TWS::getEarth()->_position.y(), TWS::getEarth()->_position.z());
        glEnd();
        glEnable(GL_LIGHTING);
    }

    uint8_t *SceneManager::screenshot() {
        unsigned int width = TWS::hsiz;
        unsigned int height = TWS::vsiz;
        GLubyte *pixels=0, *flip=0;

        pixels = (GLubyte *)malloc(sizeof(GLubyte)*width*height*3);
        flip = (GLubyte *)malloc(sizeof(GLubyte)*width*height*3);

        if (pixels!=NULL && flip!=NULL) {
            glPixelStorei(GL_PACK_ALIGNMENT, 1);
            glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

            for (uint y=0; y<height; y++) {
                for (uint x=0; x<width; x++) {
                    flip[(y*width+x)*3] = pixels[((height-1-y)*width+x)*3];
                    flip[(y*width+x)*3+1] = pixels[((height-1-y)*width+x)*3+1];
                    flip[(y*width+x)*3+2] = pixels[((height-1-y)*width+x)*3+2];
                }
            }
        }

        if (pixels != 0) free(pixels);
        return flip;
    }
};
