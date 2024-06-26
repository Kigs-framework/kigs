#pragma once

#include "Camera.h"

namespace Kigs
{
	namespace Draw
	{
		// ****************************************
		// * DX11CameraOrtho class
		// * --------------------------------------
		/**
		 * \file	DX11CameraOrtho.h
		 * \class	DX11CameraOrtho
		 * \ingroup Renderer
		 * \brief	DX11 implementation of orthographic camera.
		 *
		 */
		 // ****************************************

		class DX11CameraOrtho : public Camera
		{
		public:
			DECLARE_CLASS_INFO(DX11CameraOrtho, Camera, Renderer)
				DECLARE_INLINE_CONSTRUCTOR(DX11CameraOrtho) {}
			~DX11CameraOrtho() override;

			void	InitCullingObject(CullingObject* obj) override;
			void	getRay(const float& ScreenX, const float& ScreenY, v3f& RayOrigin, v3f& RayDirection) override;

		protected:
			bool ProtectedSetActive(TravState* state)override;
			void ProtectedRelease(TravState* state) override;

			float mSize = 100;

			WRAP_ATTRIBUTES(mSize);
		};

	}
}