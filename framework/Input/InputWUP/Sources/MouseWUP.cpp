#include "MouseWUP.h"
#include "CoreBaseApplication.h"
#include "Core.h"
#include "DeviceItem.h"
#include "Window.h"

#include "Platform/Main/BaseApp.h"


#include "winrt/Windows.UI.Input.h"
#include "winrt/Windows.Devices.Input.h"

IMPLEMENT_CLASS_INFO(MouseWUP);

MouseWUP::MouseWUP(const std::string& name, CLASS_NAME_TREE_ARG) : MouseDevice(name, PASS_CLASS_NAME_TREE_ARG)
{
	auto app = App::GetApp();

	app->GetWindow().PointerWheelChanged([this](winrt::Windows::UI::Core::CoreWindow const& window, winrt::Windows::UI::Core::PointerEventArgs args)
	{
		WheelDelta = args.CurrentPoint().Properties().MouseWheelDelta();
	});

	app->GetWindow().PointerPressed([this](winrt::Windows::UI::Core::CoreWindow const& window, winrt::Windows::UI::Core::PointerEventArgs args)
	{
		LeftPressed = args.CurrentPoint().Properties().IsLeftButtonPressed();
		RightPressed = args.CurrentPoint().Properties().IsRightButtonPressed();
		MiddlePressed = args.CurrentPoint().Properties().IsMiddleButtonPressed();
	});

	app->GetWindow().PointerReleased([this](winrt::Windows::UI::Core::CoreWindow const& window, winrt::Windows::UI::Core::PointerEventArgs args)
	{
		LeftPressed = args.CurrentPoint().Properties().IsLeftButtonPressed();
		RightPressed = args.CurrentPoint().Properties().IsRightButtonPressed();
		MiddlePressed = args.CurrentPoint().Properties().IsMiddleButtonPressed();
	});
}

MouseWUP::~MouseWUP()
{
}

bool	MouseWUP::Aquire()
{
	if (MouseDevice::Aquire())
	{
		return true;
	}
	return false;
}

bool	MouseWUP::Release()
{
	if (MouseDevice::Release())
	{
		return true;
	}
	return false;
}

void	MouseWUP::UpdateDevice()
{
	int currentDevice = 0;
	
	// posX and posY
	float x = 0.0f, y = 0.0f;

	auto pointer_position = App::GetApp()->GetWindow().PointerPosition();

	x = pointer_position.X;
	y = pointer_position.Y;

	//auto bounds = App::GetApp()->GetWindow().Bounds();
	
	mDeviceItems[currentDevice++]->getState()->SetValue(x);
	mDeviceItems[currentDevice++]->getState()->SetValue(y);
	
	mDX = x - mPosX;
	mDY = y - mPosY;
	mPosX = x;
	mPosY = y;
	
	//printf("%0.3f %0.3f (%0.3f %0.3f)\n", (float)myPosX, (float)myPosY, myDX, myDY);
	
	// wheel
	if (mWheelCount)
	{
		mDeviceItems[currentDevice++]->getState()->SetValue(WheelDelta);
		if (mDZ != mPreviousDZ)
			mPreviousDZ = mDZ;
		mDZ += WheelDelta;
		WheelDelta = 0;
	}
	
	// left right middle
	if (mButtonsCount >= 2)
	{
		mDeviceItems[currentDevice++]->getState()->SetValue(LeftPressed);
		mDeviceItems[currentDevice++]->getState()->SetValue(RightPressed);
		if(mButtonsCount > 2)
			mDeviceItems[currentDevice++]->getState()->SetValue(MiddlePressed);
	}
}

void	MouseWUP::DoInputDeviceDescription()
{
	//Input::MouseCapabilities ^mc = ref new Input::MouseCapabilities();

	winrt::Windows::Devices::Input::MouseCapabilities mc;

	mButtonsCount = mc.NumberOfButtons();
	mWheelCount = mc.VerticalWheelPresent() ? 1 : 0;// left right middle
	
	mDeviceItemsCount = mButtonsCount + mWheelCount + 2; // +2 for posx and posy
	
	DeviceItem**	devicearray = new DeviceItem*[mDeviceItemsCount];
	
	unsigned int currentDevice = 0;
	
	devicearray[currentDevice++] = new DeviceItem(DeviceItemState<kfloat>(0.0f));
	devicearray[currentDevice++] = new DeviceItem(DeviceItemState<kfloat>(0.0f));
	
	if (mWheelCount)
		devicearray[currentDevice++] = new DeviceItem(DeviceItemState<kfloat>(0.0f));
	
	int currentButton;
	for (currentButton = 0; currentButton < mButtonsCount; currentButton++)
	{
		devicearray[currentDevice++] = new DeviceItem(DeviceItemState<int>(0));
	}
	
	InitItems(mDeviceItemsCount, devicearray);
	
	for (currentDevice = 0; currentDevice < mDeviceItemsCount; currentDevice++)
		delete devicearray[currentDevice];
	
	delete[] devicearray;
}