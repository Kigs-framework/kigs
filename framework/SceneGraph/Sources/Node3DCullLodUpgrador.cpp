#include "Node3DCullLodUpgrador.h"
#include "TravState.h"
#include "Camera.h"


using namespace Kigs::Scene;

void	Node3DCullLodUpgrador::Init(CoreModifiable* toUpgrade)
{
	mLodCoef = (maFloat *)toUpgrade->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::FLOAT,"LodCoef",1.0f);
}

//  remove dynamic attributes
void	Node3DCullLodUpgrador::Destroy(CoreModifiable* toDowngrade, bool toDowngradeDeleted)
{
	toDowngrade->RemoveDynamicAttribute("LodCoef");
}


DEFINE_CULL_UPGRADOR_METHOD(Node3DCullLodUpgrador)
{
	if (Node3D::Cull(state, CullingMask))
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
			return false;
		}

		return true;
	}

	return false;
}
