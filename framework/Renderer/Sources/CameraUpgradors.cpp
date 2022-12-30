#include "CameraUpgradors.h"
#include "Core.h"
#include "CoreBaseApplication.h"
#include "ModuleInput.h"
#include "KeyboardDevice.h"

using namespace Kigs::Draw;

// connect to events and create attributes
void	OrbitCameraUp::Init(CoreModifiable* toUpgrade)
{
	// Connect notify update
	KigsCore::Connect(toUpgrade, "NotifyUpdate", toUpgrade, "OrbitCamNotifyUpdate");

	float defaultVal = 0.0f;
	mOrbitPoint = (maVect3DF * )toUpgrade->AddDynamicVectorAttribute("OrbitPoint", &defaultVal,3);
	mOrbitDirection = (maVect3DF*)toUpgrade->AddDynamicVectorAttribute("OrbitDirection", &defaultVal, 3);
	mOrbitUp = (maVect3DF*)toUpgrade->AddDynamicVectorAttribute("OrbitUp", &defaultVal, 3);
	mOrbitDistance = (maFloat * )toUpgrade->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::FLOAT,"OrbitDistance", 250.0f);

	mEnabled = (maBool*)toUpgrade->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::BOOL, "Enabled", true);
	mOrbitNeedCtrl = (maBool*)toUpgrade->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::BOOL, "OrbitNeedCtrl", true); 

	Input::ModuleInput::Get()->registerTouchEvent(toUpgrade, "ManageDirectTouch", "DirectTouch", Input::EmptyFlag);

	Camera* camera = (Camera*)toUpgrade;

	auto pos = camera->GetPosition();
	auto view = camera->GetViewVector();

	(*mOrbitDistance) = Norm(pos);

	*mOrbitDirection = view;
	*mOrbitPoint = pos + view * (*mOrbitDistance);
	*mOrbitUp = camera->GetUpVector();

	mOrbitDirection->changeNotificationLevel(Owner);
	mOrbitPoint->changeNotificationLevel(Owner);
	mOrbitUp->changeNotificationLevel(Owner);
	mOrbitDistance->changeNotificationLevel(Owner);

	KigsCore::Instance()->GetCoreApplication()->AddAutoUpdate(toUpgrade);
}

//  remove dynamic attributes and disconnect events
void	OrbitCameraUp::Destroy(CoreModifiable* toDowngrade, bool toDowngradeDeleted)
{
	if (toDowngradeDeleted) return;
	KigsCore::Disconnect(toDowngrade, "NotifyUpdate", toDowngrade, "OrbitCamNotifyUpdate");

	toDowngrade->RemoveDynamicAttribute("OrbitPoint");
	toDowngrade->RemoveDynamicAttribute("OrbitDirection");
	toDowngrade->RemoveDynamicAttribute("OrbitUp");
	toDowngrade->RemoveDynamicAttribute("OrbitDistance");
	toDowngrade->RemoveDynamicAttribute("Enabled");
	toDowngrade->RemoveDynamicAttribute("OrbitNeedCtrl");

	KigsCore::GetCoreApplication()->RemoveAutoUpdate(toDowngrade);
	
}

DEFINE_UPGRADOR_METHOD(OrbitCameraUp, OrbitCamNotifyUpdate)
{
	if (!params.empty())
	{
		GetUpgrador()->mIsDirty = true;
	}
	return false;
}

DEFINE_UPGRADOR_METHOD(OrbitCameraUp, GetDistanceForInputSort)
{
	//param.inout_sorting_layer = INT_MIN;
	//param.inout_distance = -DBL_MAX;
	return true;
}

