#if defined(_WIN32)
# include "wgl.hpp"
#else
# include "glx.hpp"
#endif

int main()
{	
	OpenGL OGL;
	//::ShowWindow(::GetConsoleWindow(), SW_HIDE);
    OGL.AppendQuad("Red");
    OGL.AppendTexture("test.png");
	OGL.GLOpenWindow("CoffeeToCPP", 640, 480, 0, 0);

	return 0;
}
