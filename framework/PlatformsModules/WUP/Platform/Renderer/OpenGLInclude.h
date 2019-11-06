#ifndef _OPENGLINCLUDES_H
#define _OPENGLINCLUDES_H

//#include <GL/glew.h>
//#include <GL/wglew.h>

// Enable function definitions in the GL headers below
#define GL_GLEXT_PROTOTYPES
// OpenGL ES includes
#include "angle_gl.h"

// EGL includes
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>

// OpenGL ES includes
//#include <GLES2/gl2.h>
//#include <GLES2/gl2ext.h>

#ifdef WIN32
#undef WIN32
#endif

#endif //_OPENGLINCLUDES_H