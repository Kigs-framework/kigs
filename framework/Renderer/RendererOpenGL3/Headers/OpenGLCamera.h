#ifndef _OPENGLCAMERA_H
#define _OPENGLCAMERA_H

#include "Camera.h"
//#include "GLSLDeferredFilter.h"


class OpenGLCamera : public Camera
{
public:
    DECLARE_CLASS_INFO(OpenGLCamera,Camera,Renderer)

    OpenGLCamera(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	bool	addItem(CoreModifiable *item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;
	bool	removeItem(CoreModifiable* item DECLARE_DEFAULT_LINK_NAME) override;

	//kstl::set<API3DDeferredFilter*, API3DDeferredFilter::PriorityCompare>& GetFilters() { return myFilters; }

protected:

	//void SetAmbient(kfloat r, kfloat g, kfloat b) override;

	bool ProtectedSetActive(TravState* state) override;
	virtual void PlatformProtectedSetActive(TravState* state);

	void ProtectedRelease(TravState* state) override;
	virtual void PlatformProtectedRelease(TravState* state);

	virtual ~OpenGLCamera();

	//kstl::set<API3DDeferredFilter*, API3DDeferredFilter::PriorityCompare> myFilters;

#ifdef WUP
	bool myNeedNearFarUpdate = true;
	void NotifyUpdate(unsigned int labelid) override;
#endif
};    

#endif //_OPENGLCAMERA_H
