#include "WindowWin32.h"
#include "RenderingScreen.h"

#include "Platform/Main/BaseApp.h"

#include "winrt/Windows.UI.Core.h"
#include "winrt/Windows.UI.ViewManagement.h"
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::UI::ViewManagement;


IMPLEMENT_CLASS_INFO(WindowWin32)

//! constructor
WindowWin32::WindowWin32(const kstl::string& name, CLASS_NAME_TREE_ARG) : Window(name, PASS_CLASS_NAME_TREE_ARG)
{
	myScreenSaverActive = false;
	myDirtySize = false; 

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
	auto size = winrt::Windows::Foundation::Size(mySizeX, mySizeY);
	view.PreferredLaunchWindowingMode(ApplicationViewWindowingMode::PreferredLaunchViewSize);
	view.PreferredLaunchViewSize(size);
	view.SetPreferredMinSize(size);

	CoreWindow::GetForCurrentThread().Activate();

	mSizeXToSet = mySizeX;
	mSizeYToSet = mySizeY;
	mRetryResize = false;// !view.TryResizeView(size);
	
	auto rect = App::GetApp()->GetWindow().Bounds();
	myPosX = (int)rect.X;
	myPosY = (int)rect.Y;
	//mySizeX = (int)rect.Width;
	//mySizeY = (int)rect.Height;
	
	if (myScreen && !myScreen->IsInit())
	{
		myScreen->setValue("ParentWindowName", getName());
		myScreen->Init();
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
	posx -= (int)myPosX;
	//if(posx >= (int)mySizeX)
	//	posx = ((int)mySizeX)-1;
	//if(posx < 0)
	//	posx = 0;

	posy -= (int)myPosY;
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
	posx -= (int)myPosX;
	//if(posx >= (int)mySizeX)
	//	posx = ((int)mySizeX)-1;
	//if(posx < 0)
	//	posx = 0;

	posy -= (int)myPosY;
	//if(posy>=(int)mySizeY)
	//	posy = ((int)mySizeY)-1;
	//if(posy < 0)
	//	posy = 0;

	if (myScreen)
	{
		myScreen->GetMousePosInDesignScreen(posx, posy, wposx, wposy);
	}
	else
	{
		wposx = (kfloat)posx;
		wposy = (kfloat)posy;
	}
}

//! update window
void  WindowWin32::Update(const Timer&  timer, void* addParam)
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
		mySizeX = (int)mSizeXToSet;
		mySizeY = (int)mSizeYToSet;
		mHasEvent = false;
		//printf("size : %d %d\n", (int)mySizeX, (int)mySizeY);

		if (myScreen)
			myScreen->Resize((kfloat)mySizeX, (kfloat)mySizeY);
	}

	// update position and size
	auto rect = App::GetApp()->GetWindow().Bounds();
	if (myPosX != (int)rect.X ||
		myPosY != (int)rect.Y
		)
	{
		myPosX = (int)rect.X;
		myPosY = (int)rect.Y;
		//printf("pos : %d %d\n", (int)myPosX, (int)myPosY);
	}


	//updating rendering screen
	auto itr = getItems().begin(); 
	auto end = getItems().end();
	for (;itr!=end;++itr)
	{
		if ((*itr).myItem->isSubType("RenderingScreen"))
			(*itr).myItem->CallUpdate(timer, nullptr);
	}
}

void WindowWin32::ShowMouse(bool bOn)
{
}