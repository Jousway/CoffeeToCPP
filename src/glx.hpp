#ifndef OpenGL_HPP
#define OpenGL_HPP
#include <epoxy/gl.h>
#include <epoxy/glx.h>

class OpenGL
{
public:
	void GLOpenWindow(char* name, int width, int height, int x, int y);
	void GLCloseWindow();
	void DrawTexture(char* Texture);
};

#endif
