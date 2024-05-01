#include "PrecompiledHeaders.h"

#include "DrawableSorter_Camera.h"
#include "TravState.h"
#include "Camera.h"

using namespace Kigs::Scene;
using namespace Kigs::Draw;

void DrawableSorter_Camera::SetCamera(Camera *pCam)
{
	mCameraPosition = pCam->GetGlobalPosition();
}

void DrawableSorter_Camera::CompleteInformationFor(DrawableSorterItem* pItem,TravState* state)
{
	v3f D;
	auto matrix = state->GetCurrentLocalToGlobalMatrix();
	pItem->mWay.mDrawable->GetGlobalPosition(&matrix, D.x, D.y, D.z);
	D -= mCameraPosition;
	pItem->mFloatVal = length2(D);
}
