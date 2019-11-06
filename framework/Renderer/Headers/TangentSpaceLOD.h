#pragma once

#include "Drawable.h"

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

	unsigned int LastRenderDisableMask = 0;
	float LastTangentSpaceLOD = -1.0f;

	maBool ForceNoTangents = BASE_ATTRIBUTE(ForceNoTangents, false);
};
