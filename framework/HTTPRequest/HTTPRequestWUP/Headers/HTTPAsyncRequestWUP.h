#pragma once

#include "HTTPAsyncRequest.h"

#include <any>

class HTTPAsyncRequestWUP;

// ****************************************
// * HTTPAsyncRequestWUP class
// * --------------------------------------
/**
* \file	HTTPAsyncRequestWUP.h
* \class	HTTPAsyncRequestWUP
* \ingroup HTTPRequest
* \brief UWP specific HTTP Request.
*
*/
// ****************************************
class HTTPAsyncRequestWUP : public HTTPAsyncRequest
{
public:
	DECLARE_CLASS_INFO(HTTPAsyncRequestWUP, HTTPAsyncRequest, HTTPRequestModule)
	DECLARE_INLINE_CONSTRUCTOR(HTTPAsyncRequestWUP) {}
	
	void InitModifiable() override;
	void protectedProcess() override;

	u32 GetCurrentProgress() override { return mProgress; }

protected:

	std::atomic<u32> mProgress{ 0 };
	std::any mClient;
	std::vector<u8> mData;
	bool mIsSync;
};
