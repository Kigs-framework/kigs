#include "RenderingCustomizer.h"


#include "TravState.h"
#include "ModuleRenderer.h"

IMPLEMENT_CLASS_INFO(RenderingCustomizer);

void RenderingCustomizer::SaveState(TravState* state)
{
	if (!NeedPop)
	{
		state->GetRenderer()->PushState();
		NeedPop = true;
	}
}
void RenderingCustomizer::RestoreState(TravState* state)
{
	if (NeedPop)
	{
		state->GetRenderer()->PopState();
		NeedPop = false;
	}
}

bool RenderingCustomizer::PreDraw(TravState* state)
{
	if (!ParentClassType::PreDraw(state)) return false;

	if (OverrideCullMode != -1)
	{
		LastCullMode = state->OverrideCullMode;
		state->OverrideCullMode = OverrideCullMode;
	}

	if (OverrideDepthTest != -1)
	{
		SaveState(state);
		state->GetRenderer()->SetDepthTestMode(OverrideDepthTest);
	}
	return true;
}

bool RenderingCustomizer::PostDraw(TravState* state)
{
	if (!ParentClassType::PostDraw(state)) return false;

	if (LastCullMode != -1)
	{
		state->OverrideCullMode = LastCullMode;
		LastCullMode = -1;
	}
	RestoreState(state);
	return true;
}
