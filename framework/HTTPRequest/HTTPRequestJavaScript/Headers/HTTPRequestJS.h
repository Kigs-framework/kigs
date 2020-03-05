#ifndef _HTTPREQUESTJS_H_
#define _HTTPREQUESTJS_H_

#include "HTTPRequest.h"

// ****************************************
// * HTTPRequestJS class
// * --------------------------------------
/*!  \class HTTPRequestJS
     manage a win32 html request
*/
// ****************************************

class HTTPRequestJS : public HTTPRequest
{
public:

	DECLARE_CLASS_INFO(HTTPRequestJS, HTTPRequest, HTTPRequestModule)

	//! constructor
	HTTPRequestJS(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	virtual void				InitModifiable();
	virtual unsigned short*		GetAnswer();


protected:

	//! destructor
	virtual						~HTTPRequestJS();
	
	unsigned int				myHandle;
	unsigned int				myBufferSize;
	unsigned short*				myBuffer;
};

#endif //_HTTPREQUESTJS_H_
