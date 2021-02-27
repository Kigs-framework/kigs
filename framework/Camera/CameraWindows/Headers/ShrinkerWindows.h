#ifndef _SHRINKERWINDOWS_H_
#define _SHRINKERWINDOWS_H_

#include "CoreModifiable.h"
#include "GenericShrinker.h"

// ****************************************
// * ShrinkerWindows class
// * --------------------------------------
/*!  \class ShrinkerWindows
     reduce camera frame size
*/
// ****************************************


class ShrinkerWindows : public GenericShrinker
{
public:


    DECLARE_CLASS_INFO(ShrinkerWindows,GenericShrinker,CameraModule)

	//! constructor
    ShrinkerWindows(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	virtual void	Process();
	
protected:

	//! destructor
    virtual ~ShrinkerWindows();

	virtual	void AllocateFrameBuffers();
	virtual	void FreeFrameBuffers();

	virtual	void	InitModifiable();

	void	RGBShrink();
	void	YUVShrink();

};

#endif //_SHRINKERWINDOWS_H_
