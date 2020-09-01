#pragma once 

#include "Drawable.h"

// ****************************************
// * RenderingCustomizer class
// * --------------------------------------
/**
* \file	RenderingCustomizer.h
* \class	RenderingCustomizer
* \ingroup Renderer
* \brief Change some rendering states during draw
*
*/
// ****************************************
class RenderingCustomizer : public Drawable
{
public:
	DECLARE_CLASS_INFO(RenderingCustomizer, Drawable, ModuleName);
	DECLARE_INLINE_CONSTRUCTOR(RenderingCustomizer) {}

private:

	unsigned int GetSelfDrawingNeeds() override
	{
		return ((unsigned int)Need_Predraw) | ((unsigned int)Need_Postdraw);
	}

	virtual bool PreDraw(TravState*) override;
	virtual bool PostDraw(TravState*) override;

	maInt mOverrideCullMode = BASE_ATTRIBUTE(OverrideCullMode, -1);
	maInt mOverrideDepthTest = BASE_ATTRIBUTE(OverrideDepthTest, -1);

	int mLastCullMode = -1;
	int mLastDepthTest = -1;

	bool mNeedPop = false;

	void SaveState(TravState* state);
	void RestoreState(TravState* state);
};
