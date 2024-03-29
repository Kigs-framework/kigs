#include "WindowWin32.h"
#include "RenderingScreen.h"

#include "Platform/Main/BaseApp.h"

#include "winrt/Windows.UI.Core.h"
#include "winrt/Windows.Foundation.h"
#include "winrt/Windows.UI.ViewManagement.h"
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::UI::ViewManagement;

using namespace Kigs::Gui;

IMPLEMENT_CLASS_INFO(WindowWin32)

//! constructor
WindowWin32::WindowWin32(const std::string& name, CLASS_NAME_TREE_ARG) : Window(name, PASS_CLASS_NAME_TREE_ARG)
{
	mScreenSaverActive = false;
	mDirtySize = false; 

	if (App::GetApp()->GetStationaryReferenceFrame()) // hololens
	{
	}
	else
	{
		App::GetApp()->GetWindow().SizeChanged([this](winrt::Windows::UI::Core::CoreWindow const& window, winrt::Windows::UI::Core::WindowSizeChangedEventArgs args)
		{
			mSizeXToSet = args.Size().Width;
			mSizeYToSet = args.Size().Height;
			mHasEvent = true;
		});
	}
}

//! destructor
WindowWin32::~WindowWin32()
{
}

//! init the window, and if OK, show it
void WindowWin32::ProtectedInit()
{
	auto view = ApplicationView::GetForCurrentView();
	auto size = winrt::Windows::Foundation::Size(mSize[0], mSize[1]);
	view.PreferredLaunchWindowingMode(ApplicationViewWindowingMode::PreferredLaunchViewSize);
	view.PreferredLaunchViewSize(size);
	view.SetPreferredMinSize(size);

	CoreWindow::GetForCurrentThread().Activate();

	mSizeXToSet = mSize[0];
	mSizeYToSet = mSize[1];
	mRetryResize = false;// !view.TryResizeView(size);
	
	auto rect = App::GetApp()->GetWindow().Bounds();
	mPosition[0] = (int)rect.X;
	mPosition[1] = (int)rect.Y;
	//mySizeX = (int)rect.Width;
	//mySizeY = (int)rect.Height;
	
	if (mScreen && !mScreen->IsInit())
	{
		mScreen->setValue("ParentWindowName", getName());
		mScreen->Init();
	}
}

void	WindowWin32::GetMousePosInWindow(int posx, int posy, kfloat& wposx, kfloat& wposy)
{
	/*if(myScreen)
	{
	GetMousePosInDesignWindow(posx,posy,wposx,wposy);
	}
	else
	{*/
	posx -= (int)mPosition[0];
	//if(posx >= (int)mySizeX)
	//	posx = ((int)mySizeX)-1;
	//if(posx < 0)
	//	posx = 0;

	posy -= (int)mPosition[1];
	//if(posy>=(int)mySizeY)
	//	posy = ((int)mySizeY)-1;
	//if(posy < 0)
	//	posy = 0;

	wposx = (kfloat)posx;
	wposy = (kfloat)posy;
	//}
}

void	WindowWin32::GetMousePosInDesignWindow(int posx, int posy, kfloat& wposx, kfloat& wposy)
{
	posx -= (int)mPosition[0];
	//if(posx >= (int)mySizeX)
	//	posx = ((int)mySizeX)-1;
	//if(posx < 0)
	//	posx = 0;

	posy -= (int)mPosition[1];
	//if(posy>=(int)mySizeY)
	//	posy = ((int)mySizeY)-1;
	//if(posy < 0)
	//	posy = 0;

	if (mScreen)
	{
		mScreen->as<Draw::RenderingScreen>()->GetMousePosInDesignScreen(posx, posy, wposx, wposy);
	}
	else
	{
		wposx = (kfloat)posx;
		wposy = (kfloat)posy;
	}
}

//! update window
void  WindowWin32::Update(const Time::Timer&  timer, void* addParam)
{
	if (mRetryResize)
	{
		auto view = ApplicationView::GetForCurrentView();
		auto size = winrt::Windows::Foundation::Size(mSizeXToSet, mSizeYToSet);
		view.PreferredLaunchWindowingMode(ApplicationViewWindowingMode::PreferredLaunchViewSize);
		view.SetPreferredMinSize(size);
		view.PreferredLaunchViewSize(size);
		CoreWindow::GetForCurrentThread().Activate();
		mRetryResize = false; // !view.TryResizeView(size);
	}

	if (mHasEvent)
	{
		mRetryResize = false;
		mSize = v2f((int)mSizeXToSet, (int)mSizeYToSet);
		mHasEvent = false;
		//printf("size : %d %d\n", (int)mySizeX, (int)mySizeY);

		if (mScreen)
			mScreen->as<Draw::RenderingScreen>()->Resize(mSize[0], mSize[1]);
	}

	// update position and size
	auto rect = App::GetApp()->GetWindow().Bounds();
	if (mPosition[0] != (int)rect.X ||
		mPosition[1] != (int)rect.Y
		)
	{
		mPosition[0] = (int)rect.X;
		mPosition[1] = (int)rect.Y;
		//printf("pos : %d %d\n", (int)myPosX, (int)myPosY);
	}


	//updating rendering screen
	auto itr = getItems().begin(); 
	auto end = getItems().end();
	for (;itr!=end;++itr)
	{
		if ((*itr).mItem->isSubType("RenderingScreen"))
			(*itr).mItem->CallUpdate(timer, nullptr);
	}
}

void WindowWin32::ShowMouse(bool bOn)
{
}