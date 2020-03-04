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
, myHandle(NULL)
, myBufferSize(0)
, myBuffer(NULL)
{
}     

//! destructor
HTTPRequestJS::~HTTPRequestJS()
{
	if (myBuffer)
	{
		delete[] myBuffer;
		myBuffer = NULL;
	}
	myBufferSize = 0;
}    



void HTTPRequestJS::InitModifiable()
{
	HTTPRequest::InitModifiable();

	if (myConnection)
	{
		if (myBuffer)
		{
			delete[] myBuffer;
			myBuffer = NULL;
		}
		myBufferSize = 0;

		kstl::string L_host, L_URL;
		myConnection->getValue(LABEL_TO_ID(HostName), L_host);
		L_URL = "http://" + L_host + "/" + myRequestURL;
		JSSendHTTPRequestUTF16(((const kstl::string&)myRequestType).c_str(), L_URL.c_str(), NULL, myBufferSize, &myBuffer);
	}
}

unsigned short* HTTPRequestJS::GetAnswer()
{
	return myBuffer;
}