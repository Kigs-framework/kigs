#include "HTTPConnectJS.h"
#include "Core.h"
#include "HTTPAsyncRequestJS.h"

IMPLEMENT_CLASS_INFO(HTTPConnectJS)

//! constructor
HTTPConnectJS::HTTPConnectJS(const kstl::string& name, CLASS_NAME_TREE_ARG) : 
HTTPConnect(name, PASS_CLASS_NAME_TREE_ARG)
{
	
	
}     

//! destructor
HTTPConnectJS::~HTTPConnectJS()
{
}    



void HTTPConnectJS::InitModifiable()
{
	HTTPConnect::InitModifiable();

	if (_isInit)
	{
	}
}


bool	 HTTPConnectJS::Open()
{
	// TODO
	return true;
}
void	 HTTPConnectJS::Close()
{
}
