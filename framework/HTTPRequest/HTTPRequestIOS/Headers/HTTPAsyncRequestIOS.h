#ifndef _HTTPASYNCREQUESTIOS_H_
#define _HTTPASYNCREQUESTIOS_H_

#include "HTTPAsyncRequest.h"

// ****************************************
// * HTTPAsyncRequestIOS class
// * --------------------------------------
/*!  \class HTTPAsyncRequestIOS
     manage a ios html request
*/
// ****************************************

class HTTPAsyncRequestIOS : public HTTPAsyncRequest
{
public:

	DECLARE_CLASS_INFO(HTTPAsyncRequestIOS, HTTPAsyncRequest, HTTPRequestModule)

	//! constructor
    HTTPAsyncRequestIOS(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	virtual void InitModifiable();

	virtual void protectedProcess();

protected:

	//! destructor
    virtual ~HTTPAsyncRequestIOS();
};

#endif //_HTTPASYNCREQUESTIOS_H_
