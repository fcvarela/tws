#include <iostream>
#include <Factory.h>
#include "Window.h"

namespace TWS {
	Window::Window(): m_Width(0), m_Height(0), m_Bpp(0), m_Fullscreen(false) {}
	Window::~Window() {SDL_Quit();}

	bool Window::createWindow(int width, int height, int bpp, bool fullscreen, const std::string& title) {
		if (SDL_Init(SDL_INIT_VIDEO) != 0) {
			std::cerr << "TWS::Window SDL_Init failed" << std::endl;
			return false;
		}

		const SDL_VideoInfo *vid = SDL_GetVideoInfo();

		if (fullscreen) {
			m_Width = vid->current_w;
			m_Height = vid->current_h;
		}

        m_Height = height;
		m_Width = width;

		std::cerr << "TWS::Window setting params: " << ((fullscreen == true) ? "Full Screen @ " : "Windowed @ ") << m_Width << "," << m_Height << std::endl;

		m_Title = title;
		m_Fullscreen = fullscreen;
		m_Bpp = bpp;

		// request "at least" this
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);

        // antialias
        uint8_t antialias = atoi(TWS::getConfig("Renderer", "antialias").c_str());
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, antialias);

		// CEGUI
		// SDL_ShowCursor(SDL_DISABLE);

		// Set the title.
		SDL_WM_SetCaption(title.c_str(), title.c_str());

		// Flags tell SDL about the type of window we are creating.
		int flags = SDL_OPENGL;

		if (fullscreen == true)
			flags |= SDL_FULLSCREEN;

		// Create the window
		SDL_Surface *screen = SDL_SetVideoMode(m_Width, m_Height, m_Bpp, flags);
		if (screen == 0) {
			std::cerr << "TWS::Window SDL_SetVideoMode failed: " << SDL_GetError() << std::endl;
			std::cerr << "TWS::Window will try turning off extras (multisampling) and reduce z-buffer (32->24 bit)" << std::endl;
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
			screen = SDL_SetVideoMode(m_Width, m_Height, m_Bpp, flags);
			if (screen == 0) {
				std::cerr << "TWS::Window SDL_SetVideoMode failed again: " << SDL_GetError() << std::endl;
				return false;
			}
		}

		//SDL doesn't trigger off a ResizeEvent at startup, but as we need this for OpenGL, we do this ourself
		SDL_Event resizeEvent;
		resizeEvent.type = SDL_VIDEORESIZE;
		resizeEvent.resize.w = m_Width;
		resizeEvent.resize.h = m_Height;
		SDL_PushEvent(&resizeEvent);

		int rgbaSize[4],stencilSize,depthSize,msBuffers,msSamples,swapControl;
		SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &rgbaSize[0]);
		SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &rgbaSize[1]);
		SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &rgbaSize[2]);
		SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &rgbaSize[3]);
		SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depthSize);
		SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &stencilSize);
		SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &msBuffers);
		SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &msSamples);
		SDL_GL_GetAttribute(SDL_GL_SWAP_CONTROL, &swapControl);

        // low level debug. need to know how many texture units are available
        GLint maxTexelUnits;
        glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &maxTexelUnits);

		std::cerr << "TWS::Window is set up" << std::endl;
		std::cerr << "TWS::Window RGBA sizes are " << rgbaSize[0] << "," << rgbaSize[1] << "," << rgbaSize[2] << "," << rgbaSize[3] << " bits" << std::endl;
		std::cerr << "TWS::Window SDL_GL_DEPTH_SIZE is " << depthSize << " bits" << std::endl;
		std::cerr << "TWS::Window SDL_GL_STENCIL_SIZE is " << stencilSize << " bits" << std::endl;
		std::cerr << "TWS::Window SDL_GL_MULTISAMPLEBUFFERS is " << msBuffers << std::endl;
		std::cerr << "TWS::Window SDL_GL_MULTISAMPLESAMPLES is " << msSamples << std::endl;
		std::cerr << "TWS::Window SDL_GL_SWAP_CONTROL (vsync) is " << ((swapControl == 0) ? "disabled" : "enabled") << std::endl;
		std::cerr << "TWS::Window Available texture units: " << maxTexelUnits << std::endl;

		char tmp[1024];
		strcpy(tmp, (char *)glGetString(GL_RENDERER));
		std::cerr << "TWS::Window GFX Card: " << tmp << std::endl;
		strcpy(tmp, (char *)glGetString(GL_VERSION));
		std::cerr << "TWS::Window Renderer: " << tmp << std::endl;

        // enable unicode
        SDL_EnableUNICODE(1);

		return true;
	}

	float Window::getHeight() {return (float)m_Height;}
	float Window::getWidth()  {return (float)m_Width;}
}

