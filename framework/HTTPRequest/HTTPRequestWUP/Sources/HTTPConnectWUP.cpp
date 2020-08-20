#include "PrecompiledHeaders.h"
#include "HTTPConnectWUP.h"
#include "Core.h"
#include "HTTPAsyncRequestWUP.h"
#include "HTTPRequestModuleWUP.h"


IMPLEMENT_CLASS_INFO(HTTPConnectWUP)

//! destructor
HTTPConnectWUP::~HTTPConnectWUP()
{
	Close();
}    




bool HTTPConnectWUP::Open()
{
	if (mIsOpen)
	{
		Close();
	}
	mIsOpen = true;
	//@TODO
	return true;
}
void HTTPConnectWUP::Close()
{
	//@TODO
	mIsOpen = false;

}


void HTTPConnectWUP::InitModifiable()
{
	HTTPConnect::InitModifiable();
	if (IsInit())
	{
		if (!Open())
		{
			UninitModifiable();
		}
	}
}
