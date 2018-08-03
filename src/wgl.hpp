#include <iostream>

#ifndef OpenGL_HPP
#define OpenGL_HPP
#include <epoxy/gl.h>
#include <epoxy/wgl.h>

class OpenGL
{
public:
	void GLOpenWindow(std::string name, int width, int height, int x, int y);
	void GLCloseWindow();
	void DrawTexture(std::string Texture);
};

#endif