DEFINE_UPGRADOR_METHOD(OrbitCameraUp, ManageDirectTouch)
{

	if (params.empty())
	{
		return false;
	}
	void* evp=nullptr;
	params[0]->getValue(evp);

	Input::DirectTouchEvent& ev = *(Input::DirectTouchEvent*)evp;


	auto ctrl = false;
	// TODO with VK keys
#ifdef WIN32
	if (auto kb = Input::ModuleInput::Get()->GetKeyboard())
		ctrl = kb->GetKey(CM_KEY_LCONTROL) | kb->GetKey(CM_KEY_RCONTROL);
#endif
	if (!GetUpgrador()->mOrbitNeedCtrl)
		ctrl = true;

	if (ev.state == Input::GestureRecognizerState::StateBegan)
	{
		if (ev.touch_state == Input::DirectTouchEvent::TouchDown)
		{
			if (ev.button_state == 1 && ctrl)
			{
				GetUpgrador()->mCurrentOperation = Operation::Orbit;
			}
			else if (ev.button_state == 2)
			{
				GetUpgrador()->mCurrentOperation = Operation::Move;
			}
			else return false;

			GetUpgrador()->mOperationStartInputPosition = ev.position;
			GetUpgrador()->mOperationStartOrbitPosition = *GetUpgrador()->mOrbitPoint;
			GetUpgrador()->mOperationStartOrbitView = *GetUpgrador()->mOrbitDirection;
			GetUpgrador()->mOperationStartOrbitUp = *GetUpgrador()->mOrbitUp;
		}
		else if (ev.touch_state == Input::DirectTouchEvent::TouchUp)
		{
			GetUpgrador()->mCurrentOperation = Operation::None;
		}
	}
	else if (ev.state == Input::GestureRecognizerState::StateChanged)
	{
		auto dt = ev.position - GetUpgrador()->mOperationStartInputPosition;
		GetUpgrador()->mHovered = true;
		if (GetUpgrador()->mCurrentOperation == Operation::Orbit)
		{
			v3f dir = GetUpgrador()->mOperationStartOrbitView;
			v3f pos = GetUpgrador()->mOperationStartOrbitPosition;
			v3f up = GetUpgrador()->mOperationStartOrbitUp;

			auto right = v3f(0, 1, 0) ^ dir; right.Normalize();

			v2f size; getRenderingScreen()->GetSize(size.x, size.y);
			size = 1.0f / size;

			auto rotation_axis = v3f(0, 1, 0);
			if (Dot(rotation_axis, up) < 0) right = -right;

			Quaternion q;
			q.SetAngAxis(v3f(0, 1, 0), 2 * dt.x * size.x);

			Quaternion q2;
			q2.SetAngAxis(right, -2 * dt.y * size.y);

			dir = q2 * q * dir;
			*GetUpgrador()->mOrbitDirection = dir;
			*GetUpgrador()->mOrbitUp = q2 * q * up;
			GetUpgrador()->mIsDirty = true;
		}
		else if (GetUpgrador()->mCurrentOperation == Operation::Move)
		{
			v2f size; getRenderingScreen()->GetSize(size.x, size.y);
			size = 1.0f / size;
			auto right = GetUpgrador()->mOperationStartOrbitUp ^ GetUpgrador()->mOperationStartOrbitView;
			auto pos = GetUpgrador()->mOperationStartOrbitPosition + (dt.x * right * size.x + dt.y * GetUpgrador()->mOperationStartOrbitUp * size.y) * *GetUpgrador()->mOrbitDistance;

			*GetUpgrador()->mOrbitPoint = pos;
			GetUpgrador()->mIsDirty = true;
		}

	}

	ev.capture_inputs = GetUpgrador()->mCurrentOperation != Operation::None;
	if (ev.capture_inputs) *ev.swallow_mask = 0xFFFFFFFF;

	PUSH_RETURN_VALUE(true);
	return true;
}

DEFINE_UPGRADOR_UPDATE(OrbitCameraUp)
{
	// always return false, else upgrador is removed

	OrbitCameraUp* localthis = GetUpgrador();

	if (!*GetUpgrador()->mEnabled) return false;

	float delta = 0.0f;
	if (auto mouse = Input::ModuleInput::Get()->GetMouse())
		delta = mouse->getWheelRollDelta() * 5;

	auto time = timer.GetTime();
	auto dt = time - GetUpgrador()->mLastTime;
	GetUpgrador()->mLastTime = time;

	auto shift = false;
	// TODO with VK keys
#ifdef WIN32
	if (auto kb = Input::ModuleInput::Get()->GetKeyboard())
		shift = kb->GetKey(CM_KEY_LSHIFT) | kb->GetKey(CM_KEY_RSHIFT);
#endif
	if (shift) delta *= 4;

	if (delta && GetUpgrador()->mHovered)
	{
		GetUpgrador()->mIsDirty = true;
		*GetUpgrador()->mOrbitDistance = std::min(std::max(1.1f * getValue<float>("NearPlane"), (*GetUpgrador()->mOrbitDistance) - 0.2f * delta * (float)dt), 0.8f * getValue<float>("FarPlane"));
	}
	GetUpgrador()->mHovered = false;

	if (GetUpgrador()->mIsDirty)
	{
		GetUpgrador()->mIsDirty = false;
		SetPosition((*GetUpgrador()->mOrbitPoint) - (*GetUpgrador()->mOrbitDistance) * (v3f)(*GetUpgrador()->mOrbitDirection));
		SetViewVector(*GetUpgrador()->mOrbitDirection);
		SetUpVector(*GetUpgrador()->mOrbitUp);
		*GetUpgrador()->mOrbitUp = GetUpVector();
		EmitSignal("Changed", this);
	}
	return false;
}
