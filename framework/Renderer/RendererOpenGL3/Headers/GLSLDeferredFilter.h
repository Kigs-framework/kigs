#ifndef _GLSLDeferredFilter_H
#define _GLSLDeferredFilter_H

#include "CoreModifiable.h"
#include "maReference.h"
#include "CoreModifiableAttribute.h"

class API3DDeferred;
class TravState;

class API3DDeferredFilter : public CoreModifiable
{
public:
	friend class API3DDeferred;

	const static unsigned int PreDrawPass = 1;
	const static unsigned int PreGeoPass = 2;
	const static unsigned int PostGeoPass = 4;
	const static unsigned int PreLightPass = 8;
	const static unsigned int PostLightPass = 16;
	const static unsigned int PostDrawPass = 32;

	struct PriorityCompare
	{
		//! overload operator () for comparison
		bool operator()(const API3DDeferredFilter * a1, const API3DDeferredFilter * a2) const
		{
			if (a1->myPriority == a2->myPriority)
				return (a1)<(a2);
			return a1->myPriority<a2->myPriority;
		}
	};

	DECLARE_CLASS_INFO(API3DDeferredFilter, CoreModifiable, Renderer)
	DECLARE_CONSTRUCTOR(API3DDeferredFilter)


	bool NeedPass(const unsigned int);

protected:
	virtual unsigned int PreDrawTreatment(TravState*, API3DDeferred*) { return -1; }
	virtual unsigned int PreGeometryTreatment(TravState*, API3DDeferred*) { return -1; }
	virtual unsigned int PostGeometryTreatment(TravState*, API3DDeferred*) { return -1; }
	virtual unsigned int PreLightTreatment(TravState*, API3DDeferred*) { return -1; }
	virtual unsigned int PostLightTreatment(TravState*, API3DDeferred*) { return -1; }
	virtual unsigned int PostDrawTreatment(TravState*, API3DDeferred*) { return -1; }

	maUInt myPriority;
	maUInt myDrawPass;

	maReference myTarget; 
};

#endif //_GLSLDeferred2_H
