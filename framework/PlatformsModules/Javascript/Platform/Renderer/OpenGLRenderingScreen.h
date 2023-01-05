#pragma once


#include "Platform/Renderer/OpenGLInclude.h"
#include <emscripten.h>
#include <emscripten/html5.h>

namespace Kigs
{
	namespace Draw
	{

		class OpenGLTexture;


		class OpenGLPlatformRenderingScreen : public RenderingScreen
		{
		public:
			DECLARE_ABSTRACT_CLASS_INFO(OpenGLPlatformRenderingScreen,RenderingScreen,Renderer)

			OpenGLPlatformRenderingScreen(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
				
		   
			void	SetWindowByHandle(void *PtrToHandle) override
			{
				
			}
			void*	GetContextHandle() override
			{
				return 0;
			}
			
		protected:
			
			void	InitModifiable() override;

			void	setCurrentContext() override
			{
		#ifndef USE_EGL
				glFinish();
				emscripten_webgl_make_context_current(EM_context);
		#endif
			}
			
			virtual ~OpenGLPlatformRenderingScreen();
			
			virtual void InitializeGL(GLsizei width, GLsizei height)=0;

		#ifdef USE_EGL
			typedef struct _escontext
			{
				/// Window handle
				EGLNativeWindowType  hWnd;

				/// EGL display
				EGLDisplay  eglDisplay;

				/// EGL context
				EGLContext  eglContext;

				/// EGL surface
				EGLSurface  eglSurface;

				EGLConfig	eglConfig;
			} ESContext;

			static ESContext esContext;
			EGLContext		eglContext;
		#else
			EMSCRIPTEN_WEBGL_CONTEXT_HANDLE EM_context;
		#endif
		};    

	}
}