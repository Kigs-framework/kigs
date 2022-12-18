#ifndef _DIRECTX11CAMERAORTHO_H
#define _DIRECTX11CAMERAORTHO_H

#include "Camera.h"

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
	void	getRay(const float &ScreenX, const float &ScreenY, Point3D &RayOrigin, Vector3D &RayDirection) override;

protected:
    bool ProtectedSetActive(TravState* state)override;
   	void ProtectedRelease(TravState* state) override; 

	maFloat mSize = BASE_ATTRIBUTE(Size,100);
};    

#endif //_DIRECTX11CAMERAORTHO_H