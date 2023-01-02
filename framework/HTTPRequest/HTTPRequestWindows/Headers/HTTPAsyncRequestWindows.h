#pragma once

#include "HTTPAsyncRequest.h"
#include <windows.h>
#include <wininet.h>

namespace Kigs
{
	namespace Http
	{

		// ****************************************
		// * HTTPAsyncRequestWindows class
		// * --------------------------------------
		/*!  \class HTTPAsyncRequestWindows
			 manage a win32 html request
		*/
		// ****************************************

		enum REQ_STATE
		{
			REQ_STATE_SEND_REQ = 0,
			REQ_STATE_SEND_REQ_WITH_BODY,
			REQ_STATE_POST_GET_DATA,
			REQ_STATE_POST_SEND_DATA,
			REQ_STATE_POST_COMPLETE,
			REQ_STATE_RESPONSE_RECV_DATA,
			REQ_STATE_RESPONSE_RECV_HEADER,
			REQ_STATE_RESPONSE_WRITE_DATA,
			REQ_STATE_COMPLETE,
		};

		class HTTPAsyncRequestWindows;

		typedef struct _REQUEST_CONTEXT
		{
			HINTERNET RequestHandle;
			HINTERNET ConnectHandle;
			HANDLE CompletionEvent;
			HANDLE CleanUpEvent;
			DWORD DownloadedBytes;
			DWORD WrittenBytes;
			DWORD ReadBytes;
			DWORD Method;
			DWORD State;
			LPSTR OutputBuffer;

			CRITICAL_SECTION CriticalSection;
			bool CritSecInitialized;

			//
			// Synchronized by CriticalSection
			//

			DWORD HandleUsageCount; // Request object is in use(not safe to close handle)
			bool Closing;           // Request is closing(don't use handle)

			HTTPAsyncRequestWindows* mRequestCaller;

		} REQUEST_CONTEXT;

		class HTTPAsyncRequestWindows : public HTTPAsyncRequest
		{
		public:
			DECLARE_CLASS_INFO(HTTPAsyncRequestWindows, HTTPAsyncRequest, HTTPRequestModule)
				HTTPAsyncRequestWindows(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
			virtual ~HTTPAsyncRequestWindows();

			void protectedProcess() override;

			void ProcessRequest(
				REQUEST_CONTEXT* ReqContext,
				DWORD Error
			);

			DWORD SendRequest(
				REQUEST_CONTEXT* ReqContext
			);


			DWORD CompleteRequest(
				REQUEST_CONTEXT* ReqContext
			);

			DWORD RecvHeader(
				REQUEST_CONTEXT* ReqContext
			);

			DWORD RecvResponseData(
				REQUEST_CONTEXT* ReqContext
			);

			DWORD WriteResponseData(
				REQUEST_CONTEXT* ReqContext,
				bool* Eof
			);


			//
			// Initialization functions
			//

			DWORD AllocateAndInitializeRequestContext(
				HINTERNET SessionHandle,
				REQUEST_CONTEXT** ReqContext
			);

			DWORD CreateWininetHandles(
				REQUEST_CONTEXT* ReqContext,
				HINTERNET SessionHandle,
				bool IsSecureConnection
			);


			//
			// Cleanup functions
			//

			void CleanUpRequestContext(
				REQUEST_CONTEXT* ReqContext
			);


			void CleanUpSessionHandle(
				HINTERNET SessionHandle
			);

			//
			// Cancellation support functions
			//


			void CloseRequestHandle(
				REQUEST_CONTEXT* ReqContext
			);

			bool AcquireRequestHandle(
				REQUEST_CONTEXT* ReqContext
			);

			void ReleaseRequestHandle(
				REQUEST_CONTEXT* ReqContext
			);


			/*static bool QueryInfos(HTTPAsyncRequestWindows* caller);*/

			int WriteResponse();


			void WaitForRequestCompletion(
				REQUEST_CONTEXT* ReqContext,
				DWORD Timeout
			);

			/*HINTERNET	getHandle()
			{
				return myHandle;

			}*/


		protected:
			void InitModifiable() override;

			struct receivedBuffer
			{
				char* mBuffer;
				int		mSize;
			};

			std::vector<receivedBuffer>	mBufferVector;

			//HINTERNET			myHandle;
			REQUEST_CONTEXT* mRequestContext;

			bool mIsSync;
		};

	}
}
