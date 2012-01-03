/*
 *  FrameBuffer.h
 *
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 *
 */

#ifndef _TWS_FRAMEBUFFER
#define _TWS_FRAMEBUFFER

#include <GL.h>

namespace TWS {
    class FrameBuffer {
        protected:
            GLuint _texture;
            GLuint _depth_texture;
            GLuint _frame_buffer;
            GLuint _depth_buffer;
            GLuint _stencil_buffer;

            GLuint _width, _height;

        public:
            FrameBuffer() {
                _texture = 0;
                _frame_buffer = 0;
                _depth_buffer = 0;
                _stencil_buffer = 0;
            }

            ~FrameBuffer() {}
            void cleanup();
            void init(GLuint width, GLuint height, bool depthBuffer, bool stencilBuffer);
            void enable();
            void disable();
            void enableTexture();
            void disableTexture();
    };
};

#endif

