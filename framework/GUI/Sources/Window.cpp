#include "PrecompiledHeaders.h"
#include "Window.h"
#ifdef _MSC_VER
#pragma warning( disable : 4355 )
#endif
IMPLEMENT_CLASS_INFO(Window)

//! constructor, init all parameters
Window::Window(const kstl::string& name,CLASS_NAME_TREE_ARG) : CoreModifiable(name,PASS_CLASS_NAME_TREE_ARG),
myFullScreen(*this,true,LABEL_AND_ID(FullScreen)),
m_bShowMouse(*this, true, LABEL_AND_ID(Mouse Cursor), false), 
myIsMainWindow(*this, true, LABEL_AND_ID(IsMainWindow), true),
myDisplayIndex(*this, true, LABEL_AND_ID(DisplayIndex), 0xFFFFFFFF),
myDirtySize(*this, false, LABEL_AND_ID(DirtySize), false),
myPosX(*this,true,LABEL_AND_ID(PositionX),-1), // -1 => centered on screen
myPosY(*this,true,LABEL_AND_ID(PositionY),-1), // -1 => centered on screen
mySizeX(*this,true,LABEL_AND_ID(SizeX)),
mySizeY(*this,true,LABEL_AND_ID(SizeY)),
myClickCallback(NULL), myDoubleClickCallback(NULL), myKeyDownCallback(NULL),myKeyUpCallback(NULL), myDestroyCallback(NULL)
{
   myScreen = 0;
   myHandle = 0;
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
		if((mySizeX != 0)&&(mySizeY != 0))
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
		myScreen=(RenderingScreen*)item.get();
	}

	return CoreModifiable::addItem(item,pos PASS_LINK_NAME(linkName));

}

//! remove item, if item is a rendering screen, set rendering screen pointer to 0
bool Window::removeItem(const CMSP& item DECLARE_LINK_NAME)
{
	if(item->isSubType("RenderingScreen"))
	{
		myScreen=nullptr;
	}

	return CoreModifiable::removeItem(item PASS_LINK_NAME(linkName));

}
