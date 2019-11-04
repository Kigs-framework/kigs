#include "PrecompiledHeaders.h"

#include "DrawableSorter_Camera.h"
#include "TravState.h"
#include "Camera.h"

void DrawableSorter_Camera::SetCamera(Camera *pCam)
{
	myCameraPosition = pCam->GetGlobalPosition();
}

void DrawableSorter_Camera::CompleteInformationFor(DrawableSorterItem* pItem,TravState* state)
{
	Vector3D D;
	auto matrix = state->GetCurrentLocalToGlobalMatrix();
	pItem->myWay.myDrawable->GetGlobalPosition(&matrix, D.x, D.y, D.z);
	D -= myCameraPosition;
	pItem->m_FloatVal = NormSquare(D);
}
