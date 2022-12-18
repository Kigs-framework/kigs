#ifndef _DX11CAMERA_H
#define _DX11CAMERA_H

#include "Camera.h"
#include "Timer.h"

// ****************************************
// * DX11Camera class
// * --------------------------------------
/**
 * \file	DX11Camera.h
 * \class	DX11Camera
 * \ingroup Renderer
 * \brief	DX11 implementation of Camera.
 *
 */
 // ****************************************

class DX11Camera : public Camera
{
public:
    DECLARE_CLASS_INFO(DX11Camera,Camera,Renderer)
    DX11Camera(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	virtual ~DX11Camera();

	std::array<mat4, 2> GetStereoViewProjections() override { return mCurrentStereoViewproj; }

protected:
	bool ProtectedSetActive(TravState* state) override;
	virtual void PlatformProtectedSetActive(TravState* state);

	void ProtectedRelease(TravState* state) override;
	virtual void PlatformProtectedRelease(TravState* state);

#if defined(WUP) && defined(GL_ES2)
	bool mNeedNearFarUpdate = true;
	void NotifyUpdate(unsigned int labelid) override;
#endif

	std::array<mat4, 2> mCurrentStereoViewproj;
};    

#endif //_DX11CAMERA_H
