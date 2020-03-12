#include "HTTPConnectAndroid.h"
#include "Core.h"
#include "HTTPAsyncRequestAndroid.h"


IMPLEMENT_CLASS_INFO(HTTPConnectAndroid)

//! constructor
HTTPConnectAndroid::HTTPConnectAndroid(const kstl::string& name,CLASS_NAME_TREE_ARG) : HTTPConnect(name,PASS_CLASS_NAME_TREE_ARG)
{
	
	
}     

//! destructor
HTTPConnectAndroid::~HTTPConnectAndroid()
{

}    


bool	HTTPConnectAndroid::Open()
{
	myIsOpen = true;

	return myIsOpen;
}

void	HTTPConnectAndroid::Close()
{
	myIsOpen = false;
}


void HTTPConnectAndroid::InitModifiable()
{
	HTTPConnect::InitModifiable();

	if (_isInit)
	{
		if (!Open())
		{
			UninitModifiable();
		}
	}
}