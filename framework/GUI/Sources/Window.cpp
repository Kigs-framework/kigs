#include "PrecompiledHeaders.h"
#include "Window.h"

using namespace Kigs::Gui;
#ifdef _MSC_VER
#pragma warning( disable : 4355 )
#endif
IMPLEMENT_CLASS_INFO(Window)

//! constructor, init all parameters
Window::Window(const std::string& name,CLASS_NAME_TREE_ARG) : CoreModifiable(name,PASS_CLASS_NAME_TREE_ARG),
mClickCallback(nullptr), mDoubleClickCallback(nullptr), mKeyDownCallback(nullptr),mKeyUpCallback(nullptr), mDestroyCallback(nullptr)
{
	setInitParameter("Size", true);
	setInitParameter("Position", true);
	setInitParameter("DisplayIndex", true);
	setInitParameter("IsMainWindow", true);
	setInitParameter("FullScreen", true);
	setInitParameter("ShowMouseCursor", true);

	mScreen = nullptr;
	mHandle = nullptr;
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
		if((mSize[0] != 0)&&(mSize[1] != 0))
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
		mScreen = item;
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
