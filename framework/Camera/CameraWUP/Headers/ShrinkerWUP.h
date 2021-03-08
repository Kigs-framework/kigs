#ifndef _ShrinkerWUP_H_
#define _ShrinkerWUP_H_

#include "CoreModifiable.h"
#include "GenericShrinker.h"

// ****************************************
// * ShrinkerWUP class
// * --------------------------------------
/**
* \file	ShrinkerWUP.h
* \class	ShrinkerWUP
* \ingroup Camera
* \brief   Specific UWP framebuffer shrinker
*/
// ****************************************


class ShrinkerWUP : public GenericShrinker
{
public:


    DECLARE_CLASS_INFO(ShrinkerWUP,GenericShrinker,CameraModule)

	//! constructor
    ShrinkerWUP(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	virtual void	Process();
	
protected:

	//! destructor
    virtual ~ShrinkerWUP();

	virtual	void AllocateFrameBuffers();
	virtual	void FreeFrameBuffers();

	virtual	void	InitModifiable();

	void	RGBShrink();
	void	YUVShrink();

};

#endif //_ShrinkerWUP_H_
