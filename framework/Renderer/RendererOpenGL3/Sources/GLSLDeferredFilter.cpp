#include "PrecompiledHeaders.h"

#include "GLSLDeferredFilter.h"

IMPLEMENT_CLASS_INFO(API3DDeferredFilter)

//# CONSTRUCTOR
IMPLEMENT_CONSTRUCTOR(API3DDeferredFilter)
, mPriority(*this, false, LABEL_AND_ID(Priority), 0)
, mDrawPass(*this, false, LABEL_AND_ID(DrawPass), 0)
, mTarget(*this, false, LABEL_AND_ID(Target))
{
}

bool API3DDeferredFilter::NeedPass(const unsigned int aPass)
{
	return (mDrawPass & aPass);
}