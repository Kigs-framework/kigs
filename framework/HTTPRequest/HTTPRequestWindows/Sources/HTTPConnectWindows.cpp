#include "PrecompiledHeaders.h"
#include "HTTPConnectWindows.h"
#include "Core.h"
#include "HTTPAsyncRequestWindows.h"
#include "HTTPRequestModuleWindows.h"


IMPLEMENT_CLASS_INFO(HTTPConnectWindows)

//! constructor
HTTPConnectWindows::HTTPConnectWindows(const kstl::string& name,CLASS_NAME_TREE_ARG) : HTTPConnect(name,PASS_CLASS_NAME_TREE_ARG)
, mConnectionHandle(NULL)
, mInternetHandle(NULL)
{
	
	
}     

//! destructor
HTTPConnectWindows::~HTTPConnectWindows()
{
	Close();
}    


bool	HTTPConnectWindows::Open()
{
	if (mIsOpen)
	{
		Close();
	}
	mIsOpen = true;
	unsigned int flags = 0;
	if (!(const bool&)mIsSynchronous)
	{
		flags = INTERNET_FLAG_ASYNC;
	}
	// init internet connection
	//create the root HINTERNET handle using the systems default
	//settings.
	mInternetHandle = ::InternetOpen("Mozilla/4.0 (compatible; MSIE 6.0;Windows NT 5.1)", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, flags);

	//check if the root HINTERNET handle has been created
	if (mInternetHandle == NULL)
	{
#ifdef _DEBUG
		printf("InternetOpen failed with the code error : %i\n", GetLastError());
#endif
		mIsOpen = false;
		goto end;
	}
	if (mInternetHandle != NULL)
	{
		InternetSetStatusCallback(mInternetHandle, &HTTPRequestModuleWindows::WinInetCallback);
		// Connection handle's Context
		
		mConnectionHandle = InternetConnectA(mInternetHandle, (LPCTSTR)mHostName.c_str(), (int)mPort, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
		
		if (mConnectionHandle == NULL)
		{
#ifdef _DEBUG
			printf("InternetConnectA failed with the code error : %i\n", GetLastError());
#endif
			mIsOpen = false;
			goto end;
		}
	}
	

end:
	if (!mIsOpen)
	{
		Close();
	}
	return mIsOpen;
}

void	HTTPConnectWindows::Close()
{
	if (mConnectionHandle)
	{
		InternetCloseHandle(mConnectionHandle);
		mConnectionHandle = NULL;
	}
	if (mInternetHandle != NULL)
	{
		InternetSetStatusCallback(mInternetHandle, 0);
		::InternetCloseHandle(mInternetHandle);
		mInternetHandle = NULL;
	}
	mIsOpen = false;
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
