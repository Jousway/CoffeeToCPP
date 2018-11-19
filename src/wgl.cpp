#include <map>

#include "wgl.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

HDC hDC;				/* device context */
HPALETTE hPalette = 0;			/* custom palette (if needed) */
int iwidth, iheight, idepth;
HGLRC hRC;				/* opengl context */
HWND  hWnd;				/* window */

std::map< std::string, std::string > StoredObjects;

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	OpenGL OGL;

    for(auto const &CurrentObject : StoredObjects)
        OGL.DrawObject(CurrentObject.first, CurrentObject.second);

	glFlush();
}

LONG WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static PAINTSTRUCT ps;

	switch (uMsg) {
	case WM_PAINT:
		display();
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		return 0;

	case WM_SIZE:
		glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
		PostMessage(hWnd, WM_PAINT, 0, 0);
		return 0;

	case WM_CHAR:
		switch (wParam) {
		case 27:			/* ESC key */
			OpenGL OGL;
			OGL.GLCloseWindow();
			PostQuitMessage(0);
			break;
		}
		return 0;

	case WM_PALETTECHANGED:
		if (hWnd == (HWND)wParam)
			break;
		/* fall through to WM_QUERYNEWPALETTE */

	case WM_QUERYNEWPALETTE:
		if (hPalette) {
			UnrealizeObject(hPalette);
			SelectPalette(hDC, hPalette, FALSE);
			RealizePalette(hDC);
			return TRUE;
		}
		return FALSE;

	case WM_CLOSE:
		OpenGL OGL;
		OGL.GLCloseWindow();
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HWND CreateOpenGLWindow(char* title, int x, int y, int width, int height, BYTE type, DWORD flags)
{
	int         n, pf;
	HWND        hWnd;
	WNDCLASS    wc;
	LOGPALETTE* lpPal;
	PIXELFORMATDESCRIPTOR pfd;
	static HINSTANCE hInstance = 0;

	/* only register the window class once - use hInstance as a flag. */
	if (!hInstance) {
		hInstance = GetModuleHandle(NULL);
		wc.style = CS_OWNDC;
		wc.lpfnWndProc = (WNDPROC)WindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = "OpenGL";

		if (!RegisterClass(&wc)) {
			MessageBox(NULL, "RegisterClass() failed:  "
				"Cannot register window class.", "Error", MB_OK);
			return NULL;
		}
	}

	hWnd = CreateWindow("OpenGL", title, WS_OVERLAPPEDWINDOW |
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		x, y, width, height, NULL, NULL, hInstance, NULL);

	if (hWnd == NULL) {
		MessageBox(NULL, "CreateWindow() failed:  Cannot create a window.",
			"Error", MB_OK);
		return NULL;
	}

	hDC = GetDC(hWnd);

	/* there is no guarantee that the contents of the stack that become
	the pfd are zeroed, therefore _make sure_ to clear these bits. */
	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | flags;
	pfd.iPixelType = type;
	pfd.cDepthBits = 32;
	pfd.cColorBits = 32;

	pf = ChoosePixelFormat(hDC, &pfd);
	if (pf == 0) {
		MessageBox(NULL, "ChoosePixelFormat() failed:  "
			"Cannot find a suitable pixel format.", "Error", MB_OK);
		return 0;
	}

	if (SetPixelFormat(hDC, pf, &pfd) == FALSE) {
		MessageBox(NULL, "SetPixelFormat() failed:  "
			"Cannot set format specified.", "Error", MB_OK);
		return 0;
	}

	DescribePixelFormat(hDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	if (pfd.dwFlags & PFD_NEED_PALETTE) {
		int redMask, greenMask, blueMask, i;

		n = 1 << pfd.cColorBits;
		if (n > 256) n = 256;

		lpPal = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) +
			sizeof(PALETTEENTRY) * n);
		memset(lpPal, 0, sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * n);
		lpPal->palVersion = 0x300;
		lpPal->palNumEntries = n;

		GetSystemPaletteEntries(hDC, 0, n, &lpPal->palPalEntry[0]);
		redMask = (1 << pfd.cRedBits) - 1;
		greenMask = (1 << pfd.cGreenBits) - 1;
		blueMask = (1 << pfd.cBlueBits) - 1;

		for (i = 0; i < n; ++i) {
			lpPal->palPalEntry[i].peRed =
				(((i >> pfd.cRedShift)   & redMask) * 255) / redMask;
			lpPal->palPalEntry[i].peGreen =
				(((i >> pfd.cGreenShift) & greenMask) * 255) / greenMask;
			lpPal->palPalEntry[i].peBlue =
				(((i >> pfd.cBlueShift)  & blueMask) * 255) / blueMask;
			lpPal->palPalEntry[i].peFlags = 0;
		}

		hPalette = CreatePalette(lpPal);
		if (hPalette) {
			SelectPalette(hDC, hPalette, FALSE);
			RealizePalette(hDC);
		}

		free(lpPal);
	}

	ReleaseDC(hWnd, hDC);

	return hWnd;
}

