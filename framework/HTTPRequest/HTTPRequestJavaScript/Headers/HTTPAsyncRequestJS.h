#ifndef _HTTPASYNCREQUESTJS_H_
#define _HTTPASYNCREQUESTJS_H_

#include "HTTPAsyncRequest.h"

// ****************************************
// * HTTPAsyncRequestJS class
// * --------------------------------------
/*!  \class HTTPAsyncRequestJS
     manage a win32 html request
*/
// ****************************************

class HTTPAsyncRequestJS : public HTTPAsyncRequest
{
public:

	DECLARE_CLASS_INFO(HTTPAsyncRequestJS, HTTPAsyncRequest, HTTPRequestModule)

	//! constructor
    HTTPAsyncRequestJS(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	virtual void			InitModifiable() override;

	void					ParseHeader(const char* header);
	void					ParseContent(const char* buffer, int buflen);
	void					ParseError(const char* error);
protected:

	//! destructor
    virtual					~HTTPAsyncRequestJS();
	
};

#endif //_HTTPASYNCREQUESTJS_H_
