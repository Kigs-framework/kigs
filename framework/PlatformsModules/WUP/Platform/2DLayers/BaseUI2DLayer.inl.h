void	BaseUI2DLayer::Platform_GetMousePosition(kfloat & X, kfloat & Y, kfloat & sX, kfloat & sY, kfloat & dRx, kfloat & dRy)
{
	// use gaze as mouse position
	X = 0; Y = 0;
	sX = 0; sY = 0;
	dRx = 0; dRy = 0;
}

#include "CoreBaseApplication.h"
#include "GazeDevice.h"

int	BaseUI2DLayer::Platform_GetInputFlag()
{ 
	int flag = UIInputEvent::NONE;

	ModuleInput* theInputModule = reinterpret_cast<ModuleInput*>(CoreGetModule(ModuleInput));
	GazeDevice * gaze = theInputModule->GetGaze();
	if (gaze == nullptr)
		return flag;

	auto touches = gaze->GetAllTouches();
	for (auto& t : touches)
	{
		//kigsprintf("%d : %0.3f;%0.3f;%0.3f (%d) %s\n", t.first, t.second.Position.x, t.second.Position.y, t.second.Position.z, t.second.state, t.second.pressed ? "pressed" : "");
		if(t.second.pressed)
			flag |= UIInputEvent::LEFT;
	}

	/*if (touches->size() > 0)
	{
		kigsprintf("%d touches\n", touches->size());
	}*/

	return flag;
}