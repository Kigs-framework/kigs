#ifndef _HTTPCONNECTIOS_H_
#define _HTTPCONNECTIOS_H_

#include "HTTPConnect.h"

// ****************************************
// * HTTPConnectIOS class
// * --------------------------------------
/*!  \class HTTPConnectIOS
     manage a ios html connection
*/
// ****************************************

class HTTPConnectIOS : public HTTPConnect
{
public:

	DECLARE_CLASS_INFO(HTTPConnectIOS, HTTPConnect, HTTPRequestModule)

	//! constructor
    HTTPConnectIOS(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

    virtual bool	Open(){return true;};
    virtual void	Close(){};

protected:

	//! destructor
    virtual         ~HTTPConnectIOS();
};

#endif //_HTTPCONNECTIOS_H_
