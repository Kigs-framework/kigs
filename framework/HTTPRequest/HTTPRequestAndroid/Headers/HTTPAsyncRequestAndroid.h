#ifndef _HTTPASYNCREQUESTANDROID_H_
#define _HTTPASYNCREQUESTANDROID_H_

#include "HTTPAsyncRequest.h"
#include <jni.h>

// ****************************************
// * HTTPAsyncRequestAndroid class
// * --------------------------------------
/*!  \class HTTPAsyncRequestAndroid
     manage an android html request
*/
// ****************************************

class HTTPAsyncRequestAndroid : public HTTPAsyncRequest
{
public:

	DECLARE_CLASS_INFO(HTTPAsyncRequestAndroid, HTTPAsyncRequest, HTTPRequestModule)

	//! constructor
    HTTPAsyncRequestAndroid(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	void InitModifiable() override;

	void protectedProcess() override;
	void GetData();

	static int requestIndex;
	static kstl::map<int, HTTPAsyncRequestAndroid*> requestMap;
	jobject myRequest;
	
protected:

	//! destructor
    virtual ~HTTPAsyncRequestAndroid();
	
	jclass				myCls;
	
	jmethodID query_method;
	jmethodID getsize_method;
	jmethodID getcontent_enc_method;
	jmethodID getcontent_dec_method;
	jmethodID getresult_method;
	
};

#endif //_HTTPASYNCREQUESTANDROID_H_
