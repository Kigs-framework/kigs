#include "HTTPConnectIOS.h"
#include "Core.h"
#include "HTTPAsyncRequestIOS.h"
#include "HTTPRequestModuleIOS.h"


IMPLEMENT_CLASS_INFO(HTTPConnectIOS)

//! constructor
HTTPConnectIOS::HTTPConnectIOS(const kstl::string& name,CLASS_NAME_TREE_ARG) : HTTPConnect(name,PASS_CLASS_NAME_TREE_ARG)
{
	
	
}     

//! destructor
HTTPConnectIOS::~HTTPConnectIOS()
{
	Close();
}    
