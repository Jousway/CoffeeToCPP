#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "glx.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	OpenGL OGL;

	OGL.DrawTexture("test.png");

	glFlush();
}

int singleBufferAttributess[] = {
    GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
    GLX_RENDER_TYPE,   GLX_RGBA_BIT,
    GLX_RED_SIZE,      1,   /* Request a single buffered color buffer */
    GLX_GREEN_SIZE,    1,   /* with the maximum number of color bits  */
    GLX_BLUE_SIZE,     1,   /* for each component                     */
    None
};

int doubleBufferAttributes[] = {
    GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
    GLX_RENDER_TYPE,   GLX_RGBA_BIT,
    GLX_DOUBLEBUFFER,  True,  /* Request a double-buffered color buffer with */
    GLX_RED_SIZE,      1,     /* the maximum number of bits per component    */
    GLX_GREEN_SIZE,    1, 
    GLX_BLUE_SIZE,     1,
    None
};


static Bool WaitForNotify( Display *dpy, XEvent *event, XPointer arg ) {
    return (event->type == MapNotify) && (event->xmap.window == (Window) arg);
}


void OpenGL::GLOpenWindow(char* name, int width, int height, int x, int y)
{
    Display              *dpy;
    Window                xWin;
    XEvent                event;
    XVisualInfo          *vInfo;
    XSetWindowAttributes  swa;
    GLXFBConfig          *fbConfigs;
    GLXContext            context;
    GLXWindow             glxWin;
    int                   swaMask;
    int                   numReturned;
    int                   swapFlag = True;

    /* Open a connection to the X server */
    dpy = XOpenDisplay( NULL );
    if ( dpy == NULL ) {
        printf( "Unable to open a connection to the X server\n" );
        exit( EXIT_FAILURE );
    }

    /* Request a suitable framebuffer configuration - try for a double 
    ** buffered configuration first */
    fbConfigs = glXChooseFBConfig( dpy, DefaultScreen(dpy),
                                   doubleBufferAttributes, &numReturned );

    if ( fbConfigs == NULL ) {  /* no double buffered configs available */
      fbConfigs = glXChooseFBConfig( dpy, DefaultScreen(dpy),
                                     singleBufferAttributess, &numReturned );
      swapFlag = False;
    }

    /* Create an X colormap and window with a visual matching the first
    ** returned framebuffer config */
    vInfo = glXGetVisualFromFBConfig( dpy, fbConfigs[0] );

    swa.border_pixel = 0;
    swa.event_mask = StructureNotifyMask;
    swa.colormap = XCreateColormap( dpy, RootWindow(dpy, vInfo->screen),
                                    vInfo->visual, AllocNone );

    swaMask = CWBorderPixel | CWColormap | CWEventMask;

    xWin = XCreateWindow( dpy, RootWindow(dpy, vInfo->screen), x, y, width, height,
                          0, vInfo->depth, InputOutput, vInfo->visual,
                          swaMask, &swa );

    /* Create a GLX context for OpenGL rendering */
    context = glXCreateNewContext( dpy, fbConfigs[0], GLX_RGBA_TYPE,
				 NULL, True );

    /* Create a GLX window to associate the frame buffer configuration
    ** with the created X window */
    glxWin = glXCreateWindow( dpy, fbConfigs[0], xWin, NULL );
    
    /* Map the window to the screen, and wait for it to appear */
    XMapWindow( dpy, xWin );
    XIfEvent( dpy, &event, WaitForNotify, (XPointer) xWin );

    /* Bind the GLX context to the Window */
    glXMakeContextCurrent( dpy, glxWin, glxWin, context );

    /* OpenGL rendering ... */
    while(true){
        display();
        if ( swapFlag )
            glXSwapBuffers( dpy, glxWin );
    }
}

void OpenGL::DrawTexture(char* Texture)
{
	glEnable(GL_TEXTURE_2D);

	int width, height, components;
	unsigned char* image = stbi_load(Texture, &width, &height, &components, 0);
	unsigned int texture;
	
	if (image == 0)
		std::cout << stbi_failure_reason();

	glGenTextures(1, &texture); // generate texture object
	glBindTexture(GL_TEXTURE_2D, texture); // enable our texture object

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glBegin(GL_QUADS);
		glTexCoord2d(0, 0); glVertex2f(-1, 1);
		glTexCoord2d(1, 0); glVertex2f(1, 1);
		glTexCoord2d(1, 1); glVertex2f(1, -1);
		glTexCoord2d(0, 1); glVertex2f(-1, -1);
	glEnd();

	stbi_image_free(image);
}
