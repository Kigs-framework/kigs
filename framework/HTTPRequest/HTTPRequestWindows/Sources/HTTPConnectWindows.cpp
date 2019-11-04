#include "PrecompiledHeaders.h"
#include "HTTPConnectWindows.h"
#include "Core.h"
#include "HTTPAsyncRequestWindows.h"
#include "HTTPRequestModuleWindows.h"


IMPLEMENT_CLASS_INFO(HTTPConnectWindows)

//! constructor
HTTPConnectWindows::HTTPConnectWindows(const kstl::string& name,CLASS_NAME_TREE_ARG) : HTTPConnect(name,PASS_CLASS_NAME_TREE_ARG)
, myConnectionHandle(NULL)
, myInternetHandle(NULL)
{
	
	
}     

//! destructor
HTTPConnectWindows::~HTTPConnectWindows()
{
	Close();
}    


bool	HTTPConnectWindows::Open()
{
	if (myIsOpen)
	{
		Close();
	}
	myIsOpen = true;
	unsigned int flags = 0;
	if (!(const bool&)myIsSynchronous)
	{
		flags = INTERNET_FLAG_ASYNC;
	}
	// init internet connection
	//create the root HINTERNET handle using the systems default
	//settings.
	myInternetHandle = ::InternetOpen("Mozilla/4.0 (compatible; MSIE 6.0;Windows NT 5.1)", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, flags);

	//check if the root HINTERNET handle has been created
	if (myInternetHandle == NULL)
	{
#ifdef _DEBUG
		printf("InternetOpen failed with the code error : %i\n", GetLastError());
#endif
		myIsOpen = false;
		goto end;
	}
	if (myInternetHandle != NULL)
	{
		InternetSetStatusCallback(myInternetHandle, &HTTPRequestModuleWindows::WinInetCallback);
		// Connection handle's Context
		
		myConnectionHandle = InternetConnectA(myInternetHandle, (LPCTSTR)myHostName.c_str(), (int)myPort, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
		
		if (myConnectionHandle == NULL)
		{
#ifdef _DEBUG
			printf("InternetConnectA failed with the code error : %i\n", GetLastError());
#endif
			myIsOpen = false;
			goto end;
		}
	}
	

end:
	if (!myIsOpen)
	{
		Close();
	}
	return myIsOpen;
}

void	HTTPConnectWindows::Close()
{
	if (myConnectionHandle)
	{
		InternetCloseHandle(myConnectionHandle);
		myConnectionHandle = NULL;
	}
	if (myInternetHandle != NULL)
	{
		InternetSetStatusCallback(myInternetHandle, 0);
		::InternetCloseHandle(myInternetHandle);
		myInternetHandle = NULL;
	}
	myIsOpen = false;
}


void HTTPConnectWindows::InitModifiable()
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
