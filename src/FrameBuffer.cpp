/*
 *  FrameBuffer.h
 *
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 *
 */

#include <iostream>
#include <FrameBuffer.h>

namespace TWS {
    void FrameBuffer::cleanup() {
        if (_stencil_buffer) glDeleteRenderbuffersEXT(1, &_stencil_buffer);
        if (_depth_buffer) glDeleteRenderbuffersEXT(1, &_depth_buffer);
        if (_frame_buffer) glDeleteFramebuffersEXT(1, &_frame_buffer);
        if (_depth_texture) glDeleteTextures(1, &_depth_texture);
        if (_texture) glDeleteTextures(1, &_texture);

        _texture = 0;
        _frame_buffer = 0;
        _depth_buffer = 0;
        _stencil_buffer = 0;
    }

    void FrameBuffer::init(GLuint width, GLuint height, bool depthBuffer, bool stencilBuffer) {
        cleanup();

        _width = width;
        _height = height;

        glGenTextures(1, &_texture);
        glBindTexture(GL_TEXTURE_2D, _texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _width, _height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glGenFramebuffersEXT(1, &_frame_buffer);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _frame_buffer);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, _texture, 0);

        if (depthBuffer) {
            glGenRenderbuffersEXT(1, &_depth_buffer);
            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, _depth_buffer);
            glRenderbufferStorage(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, _width, _height);
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, _depth_buffer);
        }

        if (stencilBuffer) {
            glGenRenderbuffersEXT(1, &_stencil_buffer);
            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, _stencil_buffer);
            glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_STENCIL_INDEX8, _width, _height);
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, _stencil_buffer);
        }

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    }

    void FrameBuffer::enable() {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _frame_buffer);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, _texture, 0);
    }

    void FrameBuffer::disable() {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    }

    void FrameBuffer::enableTexture() {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _texture);
    }

    void FrameBuffer::disableTexture() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};
