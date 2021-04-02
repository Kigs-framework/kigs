#include "PrecompiledHeaders.h"
#include "Window.h"
#ifdef _MSC_VER
#pragma warning( disable : 4355 )
#endif
IMPLEMENT_CLASS_INFO(Window)

//! constructor, init all parameters
Window::Window(const kstl::string& name,CLASS_NAME_TREE_ARG) : CoreModifiable(name,PASS_CLASS_NAME_TREE_ARG),
mFullScreen(*this,true,LABEL_AND_ID(FullScreen)),
mShowMouseCursor(*this, true, LABEL_AND_ID(ShowMouseCursor), false), 
mIsMainWindow(*this, true, LABEL_AND_ID(IsMainWindow), true),
mDisplayIndex(*this, true, LABEL_AND_ID(DisplayIndex), 0xFFFFFFFF),
mDirtySize(*this, false, LABEL_AND_ID(DirtySize), false),
mPositionX(*this,true,LABEL_AND_ID(PositionX),-1), // -1 => centered on screen
mPositionY(*this,true,LABEL_AND_ID(PositionY),-1), // -1 => centered on screen
mSizeX(*this,true,LABEL_AND_ID(SizeX)),
mSizeY(*this,true,LABEL_AND_ID(SizeY)),
mClickCallback(NULL), mDoubleClickCallback(NULL), mKeyDownCallback(NULL),mKeyUpCallback(NULL), mDestroyCallback(NULL)
{
   mScreen = 0;
   mHandle = 0;
}     

Window::~Window()
{

}    

//! try to init window, size and rendering screen must have been set
void	Window::InitModifiable()
{
	CoreModifiable::InitModifiable();
	if(_isInit)
	{
		if((mSizeX != 0)&&(mSizeY != 0))
		{
			ProtectedInit();
		}
		else
		{
			UninitModifiable();
		}
	}
}

//! add item, if item is a rendering screen, set the given item as the used rendering screen (to be done before init)
bool	Window::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	if(item->isSubType("RenderingScreen"))
	{
		mScreen=(RenderingScreen*)item.get();
	}

	return CoreModifiable::addItem(item,pos PASS_LINK_NAME(linkName));

}

//! remove item, if item is a rendering screen, set rendering screen pointer to 0
bool Window::removeItem(const CMSP& item DECLARE_LINK_NAME)
{
	if(item->isSubType("RenderingScreen"))
	{
		mScreen=nullptr;
	}

	return CoreModifiable::removeItem(item PASS_LINK_NAME(linkName));

}
