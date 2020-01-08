#include "PrecompiledHeaders.h"
#include "HTTPAsyncRequest.h"
#include "Core.h"
#include "NotificationCenter.h"
#include "CoreRawBuffer.h"

IMPLEMENT_CLASS_INFO(HTTPAsyncRequest)

HTTPAsyncRequest::~HTTPAsyncRequest()
{
	ClearReceivedBuffer();

	if (myPostBuffer)
	{
		delete[] myPostBuffer;
	}
	myPostBufferLength = 0;
}    

void HTTPAsyncRequest::SetPostBufferValue(const char* a_buffer, unsigned int buflen)
{
	if (myPostBuffer)
	{
		delete[] myPostBuffer;
	}
	myPostBufferLength = buflen;

	myPostBuffer = new char[myPostBufferLength];

	memcpy(myPostBuffer, a_buffer, myPostBufferLength);

}

bool HTTPAsyncRequest::GetAnswer(usString& answer)
{
	if (myReceivedBuffer)
	{

		switch (myContentEncoding)
		{
		case ANSI:
			answer = (usString)(const char*)myReceivedBuffer->buffer();
			if (myFoundCharset != ANSI)
			{
				answer.replaceEscapeUnicode();
			}
			break;
		case UTF8:
			answer = (UTF8Char*)myReceivedBuffer->buffer();
			break;
		case UTF16:
			answer = (unsigned short*)myReceivedBuffer->buffer();
			break;
		}

		ClearReceivedBuffer();
		//delete[] myReceivedRawBuffer;
		//myReceivedRawBuffer = 0;

		return true;
	}
	return false;
}

bool HTTPAsyncRequest::GetAnswer(kstl::string& answer)
{
	if (myReceivedBuffer)
	{
		usString L_returnedValue;
		switch (myContentEncoding)
		{
		case ANSI:
			L_returnedValue = (usString)(const char*)myReceivedBuffer->buffer();
			if (myFoundCharset != ANSI)
			{
				L_returnedValue.replaceEscapeUnicode();
			}
			break;
		case UTF8:
			L_returnedValue = (UTF8Char*)myReceivedBuffer->buffer();
			break;
		case UTF16:
			L_returnedValue = (unsigned short*)myReceivedBuffer->buffer();
			break;
		}

		ClearReceivedBuffer();

		answer = L_returnedValue.ToString();
		return true;
	}
	return false;
}

bool HTTPAsyncRequest::GetAnswer(void** buffer, int& buflen)
{
	*buffer = 0;
	buflen = 0;
	if (myReceivedBuffer)
	{
		*buffer = myReceivedBuffer->buffer();
		buflen = myReceivedBuffer->length();

		return true;
	}
	return false;
}

bool HTTPAsyncRequest::GetAnswer(CoreRawBuffer*& buffer)
{
	buffer = myReceivedBuffer;
	if (myReceivedBuffer)
	{
		myReceivedBuffer->GetRef();
		return true;
	}
	return false;
}

void HTTPAsyncRequest::protectedProcess()
{
	// first check if myCallbackObject is set

	if ((const kstl::string&)myNotificationName != "")
	{
		if ((CoreModifiable*)myCallbackObject)
		{
			((CoreModifiable*)myCallbackObject)->CallMethod(myNotificationName.const_ref(), (*(kstl::vector<CoreModifiableAttribute*>*)(0)), this, this);
		}
		else // not a call but a notification
		{
			KigsCore::GetNotificationCenter()->postNotificationName(myNotificationName, this, myReceivedBuffer->buffer());
		}
	}
	else
	{
		EmitSignal(Signals::OnResponse, this);
	}
}

void HTTPAsyncRequest::ClearReceivedBuffer()
{
	if (myReceivedBuffer)
	{
		myReceivedBufferRef = "";
		myReceivedBuffer->Destroy();
		myReceivedBuffer = 0;
	}
}
