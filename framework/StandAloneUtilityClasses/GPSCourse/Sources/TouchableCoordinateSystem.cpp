#include "PrecompiledHeaders.h"

#include "..\Headers\TouchableCoordinateSystem.h"
#include "TecLibs/Tec3D.h"
#include "RendererMatrix.h"
#include "CoreBaseApplication.h"
#include "ModuleInput.h"
IMPLEMENT_CLASS_INFO(TouchableCoordinateSystem)

TouchableCoordinateSystem::TouchableCoordinateSystem(const kstl::string& name,CLASS_NAME_TREE_ARG) : Node3D(name,PASS_CLASS_NAME_TREE_ARG)
, leftClicked(false)
{
}     

void	TouchableCoordinateSystem::InitModifiable()
{
	Node3D::InitModifiable();
	if (IsInit())
	{
		ModuleInput* myInputModule = (ModuleInput*)KigsCore::GetModule("ModuleInput");
		myMouse = myInputModule->GetMouse();
		CoreBaseApplication*	L_currentApp = KigsCore::GetCoreApplication();
		if (L_currentApp)
		{
			L_currentApp->AddAutoUpdate(this);
		}
	}
	else
	{
		UninitModifiable();
	}
}

void TouchableCoordinateSystem::Update(const Timer& timer, void* addParam)
{
	if (leftClicked)
	{
		// TODO
		/*Point2D myMouseDelta;
		myMouse->getMouvement(myMouseDelta.x, myMouseDelta.y);
		Vector3D actualRotation;
		getRotation(actualRotation.x, actualRotation.y, actualRotation.z);
		actualRotation.y -= myMouseDelta.x*0.01;
		setRotation(actualRotation.x, actualRotation.y, actualRotation.z);*/
	}

	if (myMouse->getButtonState(MouseDevice::LEFT) > 0) //Mouse Left button click
	{
		leftClicked = true;
	}
	else
	{
		leftClicked = false;
	}
}

TouchableCoordinateSystem::~TouchableCoordinateSystem()
{
	CoreBaseApplication*	L_currentApp = KigsCore::GetCoreApplication();
	if (L_currentApp)
	{
		L_currentApp->RemoveAutoUpdate(this);
	}
}    

