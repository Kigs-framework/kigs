#ifndef _RENDERERPROFILEDRAWINGOBJECT_H_
#define _RENDERERPROFILEDRAWINGOBJECT_H_

#include "CoreModifiable.h"
#include "TimeProfiler.h"

#include "TecLibs/Tec3D.h"
#include "maString.h"

// ****************************************
// * RendererProfileDrawingObject class
// * --------------------------------------
/**
* \file	RendererProfileDrawingObject.h
* \class	RendererProfileDrawingObject
* \ingroup Renderer
* \brief Draw profilers
*
* ?? Obsolete ??
*
*/
// ****************************************

class RendererProfileDrawingObject : public CoreModifiable
{
public:

	DECLARE_CLASS_INFO(RendererProfileDrawingObject,CoreModifiable,Renderer)
	RendererProfileDrawingObject(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
protected:

	DECLARE_METHOD(DrawProfiles);
	COREMODIFIABLE_METHODS(DrawProfiles);

	Vector3D	mColors[MAX_PROFILER_COUNT];

	bool			mFirstDraw;
	unsigned int 	mProfilerCount;

	maString mFps;

	void			setGlobalFrameCount(int c)
	{
		mMaxGlobalFrameCount[mCurrentGlobalFrameCountIndex] = c;
		++mCurrentGlobalFrameCountIndex;
		if (mCurrentGlobalFrameCountIndex > 15)
		{
			mCurrentGlobalFrameCountIndex = 0;
		}
	}

	int getMaxFrameCount()
	{
		int i;
		int result = mMaxGlobalFrameCount[0];
		for (i = 1; i < 16; i++)
		{
			if (mMaxGlobalFrameCount[i] > result)
			{
				result = mMaxGlobalFrameCount[i];
			}
		}
		return result;
	}

	int				mMaxGlobalFrameCount[16];
	int				mCurrentGlobalFrameCountIndex;
};

#endif //_RENDERERPROFILEDRAWINGOBJECT_H_