#ifndef _HTTPREQUESTMODULEWINDOWS_H_
#define _HTTPREQUESTMODULEWINDOWS_H_

#include "ModuleBase.h"
#include "Core.h"
#include <windows.h>
#include <wininet.h>

class HTTPRequestModuleWindows;
extern HTTPRequestModuleWindows* gInstanceModuleHTTPRequestWindows;


#ifdef _KIGS_ONLY_STATIC_LIB_
#define MODULEINITFUNC			PlatformHTTPRequestModuleInit
extern SP<ModuleBase> PlatformHTTPRequestModuleInit(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params);
#else
#define MODULEINITFUNC			ModuleInit
#endif

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
	HTTPRequestModuleWindows(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	virtual ~HTTPRequestModuleWindows();

	//! module init
	void Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params);

	//! module close
	void Close();         
	             
	//! module update
	virtual void Update(const Timer& timer, void* addParam);

	static HINTERNET	getInternetHandle();

	static void __stdcall WinInetCallback(HINTERNET, DWORD_PTR, DWORD, LPVOID, DWORD);
}; 

#endif //_HTTPREQUESTMODULEWINDOWS_H_
