/*
 *  Console.cpp
 *
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 *
 */

#include <Console.h>
#include <Factory.h>
#include <GL.h>

namespace TWS {
    void Console::insertCommand(std::string cmd) {
        _cmdhistory[_cmd_insert_idx] = cmd;
        _cmd_insert_idx = (_cmd_insert_idx+1)%CMD_BUF_HIST;
        _cmd_current_idx = _cmd_insert_idx;
    }

    void Console::parseCommand() {
        insertCommand(_cmdbuff);

        // toggles
        if (_cmdbuff == "t hdr") hdr = !hdr;
        if (_cmdbuff == "t wireframe") wireframe = !wireframe;
        if (_cmdbuff == "t fps") fps = !fps;

        // exposure set
        std::string expos("s exposure ");
        if (_cmdbuff.substr(0, expos.length()) == expos) {
            expos = _cmdbuff.substr(expos.length());
            exposure = strtod(expos.c_str(), NULL);
        }

        // gets
        if (_cmdbuff == "exposure") {
            char res[128];
            sprintf(res, "exposure: %2.2f", exposure);
            insertCommand(std::string(res));
        }

        if (_cmdbuff == "fps") {
            char res[128];
            sprintf(res, "fps: %2.2f", TWS::fps);
            insertCommand(std::string(res));
        }

        _cmdbuff = "";
    }

    void Console::draw() {
        if (!_active) return;

        glPushMatrix();
        // no wireframe here
        glPolygonMode(GL_FRONT, GL_FILL);

        // disable lighting to ensure we can use glColor
        glDisable(GL_LIGHTING);

        // set up an orthogonal 2d projection matrix
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        // assume width > height
        GLdouble size = TWS::hsiz/2.0f;
        GLdouble aspect = (GLdouble)TWS::hsiz/(GLdouble)TWS::vsiz;
        glOrtho(-size*aspect, size*aspect, -size, size, -1.0, 1.0);

        // set 1 scren unit = 1 pixel
        glScaled(aspect, aspect, 1.0);

        // reset to modelview
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        GLfloat botleft[2] = {-TWS::hsiz/2.0f+8.0, -TWS::vsiz/2.0f+8.0};
        GLfloat topleft[2] = {-TWS::hsiz/2.0f+8.0, TWS::vsiz/2.0f-8.0};
        GLfloat topright[2] = {TWS::hsiz/2.0f-8.0, TWS::vsiz/2.0f-8.0};
        GLfloat botright[2] = {TWS::hsiz/2.0f-8.0, -TWS::vsiz/2.0f+8.0};

        // draw background
        glColor4f(0.5, 1.0, 0.5, 0.1);
        glBegin(GL_QUADS);
        glVertex2fv(topright);
        glVertex2fv(topleft);
        glVertex2fv(botleft);
        glVertex2fv(botright);
        glEnd();

        glColor4f(1.0, 1.0, 1.0, 1.0);

        // base
        int8_t i =_cmd_insert_idx-1;
        for (uint8_t j=0; j<CMD_BUF_HIST; j++, i--) {
            glRasterPos2f(-TWS::hsiz/2.0+10.0f, -TWS::vsiz/2.0+12.0 + (j+1)*16.0);
            if (i<0) i = 29 + i;
            _font->Render(_cmdhistory[i%CMD_BUF_HIST].c_str());
        }

        glRasterPos2f(-TWS::hsiz/2.0+10.0f, -TWS::vsiz/2.0+12.0);
        _font->Render(_cmdbuff.c_str());

        // restore previous state
        glEnable(GL_LIGHTING);

        glPopMatrix();
    }

    void Console::parseKey(uint16_t key) {
        if (key == 32 || key == 44 || key == 46 || key == 61 || (key>=48 && key<=57) || (key>=97 && key<=122))
            _cmdbuff.push_back(key);

        if ((key == 127 || key == 8) && _cmdbuff.length())
            _cmdbuff = _cmdbuff.erase(_cmdbuff.length()-1);

        if (key == 13)
            parseCommand();

        if (key == 273)
            scrollBack();

        if (key == 274)
            scrollFront();
    }
    
    void Console::scrollBack() {
        _cmd_current_idx--;
        _cmd_current_idx = (_cmd_current_idx < 0) ? CMD_BUF_HIST + _cmd_current_idx : _cmd_current_idx % CMD_BUF_HIST;
        _cmdbuff = _cmdhistory[_cmd_current_idx];
    }
    
    void Console::scrollFront() {
        _cmd_current_idx++;
        _cmd_current_idx = (_cmd_current_idx < 0) ? CMD_BUF_HIST + _cmd_current_idx : _cmd_current_idx % CMD_BUF_HIST;
        _cmdbuff = _cmdhistory[_cmd_current_idx];
    }
};
