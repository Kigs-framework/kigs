#include "HTTPRequestJS.h"
#include "HTTPConnectJS.h"
#include "Core.h"
#include "NotificationCenter.h"

IMPLEMENT_CLASS_INFO(HTTPRequestJS)

extern "C" void		JSSendHTTPRequest(const char* type, const char* url, const char* a_data, unsigned int& a_bufferSize, char** a_Buffer);
extern "C" void		JSSendHTTPRequestUTF16(const char* type, const char* url, const char* a_data, unsigned int& a_bufferSize, unsigned short** a_Buffer);

//! constructor
HTTPRequestJS::HTTPRequestJS(const kstl::string& name, CLASS_NAME_TREE_ARG) :
HTTPRequest(name,PASS_CLASS_NAME_TREE_ARG)
, mHandle(NULL)
, mBufferSize(0)
, mBuffer(NULL)
{
}     

//! destructor
HTTPRequestJS::~HTTPRequestJS()
{
	if (mBuffer)
	{
		delete[] mBuffer;
		mBuffer = NULL;
	}
	mBufferSize = 0;
}    



void HTTPRequestJS::InitModifiable()
{
	HTTPRequest::InitModifiable();

	if (mConnection)
	{
		if (mBuffer)
		{
			delete[] mBuffer;
			mBuffer = NULL;
		}
		mBufferSize = 0;

		kstl::string L_host, L_URL;
		mConnection->getValue(LABEL_TO_ID(HostName), L_host);
		L_URL = "http://" + L_host + "/" + mRequestURL;
		JSSendHTTPRequestUTF16(((const kstl::string&)mRequestType).c_str(), L_URL.c_str(), NULL, mBufferSize, &mBuffer);
	}
}

unsigned short* HTTPRequestJS::GetAnswer()
{
	return mBuffer;
}