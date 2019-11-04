#ifndef _HTTPCONNECTWINDOWS_H_
#define _HTTPCONNECTWINDOWS_H_

#include "HTTPConnect.h"
#include <windows.h>
#include <wininet.h>

// ****************************************
// * HTTPConnectWindows class
// * --------------------------------------
/*!  \class HTTPConnectWindows
     manage a win32 html connection
*/
// ****************************************

class HTTPConnectWindows : public HTTPConnect
{
public:

	DECLARE_CLASS_INFO(HTTPConnectWindows, HTTPConnect, HTTPRequestModule)

	//! constructor
    HTTPConnectWindows(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	virtual void InitModifiable();

	HINTERNET getHandle()
	{
		return myConnectionHandle;
	}


	virtual bool	Open();
	virtual void	Close();

protected:

	//! destructor
    virtual ~HTTPConnectWindows();

	HINTERNET					myConnectionHandle;
	HINTERNET					myInternetHandle;
	INTERNET_STATUS_CALLBACK	myiscCallback;
};

#endif //_HTTPCONNECTWINDOWS_H_
