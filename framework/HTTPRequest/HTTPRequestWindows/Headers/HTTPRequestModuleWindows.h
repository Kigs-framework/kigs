#pragma once

#include "ModuleBase.h"
#include "Core.h"
#include <windows.h>
#include <wininet.h>

namespace Kigs
{
	namespace Http
	{
		using namespace Kigs::Core;

		SP<ModuleBase> PlatformHTTPRequestModuleInit(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params);
		// ****************************************
		// * HTTPRequestModuleWindows class
		// * --------------------------------------
		/*!  \class HTTPRequestModuleWindows
			Windows HTTP request manager
		*/
		// ****************************************

		class HTTPRequestModuleWindows : public ModuleBase
		{
		public:
			DECLARE_CLASS_INFO(HTTPRequestModuleWindows, ModuleBase, HTTPRequestModule)
				HTTPRequestModuleWindows(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
			virtual ~HTTPRequestModuleWindows();

			//! module init
			void Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params);

			//! module close
			void Close();

			//! module update
			virtual void Update(const Time::Timer& timer, void* addParam);

			static HINTERNET	getInternetHandle();

			static void __stdcall WinInetCallback(HINTERNET, DWORD_PTR, DWORD, LPVOID, DWORD);
		};

	}
}