void OpenGL::GLOpenWindow(char *name, int width, int height, int x, int y)
{
	MSG   msg;

	hWnd = CreateOpenGLWindow(name, x, y, width, height,
		PFD_TYPE_RGBA, 0);
	if (hWnd == NULL)
		exit(1);

	hDC = GetDC(hWnd);
	hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hRC);

	ShowWindow(hWnd, SW_SHOW);

	while (GetMessage(&msg, hWnd, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	wglMakeCurrent(NULL, NULL);
	ReleaseDC(hWnd, hDC);
	wglDeleteContext(hRC);
	DestroyWindow(hWnd);
	if (hPalette)
		DeleteObject(hPalette);
}

void OpenGL::GLCloseWindow()
{
	wglMakeCurrent(NULL, NULL);
	ReleaseDC(hWnd, hDC);
	wglDeleteContext(hRC);
	DestroyWindow(hWnd);
}

void OpenGL::AppendTexture(std::string Texture)
{
    StoredObjects[Texture] = "Image";
}

void OpenGL::AppendQuad(std::string Color)
{
    StoredObjects[Color] = "Quad";
}

std::map<std::string, std::tuple<float,float,float>> Colors =
{
    {"White", {1.0f, 1.0f, 1.0f}},
    {"Black", {0.0f, 0.0f, 0.0f}},
    {"Red",   {1.0f, 0.0f, 0.0f}},
};

void OpenGL::DrawObject(std::string Object, std::string Type)
{
    if(Type == "Image"){
    	glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND); 

    	int width, height, components;
    	unsigned char* image = stbi_load(Object.c_str(), &width, &height, &components, 0);
    	unsigned int texture;
	
    	if (image == 0)
    		std::cout << stbi_failure_reason();

    	glGenTextures(1, &texture); // generate texture object
    	glBindTexture(GL_TEXTURE_2D, texture); // enable our texture object

    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    	glBegin(GL_QUADS);
    		glTexCoord2d(0, 0); glVertex2f(-1, 1);
    		glTexCoord2d(1, 0); glVertex2f(1, 1);
    		glTexCoord2d(1, 1); glVertex2f(1, -1);
    		glTexCoord2d(0, 1); glVertex2f(-1, -1);
    	glEnd();

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);

	    stbi_image_free(image);
    }else if(Type == "Quad"){
        glColor3f(std::get<0>(Colors[Object]), std::get<1>(Colors[Object]), std::get<2>(Colors[Object]));

        glBegin(GL_QUADS);
    		glTexCoord2d(0, 0); glVertex2f(-1, 1);
    		glTexCoord2d(1, 0); glVertex2f(1, 1);
    		glTexCoord2d(1, 1); glVertex2f(1, -1);
    		glTexCoord2d(0, 1); glVertex2f(-1, -1);
    	glEnd();
    }
}
