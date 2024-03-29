#include "PrecompiledHeaders.h"
#include "HTTPAsyncRequest.h"
#include "Core.h"
#include "NotificationCenter.h"
#include "CoreRawBuffer.h"

using namespace Kigs::Http;

IMPLEMENT_CLASS_INFO(HTTPAsyncRequest)

HTTPAsyncRequest::~HTTPAsyncRequest()
{
	ClearReceivedBuffer();
	delete[] mPostBuffer;
	mPostBufferLength = 0;
}    

void HTTPAsyncRequest::SetPostBufferValue(const char* a_buffer, unsigned int buflen)
{
	delete[] mPostBuffer;
	mPostBufferLength = buflen;

	mPostBuffer = new char[mPostBufferLength];

	memcpy(mPostBuffer, a_buffer, mPostBufferLength);

}

bool HTTPAsyncRequest::GetAnswer(usString& answer)
{
	if (mReceivedBuffer.ref())
	{

		switch (mContentEncoding)
		{
		case ANSI:
			answer = (usString)(const char*)mReceivedBuffer.const_ref()->buffer();
			if (mFoundCharset != ANSI)
			{
				answer.replaceEscapeUnicode();
			}
			break;
		case UTF8:
			answer = (UTF8Char*)mReceivedBuffer.const_ref()->buffer();
			break;
		case UTF16:
			answer = (unsigned short*)mReceivedBuffer.const_ref()->buffer();
			break;
		}

		ClearReceivedBuffer();
		//delete[] myReceivedRawBuffer;
		//myReceivedRawBuffer = 0;

		return true;
	}
	return false;
}

bool HTTPAsyncRequest::GetAnswer(std::string& answer)
{
	if (mReceivedBuffer.ref())
	{
		usString L_returnedValue;
		switch (mContentEncoding)
		{
		case ANSI:
			L_returnedValue = (usString)(const char*)mReceivedBuffer.const_ref()->buffer();
			if (mFoundCharset != ANSI)
			{
				L_returnedValue.replaceEscapeUnicode();
			}
			break;
		case UTF8:
			L_returnedValue = (UTF8Char*)mReceivedBuffer.const_ref()->buffer();
			break;
		case UTF16:
			L_returnedValue = (unsigned short*)mReceivedBuffer.const_ref()->buffer();
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
	if (mReceivedBuffer.ref())
	{
		*buffer = mReceivedBuffer.const_ref()->buffer();
		buflen = (int)mReceivedBuffer.const_ref()->length();

		return true;
	}
	return false;
}

bool HTTPAsyncRequest::GetAnswer(SP<CoreRawBuffer>& buffer)
{
	buffer = mReceivedBuffer.const_ref();
	return (bool)buffer;
}

void HTTPAsyncRequest::protectedProcess()
{
	// first check if myCallbackObject is set

	if (mNotification != "")
	{
		CMSP	currentCallbackReceiver = getValue<CMSP>("CallbackReceiver");
		if (currentCallbackReceiver)
		{
			std::vector<CoreModifiableAttribute*> empty;
			currentCallbackReceiver->CallMethod(mNotification, empty, this, this);
		}
		else // not a call but a notification
		{
			KigsCore::GetNotificationCenter()->postNotificationName(mNotification, this, mReceivedBuffer.const_ref()->buffer());
		}
	}
	else
	{
		EmitSignal(Signals::OnResponse, this);
	}
}

void HTTPAsyncRequest::ClearReceivedBuffer()
{
	//myReceivedBuffer = nullptr;
}
