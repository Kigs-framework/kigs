#include "GazeManager.h"
#include "Camera.h"
#include "ModuleInput.h"
#include "GazeDevice.h"

IMPLEMENT_CLASS_INFO(GazeManager);


void GazeManager::GetGazeRay(Point3D& start, Vector3D& dir, Vector3D& up)
{
	Camera * lGazeCam = (Camera*)(CoreModifiable*)myGazeCamera;
	if (mUseMotionControllerAsGaze)
	{
		auto& touches = ModuleInput::Get()->GetGaze()->GetAllTouches();
		if (touches.size())
		{
			auto t = touches.begin()->second;
			start = t.Position;
			dir = t.Forward;
			up = t.Up;
		}
	}
	else if ((CoreModifiable*)myGazeCamera)
	{
		start = lGazeCam->GetGlobalPosition();
		dir = lGazeCam->GetGlobalViewVector();
		up = lGazeCam->GetGlobalUpVector();
	}
}

void GazeManager::SetShowCursor(bool enabled)
{
	if (myGazeFeedback)
		myGazeFeedback->setValue("Show", enabled);
}
