#ifndef _OPENGLINCLUDES_H
#define _OPENGLINCLUDES_H

#ifdef GL_ES2
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#include <GLES/gl.h>
#include <GLES/glext.h>
#endif

#ifndef GL_COMPRESSED_RGB8_ETC2
#define GL_COMPRESSED_RGB8_ETC2 0x9274
#define GL_COMPRESSED_RGBA8_ETC2_EAC 0x9278
#endif
#endif //_OPENGLINCLUDES_H