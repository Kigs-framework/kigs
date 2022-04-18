#include "HTTPAsyncRequestJS.h"
#include "HTTPConnectJS.h"
#include "Core.h"
#include "NotificationCenter.h"
#include "AsciiParserUtils.h"
#include "SmartPointer.h"
#include "CoreRawBuffer.h"

IMPLEMENT_CLASS_INFO(HTTPAsyncRequestJS)

extern "C" void	JSSendHTTPRequest(const char* type, const char* url, void* caller,bool isAsync, void* data=NULL, int data_size=0);
extern "C" void HTTPAsyncRequestJSParseContent(void* a_caller, const char* buffer, int buflen);
extern "C" void HTTPAsyncRequestJSParseHeader(void* a_caller, const char* header);
extern "C" void HTTPAsyncRequestJSParseError(void* a_caller, const char* error);

void HTTPAsyncRequestJSParseContent(void* a_caller, const char* buffer, int buflen)
{
	reinterpret_cast<HTTPAsyncRequestJS*>(a_caller)->ParseContent(buffer, buflen);
}

void HTTPAsyncRequestJSParseHeader(void* a_caller, const char* header)
{
	//printf("parse header : %s \n", header);
	reinterpret_cast<HTTPAsyncRequestJS*>(a_caller)->ParseHeader(header);
}

void HTTPAsyncRequestJSParseError(void* a_caller, const char* error) {
	reinterpret_cast<HTTPAsyncRequestJS*>(a_caller)->ParseError(error);
}


//! constructor
HTTPAsyncRequestJS::HTTPAsyncRequestJS(const kstl::string& name,CLASS_NAME_TREE_ARG) : HTTPAsyncRequest(name,PASS_CLASS_NAME_TREE_ARG)
{
}     

//! destructor
HTTPAsyncRequestJS::~HTTPAsyncRequestJS()
{
}

void		HTTPAsyncRequestJS::ParseHeader(const char* header)
{
	AsciiParserUtils	parserHeader((char*)header, strlen(header));
	if (parserHeader.MoveToString("Content-encoding: "))
	{
		AsciiParserUtils	encoding(parserHeader);
		parserHeader.GetString(encoding, '.');

		if (strcmp(encoding.c_str(), "utf-8") == 0)
		{
			mContentEncoding = UTF8;
		}
		else if (strcmp(encoding.c_str(), "utf-16") == 0)
		{
			mContentEncoding = UTF16;
		}
	}
	parserHeader.SetPosition(0);
	mFoundCharset = ANSI;
	if (parserHeader.MoveToString("Content-Type: "))
	{
		AsciiParserUtils	contenttype(parserHeader);
		parserHeader.GetString(contenttype, '.');

		mContentType = contenttype;

		// search charset in content-type
		if (contenttype.MoveToString("charset="))
		{
			AsciiParserUtils charset(contenttype);
			contenttype.GetWord(charset, ';');
			kstl::string strcharset = (const kstl::string&)charset;
			if (strcharset == "utf-8")
			{
				mFoundCharset = UTF8;
			}
			else if (strcharset == "utf-16")
			{
				mFoundCharset = UTF16;
			}

		}
	}
}

void HTTPAsyncRequestJS::ParseContent(const char* buffer, int buflen) {
	//printf("parse content (%d) : %s\n", buflen, buffer);
	/*printf("parse content : \n");
	for (int i = 0; i < buflen; i++)
		printf("%c", buffer[i]);

	printf("\n");*/
	/*if (mReceivedRawBuffer)
		delete[] mReceivedRawBuffer;

	mReceivedRawBufferSize = buflen;
	mReceivedRawBuffer = new unsigned char[buflen];

	memcpy(mReceivedRawBuffer, buffer, buflen);

	setDone();*/
	
	auto tmp= MakeRefCounted<CoreRawBuffer>((void*)buffer, (unsigned int)buflen,true);
	mReceivedBuffer = tmp.get();
	
	setDone();

	//delete[] buffer;
}

void HTTPAsyncRequestJS::ParseError(const char* error) {
	setTimedOut();
}

void HTTPAsyncRequestJS::InitModifiable()
{
	ParentClassType::InitModifiable();
	if ((CoreModifiable*)mConnection)
	{
		bool isSync = false;
		mConnection->getValue("IsSynchronous", isSync);

		kstl::string L_host, L_URL;
		mConnection->getValue("HostName)", L_host);
		L_URL = "https://" + L_host + "/" + (std::string)mURL;

		JSSendHTTPRequest(((const kstl::string&)mType).c_str(), L_URL.c_str(), this, !isSync, mPostBuffer, mPostBufferLength);
	}
}


