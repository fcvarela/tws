#ifndef _TWS_WINDOW
#define _TWS_WINDOW

#include <string>
#include <iostream>

#include <GL.h>

namespace TWS
{
	class Window
	{
		private:
			int m_Width;
			int m_Height;
			int m_Bpp;
			bool m_Fullscreen;
			std::string m_Title;
			
		public:
			Window();
			~Window();
			
			bool createWindow(int width, int height, int bpp, bool fullscreen, const std::string& title);
			float getHeight();
			float getWidth();
	};
};

#endif

