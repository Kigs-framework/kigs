#ifndef _HTTPASYNCREQUEST_H_
#define _HTTPASYNCREQUEST_H_

#include "AsyncRequest.h"
#include "CoreModifiableAttribute.h"
#include "maReference.h"
#include "maBuffer.h"
#include "AttributePacking.h"

#include <atomic>

// ****************************************
// * HTTPAsyncRequest class
// * --------------------------------------
/*!  \class HTTPAsyncRequest
     manage an async html request
*/
// ****************************************

class HTTPConnect;
class CoreRawBuffer;
class HTTPAsyncRequest : public AsyncRequest
{	
public:
	enum ContentEncoding
	{
		ANSI = 0,
		UTF8,
		UTF16
	};
	
	DECLARE_ABSTRACT_CLASS_INFO(HTTPAsyncRequest, AsyncRequest, HTTPRequestModule)
	DECLARE_INLINE_CONSTRUCTOR(HTTPAsyncRequest) {}
	SIGNALS(OnResponse,OnBufferPartReceived);
	
	void SetPostBufferValue(const char* a_buffer, unsigned int buflen);

	void AddHeader(const std::string& header)
	{
		mHeaders.push_back(header);
	}

	virtual u32 GetCurrentProgress() { return 0; }

	virtual bool	GetAnswer(usString& answer);
	virtual bool	GetAnswer(kstl::string& answer);
	virtual bool	GetAnswer(void** buffer, int& buflen);
	virtual bool	GetAnswer(CoreRawBuffer*& buffer);

	void AddUploadProgress(s32 bytes) { mUploadProgress += bytes; }
	void AddDownloadProgress(s32 bytes) { mDownloadProgress += bytes; }
	
	s32 GetUploadProgress() { return mUploadProgress; }
	s32 GetDownloadProgress() { return mDownloadProgress; }

protected:

	std::atomic<s32> mUploadProgress{ 0 };
	std::atomic<s32> mDownloadProgress{ 0 };

	void	protectedProcess() override;

	//! destructor
    virtual ~HTTPAsyncRequest();

	maReference												myConnection = BASE_ATTRIBUTE(Connection, "");
	maEnum<4>												myRequestType = BASE_ATTRIBUTE(Type, "GET", "POST", "PUT", "DELETE");
	maUInt													myFlags = BASE_ATTRIBUTE(FLAGS, 0);
	maString												myRequestURL = BASE_ATTRIBUTE(URL, "");
	maUInt													myVersion = BASE_ATTRIBUTE(Version, 0);

	// Old mode - use OnResponse signal instead
	maString												myNotificationName = BASE_ATTRIBUTE(Notification, "");
	maReference												myCallbackObject = BASE_ATTRIBUTE(CallbackReceiver, "");
	maBuffer												myReceivedBuffer = BASE_ATTRIBUTE(ReceivedBuffer, "");
	//

	ContentEncoding											myContentEncoding = ANSI;
	ContentEncoding											myFoundCharset = ANSI;
	kstl::string											myContentType;

	// post
	char*													myPostBuffer = nullptr;
	u32														myPostBufferLength = 0u;

	std::vector<std::string>								mHeaders;

	void	ClearReceivedBuffer();

	// size of "cache" buffer when receiving file part
	maInt						myReceiveBufferSize = BASE_ATTRIBUTE(ReceiveBufferSize, 4096);

	maBool						myReceiveFullAnswer = BASE_ATTRIBUTE(ReceiveFullAnswer, true);
};

#endif //_HTTPASYNCREQUEST_H_
