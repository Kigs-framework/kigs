#ifndef _DIRECTX11CAMERAORTHO_H
#define _DIRECTX11CAMERAORTHO_H

#include "Camera.h"

class DX11CameraOrtho : public Camera
{
public:
	DECLARE_CLASS_INFO(DX11CameraOrtho, Camera, Renderer)
	DECLARE_INLINE_CONSTRUCTOR(DX11CameraOrtho) {}


	void	InitCullingObject(CullingObject* obj)override;
	void	getRay(const kfloat &ScreenX, const kfloat &ScreenY, Point3D &RayOrigin, Vector3D &RayDirection)override;


protected:
	~DX11CameraOrtho()override;

    bool ProtectedSetActive(TravState* state)override;
   	void ProtectedRelease(TravState* state) override; 

	maFloat m_Size = BASE_ATTRIBUTE(Size,100);
};    

#endif //_DIRECTX11CAMERAORTHO_H