#pragma once

#include "Platform/Renderer/OpenGLInclude.h"

#include <EGL/egl.h>

namespace Kigs
{
	namespace Draw
	{
		class OpenGLTexture;

		class OpenGLPlatformRenderingScreen : public RenderingScreen
		{
		public:
			DECLARE_ABSTRACT_CLASS_INFO(OpenGLPlatformRenderingScreen, RenderingScreen, Renderer)

			OpenGLPlatformRenderingScreen(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			void	SetWindowByHandle(void* PtrToHandle) override {	}
			void* GetContextHandle() override { return 0; }


		protected:
			virtual ~OpenGLPlatformRenderingScreen();
			virtual void	InitModifiable() override;
			virtual void InitializeGL(GLsizei width, GLsizei height) = 0;
			void setCurrentContext() override
			{
				eglMakeCurrent(mDisplay, mSurface, mSurface, mContext);
			}

			EGLDisplay mDisplay;
			EGLSurface mSurface;
			EGLContext mContext;
		};
	}
}