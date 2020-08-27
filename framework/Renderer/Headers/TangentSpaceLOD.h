#pragma once

#include "Drawable.h"
// ****************************************
// * TangentSpaceLOD class
// * --------------------------------------
/**
* \file	TangentSpaceLOD.h
* \class	TangentSpaceLOD
* \ingroup Renderer
* \brief Decide to draw or not children according to tangent space.
*
*/
// ****************************************
class TangentSpaceLOD : public Drawable
{
public:
	DECLARE_CLASS_INFO(TangentSpaceLOD, Drawable, ModuleName);
	DECLARE_INLINE_CONSTRUCTOR(TangentSpaceLOD) {}


private:

	virtual	unsigned int	GetSelfDrawingNeeds() override
	{
		return ((unsigned int)Need_Predraw) | ((unsigned int)Need_Postdraw);
	}

	virtual bool PreDraw(TravState*) override;
	virtual bool PostDraw(TravState*) override;

	unsigned int mLastRenderDisableMask = 0;
	float mLastTangentSpaceLOD = -1.0f;

	maBool mForceNoTangents = BASE_ATTRIBUTE(ForceNoTangents, false);
};
