#if defined(_WIN32)
# include "wgl.hpp"
#endif

int main()
{	
	OpenGL OGL;
	//::ShowWindow(::GetConsoleWindow(), SW_HIDE);
	OGL.GLOpenWindow("CoffeeToCPP", 640, 480, 0, 0);
	OGL.DrawTexture("test.png");

	return 0;
}