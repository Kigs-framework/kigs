#include "RenderingCustomizer.h"


#include "TravState.h"
#include "ModuleRenderer.h"

IMPLEMENT_CLASS_INFO(RenderingCustomizer);

void RenderingCustomizer::SaveState(TravState* state)
{
	if (!mNeedPop)
	{
		state->GetRenderer()->PushState();
		mNeedPop = true;
	}
}
void RenderingCustomizer::RestoreState(TravState* state)
{
	if (mNeedPop)
	{
		state->GetRenderer()->PopState();
		mNeedPop = false;
	}
}

bool RenderingCustomizer::PreDraw(TravState* state)
{
	if (!ParentClassType::PreDraw(state)) return false;

	if (mOverrideCullMode != -1)
	{
		mLastCullMode = state->mOverrideCullMode;
		state->mOverrideCullMode = mOverrideCullMode;
	}

	if (mOverrideDepthTest != -1)
	{
		SaveState(state);
		//state->GetRenderer()->SetDepthTestMode(mOverrideDepthTest);
		state->GetRenderer()->SetDepthTestFunc(mOverrideDepthTest == 0 ? RendererDepthTestFunc::RENDERER_DEPTH_TEST_ALWAYS : RendererDepthTestFunc::RENDERER_DEPTH_TEST_LESS);
	}

	if (mOverrideDepthWrite != -1)
	{
		SaveState(state);
		state->GetRenderer()->SetDepthMaskMode(mOverrideDepthWrite == 0 ? RendererDepthMaskMode::RENDERER_DEPTH_MASK_OFF : RendererDepthMaskMode::RENDERER_DEPTH_MASK_ON);
	}

	return true;
}

bool RenderingCustomizer::PostDraw(TravState* state)
{
	if (!ParentClassType::PostDraw(state)) return false;

	if (mLastCullMode != -1)
	{
		state->mOverrideCullMode = mLastCullMode;
		mLastCullMode = -1;
	}
	RestoreState(state);
	return true;
}
