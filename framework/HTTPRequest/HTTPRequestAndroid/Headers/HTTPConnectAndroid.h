#ifndef _HTTPCONNECTANDROID_H_
#define _HTTPCONNECTANDROID_H_

#include "HTTPConnect.h"


// ****************************************
// * HTTPConnectAndroid class
// * --------------------------------------
/*!  \class HTTPConnectAndroid
     manage an android html connection
*/
// ****************************************

class HTTPConnectAndroid : public HTTPConnect
{
public:

	DECLARE_CLASS_INFO(HTTPConnectAndroid, HTTPConnect, HTTPRequestModule)

	//! constructor
    HTTPConnectAndroid(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	void InitModifiable() override;

	bool	Open() override;
	void	Close() override;
		
protected:

	//! destructor
    virtual ~HTTPConnectAndroid();
	
	// HINTERNET					myConnectionHandle;
	// HINTERNET					myInternetHandle;
	// INTERNET_STATUS_CALLBACK	myiscCallback;
};

#endif //_HTTPCONNECTANDROID_H_
