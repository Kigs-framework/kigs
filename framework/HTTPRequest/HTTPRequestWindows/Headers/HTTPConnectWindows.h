#pragma once

#include "HTTPConnect.h"
#include <windows.h>
#include <wininet.h>

namespace Kigs
{
	namespace Http
	{
		// ****************************************
		// * HTTPConnectWindows class
		// * --------------------------------------
		/*!  \class HTTPConnectWindows
			 manage a win32 html connection
		*/
		// ****************************************

		class HTTPConnectWindows : public HTTPConnect
		{
		public:

			DECLARE_CLASS_INFO(HTTPConnectWindows, HTTPConnect, HTTPRequestModule)

				//! constructor
				HTTPConnectWindows(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
			virtual ~HTTPConnectWindows();

			HINTERNET getHandle()
			{
				return mConnectionHandle;
			}

			virtual bool	Open();
			virtual void	Close();

		protected:
			virtual void InitModifiable();

			HINTERNET					mConnectionHandle;
			HINTERNET					mInternetHandle;
			INTERNET_STATUS_CALLBACK	mIsCallback;
		};

	}
}
