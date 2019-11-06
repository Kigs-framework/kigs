#pragma once 

#include "Drawable.h"
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

	maInt OverrideCullMode = BASE_ATTRIBUTE(OverrideCullMode, -1);
	maInt OverrideDepthTest = BASE_ATTRIBUTE(OverrideDepthTest, -1);

	int LastCullMode = -1;
	int LastDepthTest = -1;

	bool NeedPop = false;

	void SaveState(TravState* state);
	void RestoreState(TravState* state);
};
