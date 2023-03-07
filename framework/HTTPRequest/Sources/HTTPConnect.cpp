#include "PrecompiledHeaders.h"
#include "HTTPConnect.h"
#include "Core.h"

using namespace Kigs::Http;

IMPLEMENT_CLASS_INFO(HTTPConnect)

void HTTPConnect::InitModifiable()
{
	if (mHostName != "")
	{
		CoreModifiable::InitModifiable();
	}
}

SmartPointer<HTTPAsyncRequest> HTTPConnect::retreiveGetAsyncRequest(const char* a_URLRequest, const char* a_NotificationName, CoreModifiable* a_CallbackObject)
{
	if (mIsSynchronous || (!a_NotificationName && !a_CallbackObject))
		return nullptr;

	SP<HTTPAsyncRequest> request = KigsCore::GetInstanceOf("HTTPAsyncRequest_" + getName(), "HTTPAsyncRequest");
	request->setValue("Type", "GET");
	request->setValue("URL", a_URLRequest);
	request->setValue("Connection", this->SharedFromThis());
	if (a_NotificationName)
		request->setValue("Notification", a_NotificationName);
	if (a_CallbackObject)
		request->setValue("CallbackReceiver", a_CallbackObject->SharedFromThis());

	KigsCore::addAsyncRequest(request);

	
	return request;
}

SmartPointer<HTTPAsyncRequest> HTTPConnect::retreiveDeleteAsyncRequest(const char* a_URLRequest, const char* a_NotificationName, CoreModifiable* a_CallbackObject)
{
	auto ret = retreiveGetAsyncRequest(a_URLRequest, a_NotificationName, a_CallbackObject);
	if (ret)
		ret->setValue("Type", "DELETE");
	return ret;
}

SmartPointer<HTTPAsyncRequest> HTTPConnect::retreivePostAsyncRequest(const char* a_URLRequest, const char* buffer, const unsigned int& a_requestSize, const char* a_NotificationName, CoreModifiable* a_CallbackObject)
{
	if (mIsSynchronous || (!a_NotificationName && !a_CallbackObject))
		return nullptr;

	SP<HTTPAsyncRequest> request = KigsCore::GetInstanceOf("HTTPAsyncRequest_" + getName(), "HTTPAsyncRequest");
	request->setValue("Type", "POST");
	request->setValue("URL", a_URLRequest);
	request->SetPostBufferValue(buffer, a_requestSize);
	request->setValue("Connection", this->SharedFromThis());
	if (a_NotificationName)
		request->setValue("Notification", a_NotificationName);
	if (a_CallbackObject)
		request->setValue("CallbackReceiver", a_CallbackObject->SharedFromThis());

	
	KigsCore::addAsyncRequest(request);
	
	
	return request;
}

SmartPointer<HTTPAsyncRequest> HTTPConnect::retreivePutAsyncRequest(const char* a_URLRequest, const char* buffer, const unsigned int& a_requestSize, const char* a_NotificationName, CoreModifiable* a_CallbackObject)
{
	auto ret = retreivePostAsyncRequest(a_URLRequest, buffer, a_requestSize, a_NotificationName, a_CallbackObject);
	if (ret)
		ret->setValue("Type", "PUT");
	return ret;
}

SmartPointer<HTTPAsyncRequest> HTTPConnect::retreiveGetRequest(const char* a_URLRequest)
{
	if (!mIsSynchronous)
		return nullptr;

	SP<HTTPAsyncRequest> request = KigsCore::GetInstanceOf("HTTPAsyncRequest_" + getName(), "HTTPAsyncRequest");
	request->setValue("Type", "GET");
	request->setValue("URL", a_URLRequest);
	request->setValue("Connection", this->SharedFromThis());
	
	
	return request;
}

SmartPointer<HTTPAsyncRequest> HTTPConnect::retreivePostRequest(const char* a_URLRequest, const char* buffer, const unsigned int& a_requestSize)
{
	if (!mIsSynchronous)
		return 0;

	SP<HTTPAsyncRequest> request = KigsCore::GetInstanceOf("HTTPAsyncRequest_" + getName(), "HTTPAsyncRequest");
	request->setValue("Type", "POST");
	request->setValue("URL", a_URLRequest);
	request->SetPostBufferValue(buffer, a_requestSize);
	request->setValue("Connection", this->SharedFromThis());

	return request;
}

std::string HTTPConnect::getHostNameWithProtocol()
{
	std::string hostName;

	if (mType == 0)
	{
		hostName = "http://";
	}
	else
	{
		hostName = "https://";
	}

	hostName += (const std::string&)mHostName;

	return hostName;
}