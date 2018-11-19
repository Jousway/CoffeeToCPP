#include <iostream>

#ifndef OpenGL_HPP
#define OpenGL_HPP
#include <epoxy/gl.h>
#include <epoxy/wgl.h>

class OpenGL
{
public:
	void GLOpenWindow(char *name, int width, int height, int x, int y);
	void GLCloseWindow();
    void AppendQuad(std::string Color);
    void AppendTexture(std::string Texture);
    void DrawObject(std::string Texture, std::string Type);
};

#endif
