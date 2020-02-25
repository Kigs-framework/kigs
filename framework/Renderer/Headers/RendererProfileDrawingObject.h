#ifndef _RENDERERPROFILEDRAWINGOBJECT_H_
#define _RENDERERPROFILEDRAWINGOBJECT_H_

#include "CoreModifiable.h"
#include "TimeProfiler.h"

#include "TecLibs/Tec3D.h"
#include "maString.h"

class RendererProfileDrawingObject : public CoreModifiable
{
public:

	DECLARE_CLASS_INFO(RendererProfileDrawingObject,CoreModifiable,Renderer)
	RendererProfileDrawingObject(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
protected:

	DECLARE_METHOD(DrawProfiles);
	COREMODIFIABLE_METHODS(DrawProfiles);

	Vector3D	myColors[MAX_PROFILER_COUNT];

	bool			myFirstDraw;
	unsigned int 	myProfilerCount;

	maString myFps;

	void			setGlobalFrameCount(int c)
	{
		myMaxGlobalFrameCount[myCurrentGlobalFrameCountIndex] = c;
		++myCurrentGlobalFrameCountIndex;
		if (myCurrentGlobalFrameCountIndex > 15)
		{
			myCurrentGlobalFrameCountIndex = 0;
		}
	}

	int getMaxFrameCount()
	{
		int i;
		int result = myMaxGlobalFrameCount[0];
		for (i = 1; i < 16; i++)
		{
			if (myMaxGlobalFrameCount[i] > result)
			{
				result = myMaxGlobalFrameCount[i];
			}
		}
		return result;
	}

	int				myMaxGlobalFrameCount[16];
	int				myCurrentGlobalFrameCountIndex;
};

#endif //_RENDERERPROFILEDRAWINGOBJECT_H_