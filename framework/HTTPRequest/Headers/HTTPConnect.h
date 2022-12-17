#ifndef _HTTPCONNECT_H_
#define _HTTPCONNECT_H_

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "HTTPAsyncRequest.h"

// ****************************************
// * HTTPConnect class
// * --------------------------------------
/**
* \file	HTTPConnect.h
* \class	HTTPConnect
* \ingroup HTTPRequest
* \brief Connect to an http serveur.
*
*/
// ****************************************

class HTTPConnect : public CoreModifiable
{
public:

	DECLARE_ABSTRACT_CLASS_INFO(HTTPConnect, CoreModifiable, HTTPRequestModule)
	DECLARE_INLINE_CONSTRUCTOR(HTTPConnect) {}

	void InitModifiable() override;

	//! HTTPAsyncRequest* sendAsyncRequest(const char* a_URLRequest, const char* a_NotificationName = "", CoreModifiable::CoreModifiableMethod<CoreModifiable>* a_Func = nullptr, const char* a_RequesType = "GET")
	//! call method or post a notification with an unsigned short* in parameter (result)
	SmartPointer<HTTPAsyncRequest> retreiveGetAsyncRequest(const char* a_URLRequest, const char* a_NotificationName = nullptr, CoreModifiable* a_CallbackObject=nullptr);
	SmartPointer<HTTPAsyncRequest> retreiveDeleteAsyncRequest(const char* a_URLRequest, const char* a_NotificationName = nullptr, CoreModifiable* a_CallbackObject = nullptr);
	SmartPointer<HTTPAsyncRequest> retreivePostAsyncRequest(const char* a_URLRequest, const char* buffer, const unsigned int& a_requestSize, const char* a_NotificationName = nullptr, CoreModifiable* a_CallbackObject=nullptr);
	SmartPointer<HTTPAsyncRequest> retreivePutAsyncRequest(const char* a_URLRequest, const char* buffer, const unsigned int& a_requestSize, const char* a_NotificationName = nullptr, CoreModifiable* a_CallbackObject = nullptr);

	SmartPointer<HTTPAsyncRequest> retreiveGetRequest(const char* a_URLRequest);
	SmartPointer<HTTPAsyncRequest> retreivePostRequest(const char* a_URLRequest, const char* buffer, const unsigned int& a_requestSize);

	virtual bool	Open()=0;
	virtual void	Close()=0;

	std::string	getHostName()
	{
		return (const std::string&)mHostName;
	}

	std::string getHostNameWithProtocol();

	bool IsSync() { return mIsSynchronous; }

protected:
	maString	mHostName = BASE_ATTRIBUTE(HostName, "");
	maInt		mPort = BASE_ATTRIBUTE(Port, 80);
	maBool		mIsSynchronous = BASE_ATTRIBUTE(IsSynchronous, false);
	maEnum<2>	mType = BASE_ATTRIBUTE(Type, "HTTP", "HTTPS");
	bool		mIsOpen = false;

};

#endif //_HTTPCONNECT_H_
