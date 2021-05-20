#ifndef _OPENGLCAMERAORTHO_H
#define _OPENGLCAMERAORTHO_H

#include "Camera.h"
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
	void	getRay(const kfloat &ScreenX, const kfloat &ScreenY, Point3D &RayOrigin, Vector3D &RayDirection)override;


protected:
	

    bool ProtectedSetActive(TravState* state)override;
   	void ProtectedRelease(TravState* state) override; 

	maFloat mSize = BASE_ATTRIBUTE(Size,100);
};    

#endif