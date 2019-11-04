#ifndef _OPENGLCAMERAORTHO_H
#define _OPENGLCAMERAORTHO_H

#include "Camera.h"

class OpenGLCameraOrtho : public Camera
{
public:
	DECLARE_CLASS_INFO(OpenGLCameraOrtho, Camera, Renderer)
	DECLARE_INLINE_CONSTRUCTOR(OpenGLCameraOrtho) {}


	void	InitCullingObject(CullingObject* obj)override;
	void	getRay(const kfloat &ScreenX, const kfloat &ScreenY, Point3D &RayOrigin, Vector3D &RayDirection)override;


protected:
	~OpenGLCameraOrtho()override;

    bool ProtectedSetActive(TravState* state)override;
   	void ProtectedRelease(TravState* state) override; 

	maFloat m_Size = BASE_ATTRIBUTE(Size,100);
};    

#endif