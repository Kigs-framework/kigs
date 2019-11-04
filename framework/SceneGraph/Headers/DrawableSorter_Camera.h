#pragma once


#include "DrawableSorter.h"

class TravState;
class Camera;

class DrawableSorter_Camera : public DrawableSorter
{
public:
	void SetCamera(Camera *pCam);
	void CompleteInformationFor(DrawableSorterItem* pItem,TravState* state);

protected:
	v3f myCameraPosition;
};

