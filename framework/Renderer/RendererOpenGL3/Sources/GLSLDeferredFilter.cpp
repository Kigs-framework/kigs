#include "PrecompiledHeaders.h"

#include "GLSLDeferredFilter.h"

using namespace Kigs::Draw;

IMPLEMENT_CLASS_INFO(API3DDeferredFilter)

//# CONSTRUCTOR
IMPLEMENT_CONSTRUCTOR(API3DDeferredFilter)
, mPriority(*this, "Priority", 0)
, mDrawPass(*this, "DrawPass", 0)
, mTarget(*this, "Target")
{
}

bool API3DDeferredFilter::NeedPass(const unsigned int aPass)
{
	return (mDrawPass & aPass);
}