#pragma once

#include "Camera.h"

namespace Kigs
{
	namespace Draw
	{
		// ****************************************
		// * OpenGLCamera class
		// * --------------------------------------
		/**
		 * \file	OpenGLCamera.h
		 * \class	OpenGLCamera
		 * \ingroup Renderer
		 * \brief	OpenGL implementation of Camera.
		 */
		 // ****************************************


		class OpenGLCamera : public Camera
		{
		public:
			DECLARE_CLASS_INFO(OpenGLCamera, Camera, Renderer)
				OpenGLCamera(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
			virtual ~OpenGLCamera();

		protected:
			bool ProtectedSetActive(TravState* state) override;
			virtual void PlatformProtectedSetActive(TravState* state);

			void ProtectedRelease(TravState* state) override;
			virtual void PlatformProtectedRelease(TravState* state);

#ifdef WUP
			bool mNeedNearFarUpdate = true;
			void NotifyUpdate(unsigned int labelid) override;
#endif
		};
	}
}