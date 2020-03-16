#include "PrecompiledHeaders.h"
#include "HTTPAsyncRequestWUP.h"
#include "HTTPConnectWUP.h"
#include "Core.h"
#include "NotificationCenter.h"
#include "AsciiParserUtils.h"

//#include "ppltasks.h"
#include "winrt/Windows.Web.Http.h"
#include "winrt/Windows.Foundation.h"
#include "winrt/Windows.Storage.Streams.h"
#include "winrt/Windows.Foundation.Collections.h"



using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::Foundation::Collections;
//using namespace Concurrency;

static std::vector<u8> GetDataFromIBuffer(IBuffer buf)
{
	auto reader = DataReader::FromBuffer(buf);
	auto len = buf.Length();
	std::vector<u8> data(reader.UnconsumedBufferLength());

	if (!data.empty())
		reader.ReadBytes(data);

	return data;
}


IMPLEMENT_CLASS_INFO(HTTPAsyncRequestWUP)

static auto GetDataFromHTTPContent(IHttpContent content)
{
	std::mutex mtx;
	std::condition_variable cv;
	bool b = false;
	IBuffer buffer = nullptr;
	content.ReadAsBufferAsync().Completed([&](IAsyncOperationWithProgress<IBuffer, u64> m, AsyncStatus s)
	{
		std::lock_guard<std::mutex> lock(mtx);
		if (s == AsyncStatus::Completed)
		{
			buffer = m.GetResults();
			b = true;
			cv.notify_one();
		}
	});

	{
		std::unique_lock<std::mutex> lock(mtx);
		cv.wait(lock, [&]() { return b; });
	}
	return GetDataFromIBuffer(buffer);
}

void HTTPAsyncRequestWUP::InitModifiable()
{
	HTTPAsyncRequest::InitModifiable();
	if (myConnection)
	{
		//mClient = ref new Windows::Web::Http::HttpClient;
		auto& client = mClient.emplace<HttpClient>();
		
		


		auto hostname = myConnection->getValue<std::string>("HostName");
		auto type = myConnection->getValue<std::string>("Type") == "HTTP" ? "http://" : "https://";

		auto uri = type + hostname + ":" + std::to_string(myConnection->getValue<int>("Port")) + myRequestURL.const_ref();

		std::wstring uriL;
		std::transform(uri.begin(), uri.end(), std::back_inserter(uriL), [](auto& a){ return a;	});
		Uri request_uri = uriL.c_str();

		
		IAsyncOperationWithProgress<HttpResponseMessage, HttpProgress> async_op;
		auto request_type = (std::string)myRequestType;
		if (request_type == "GET")
		{
			async_op = client.GetAsync(request_uri);
		}
		else if (request_type == "POST")
		{
			DataWriter writer{};
			writer.WriteBytes({ (u8*)myPostBuffer, (u8*)myPostBuffer + myPostBufferLength });
			auto ibuffer = writer.DetachBuffer();
			HttpBufferContent content{ ibuffer };
			async_op = client.PostAsync(request_uri, content);
		}
		else if (request_type == "PUT")
		{
			DataWriter writer{};
			writer.WriteBytes({ (u8*)myPostBuffer, (u8*)myPostBuffer + myPostBufferLength });
			auto ibuffer = writer.DetachBuffer();
			HttpBufferContent content{ ibuffer };
			async_op = client.PutAsync(request_uri, content);
		}
		
		bool isSync = false;
		myConnection->getValue("IsSynchronous", isSync);

		if (isSync)
		{
			bool b = false;
			std::mutex mtx;
			std::condition_variable cv;
			HttpResponseMessage message = nullptr;
			HttpProgress progress;
			
			async_op.Completed([&](IAsyncOperationWithProgress<HttpResponseMessage, HttpProgress> m, AsyncStatus s)
			{
				std::lock_guard<std::mutex> lock(mtx);
				if (s == AsyncStatus::Completed)
				{
					message = m.GetResults();
				}
				else if (s == AsyncStatus::Canceled)
				{
					setCancelled();
				}
				else if (s == AsyncStatus::Error)
				{
					setTimedOut();
				}
				else
				{
					// Started
					return;
				}
				b = true;
				cv.notify_one();
			});

			
			{
				std::unique_lock<std::mutex> lock(mtx); 
				cv.wait(lock, [&]() { return b; });
			}
			

			bool success = message.IsSuccessStatusCode();
			
			b = false;
			IBuffer buffer = nullptr;

			auto phrase = message.ReasonPhrase();
			
			mData = GetDataFromHTTPContent(message.Content());
			setDone();
		}
		else
		{
			
			async_op.Progress([this](IAsyncOperationWithProgress<HttpResponseMessage, HttpProgress> message, HttpProgress progress)
			{
				mProgress = progress.BytesReceived;
			});
			async_op.Completed([this](IAsyncOperationWithProgress<HttpResponseMessage, HttpProgress> message, AsyncStatus s)
			{
				if (s == AsyncStatus::Completed)
				{
					mData = GetDataFromHTTPContent(message.GetResults().Content());
					setDone();
				}
				else if (s == AsyncStatus::Canceled)
				{
					setCancelled();
				}
				else if (s == AsyncStatus::Error)
				{
					setTimedOut();
				}
				else
				{
					// Started
				}
			});
		}
	}
}

void HTTPAsyncRequestWUP::protectedProcess()
{
	auto data = new char[mData.size()];
	memcpy(data, mData.data(), mData.size());
	auto buf = OwningRawPtrToSmartPtr(new CoreRawBuffer(data, mData.size(), true));
	myReceivedBuffer = buf.get();
	mData.clear();
	mData.shrink_to_fit();
	mClient.reset();
	ParentClassType::protectedProcess();
}