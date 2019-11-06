#ifndef _AOBJECTRESOURCE_H_
#define _AOBJECTRESOURCE_H_

#include "CoreModifiable.h"

class AObjectResource : public CoreModifiable
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(AObjectResource, CoreModifiable, Animation)
	DECLARE_INLINE_CONSTRUCTOR(AObjectResource) {}
};

#endif //_AOBJECTRESOURCE_H_
