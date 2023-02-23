#pragma once

#include "AsyncRequest.h"
#include "CoreModifiableAttribute.h"
#include "maReference.h"
#include "maBuffer.h"
#include "AttributePacking.h"

#include <atomic>

namespace Kigs
{
	namespace Core
	{
		class CoreRawBuffer;
	}
	namespace Http
	{
		using namespace Kigs::Core;

		class HTTPConnect;

		// ****************************************
		// * HTTPAsyncRequest class
		// * --------------------------------------
		/**
		* \file	HTTPAsyncRequest.h
		* \class	HTTPAsyncRequest
		* \ingroup HTTPRequest
		* \brief Manage Async HTTP Request
		*
		*/
		// ****************************************
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
			virtual ~HTTPAsyncRequest();

			SIGNALS(OnResponse, OnBufferPartReceived);

			void SetPostBufferValue(const char* a_buffer, unsigned int buflen);

			void AddHeader(const std::string& header)
			{
				mHeaders.push_back(header);
			}
			void ClearHeaders()
			{
				mHeaders.clear();
			}


			virtual u32 GetCurrentProgress() { return 0; }

			virtual bool	GetAnswer(usString& answer);
			virtual bool	GetAnswer(std::string& answer);
			virtual bool	GetAnswer(void** buffer, int& buflen);
			virtual bool	GetAnswer(SP<CoreRawBuffer>& buffer);

			void AddUploadProgress(s32 bytes) { mUploadProgress += bytes; }
			void AddDownloadProgress(s32 bytes) { mDownloadProgress += bytes; }

			s32 GetUploadProgress() { return mUploadProgress; }
			s32 GetDownloadProgress() { return mDownloadProgress; }

		protected:

			std::atomic<s32> mUploadProgress{ 0 };
			std::atomic<s32> mDownloadProgress{ 0 };

			void	protectedProcess() override;
			void	ClearReceivedBuffer();

			bool													mReceiveFullAnswer = true;
			u32														mFLAGS = 0;
			// size of "cache" buffer when receiving file part
			s32														mReceiveBufferSize = 4096;
			u32														mVersion = 0;

			std::string												mURL = "";
			// Old mode - use OnResponse signal instead
			std::string												mNotification = "";
			std::weak_ptr<CoreModifiable>							mCallbackReceiver;
			std::weak_ptr<CoreModifiable>							mConnection;

			WRAP_ATTRIBUTES(mReceiveFullAnswer, mFLAGS, mReceiveBufferSize, mVersion, mURL, mNotification, mCallbackReceiver, mConnection);
			//

			ContentEncoding											mContentEncoding = ANSI;
			ContentEncoding											mFoundCharset = ANSI;
			std::string												mContentType;

			// post
			char*													mPostBuffer = nullptr;
			u32														mPostBufferLength = 0u;

			std::vector<std::string>								mHeaders;

			maEnum<4>												mType = BASE_ATTRIBUTE(Type, "GET", "POST", "PUT", "DELETE");
			maBuffer												mReceivedBuffer = BASE_ATTRIBUTE(ReceivedBuffer, "");
		};

	}
}
