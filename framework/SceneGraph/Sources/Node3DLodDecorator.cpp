#include "PrecompiledHeaders.h"
#include "Node3DLodDecorator.h"
#include "TravState.h"
#include "Camera.h"

using namespace Kigs::Scene;
int gCulledByLODDecorator = 0;

DECLARE_DECORABLE_IMPLEMENT(bool, Cull, Node3DLodDecorator, TravState* state, unsigned int cullingMask)
{
	if (Cull.father(state, cullingMask))
	{
		// retreive bbox
		BBox gbb;
		GetGlobalBoundingBox(gbb.m_Min, gbb.m_Max);
		auto d = gbb.Distance(state->GetCurrentCamera()->GetGlobalPosition());
		auto bbsize = Norm(gbb.m_Max - gbb.m_Min);

		if (bbsize == 0.0f)
			return true;

		float coef = d / bbsize;
		float lodcoef = 1.0f;

		getValue("LodCoef", lodcoef);

		if (coef > lodcoef)
		{
			++gCulledByLODDecorator;
			return false;
		}

		return true;
	}

	return false;
}