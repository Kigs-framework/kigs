#include "PrecompiledHeaders.h"

#include "GLSLDeferredFilter.h"

using namespace Kigs::Draw;

IMPLEMENT_CLASS_INFO(API3DDeferredFilter)

//# CONSTRUCTOR
IMPLEMENT_CONSTRUCTOR(API3DDeferredFilter)
, mPriority(*this, false, "Priority", 0)
, mDrawPass(*this, false, "DrawPass", 0)
, mTarget(*this, false, "Target")
{
}

bool API3DDeferredFilter::NeedPass(const unsigned int aPass)
{
	return (mDrawPass & aPass);
}