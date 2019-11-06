#include "PrecompiledHeaders.h"

#include "GLSLDeferredFilter.h"

IMPLEMENT_CLASS_INFO(API3DDeferredFilter)

//# CONSTRUCTOR
IMPLEMENT_CONSTRUCTOR(API3DDeferredFilter)
, myPriority(*this, false, LABEL_AND_ID(Priority), 0)
, myDrawPass(*this, false, LABEL_AND_ID(DrawPass), 0)
, myTarget(*this, false, LABEL_AND_ID(Target))
{
}

bool API3DDeferredFilter::NeedPass(const unsigned int aPass)
{
	return (myDrawPass & aPass);
}