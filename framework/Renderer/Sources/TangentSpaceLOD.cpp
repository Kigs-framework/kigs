#include "TangentSpaceLOD.h"
#include "TravState.h"
#include "Camera.h"
#include "ModuleRenderer.h"

using namespace Kigs::Draw;

IMPLEMENT_CLASS_INFO(TangentSpaceLOD);

bool TangentSpaceLOD::PreDraw(TravState* state)
{
	if (!ParentClassType::PreDraw(state)) return false;
	
	if (state->mCurrentNode)
	{
		mLastRenderDisableMask = state->GetRenderDisableMask();
		mLastTangentSpaceLOD = state->mTangentSpaceLOD;

		
		BBox gbb;
		state->mCurrentNode->GetGlobalBoundingBox(gbb.m_Min, gbb.m_Max);

		auto d = gbb.Distance(state->GetCurrentCamera()->GetPosition());
		auto bbsize = Norm(gbb.m_Max - gbb.m_Min);
		bool disable_tangent_space = false;


		float coef = 1.0f;

		if (mForceNoTangents)
		{
			disable_tangent_space = true;
			coef = 0;
		}
		else if (bbsize != 0.0f)
		{
			coef = (d - bbsize) / bbsize;
			if (coef < 0)
				coef = 1;
			else if (coef > 1)
			{
				disable_tangent_space = true;
				coef = 0;
			}
			else
				coef = 1 - coef;
		}

		if (disable_tangent_space)
		{
			state->SetRenderDisableMask(ModuleRenderer::TANGENT_ARRAY_MASK);
		}
		state->mTangentSpaceLOD = coef;
	}
	return true;
}

bool TangentSpaceLOD::PostDraw(TravState* state)
{
	if (!ParentClassType::PostDraw(state)) return false;

	if (state->mCurrentNode)
	{
		state->SetRenderDisableMask(mLastRenderDisableMask);
		state->mTangentSpaceLOD = mLastTangentSpaceLOD;
	}
	return true;
}
