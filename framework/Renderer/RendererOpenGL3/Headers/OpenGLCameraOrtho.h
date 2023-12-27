#pragma once

#include "Camera.h"

namespace Kigs
{
	namespace Draw
	{
		// ****************************************
		// * OpenGLCameraOrtho class
		// * --------------------------------------
		/**
		 * \file	OpenGLCameraOrtho.h
		 * \class	OpenGLCameraOrtho
		 * \ingroup Renderer
		 * \brief	OpenGL implementation of Orthographic Camera.
		 */
		 // ****************************************
		class OpenGLCameraOrtho : public Camera
		{
		public:
			DECLARE_CLASS_INFO(OpenGLCameraOrtho, Camera, Renderer)
				DECLARE_INLINE_CONSTRUCTOR(OpenGLCameraOrtho) {}
			~OpenGLCameraOrtho()override;

			void	InitCullingObject(CullingObject* obj)override;
			void	getRay(const float& ScreenX, const float& ScreenY, Point3D& RayOrigin, v3f& RayDirection)override;


		protected:


			bool ProtectedSetActive(TravState* state)override;
			void ProtectedRelease(TravState* state) override;

			maFloat mSize = BASE_ATTRIBUTE(Size, 100);
		};

	}
}