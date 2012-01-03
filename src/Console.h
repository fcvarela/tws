/*
 *  Console.h
 *
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 *
 */

#ifndef _TWS_CONSOLE
#define _TWS_CONSOLE

#include <stdint.h>
#include <list>
#include <string>
#include <sstream>
#include <iostream>
#include <FTGL/ftgl.h>

// max cmd size
#define CMD_MAX_LEN 80

// cmd history size
#define CMD_BUF_HIST 29

namespace TWS {
    class Console {
        private:
            std::string _cmdhistory[CMD_BUF_HIST];
            std::string _cmdbuff;
            FTFont *_font;
            bool _active;
            int8_t _cmd_insert_idx;
            int8_t _cmd_current_idx;

            void insertCommand(std::string cmd);
            void parseBuffer();
            void parseCommand();
            void scrollBack();
            void scrollFront();

        public:
            Console() {
                exposure = 2.0;
                fps = hdr = wireframe = false;
                _font = new FTGLBitmapFont("data/fonts/Arial.ttf");
                _font->FaceSize(12.0);
                _active = false;
                _cmd_insert_idx = 0;
                _cmd_current_idx = 0;
            }

            ~Console() {delete _font;}
            bool active() {return _active;}
            void toggle() {_active = !_active;}

            void draw();
            void parseKey(uint16_t key);
            void parsebuffer();

            // external
            bool fps, hdr, wireframe;
            double exposure;
    };
};

#endif

