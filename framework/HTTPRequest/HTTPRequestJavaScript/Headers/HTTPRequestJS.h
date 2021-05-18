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

	virtual void				InitModifiable() override;
	virtual unsigned short*		GetAnswer();


protected:

	//! destructor
	virtual						~HTTPRequestJS();
	
	unsigned int				mHandle;
	unsigned int				mBufferSize;
	unsigned short*				mBuffer;
};

#endif //_HTTPREQUESTJS_H_
