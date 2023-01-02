#include "PrecompiledHeaders.h"
#include "HTTPRequestModuleWindows.h"
#include "HTTPAsyncRequestWindows.h"
#include "HTTPConnectWindows.h"

using namespace Kigs::Http;

//#define USE_DEBUG_PRINT
#ifdef USE_DEBUG_PRINT
#define DEBUG_PRINT(...) fprintf(stderr,__VA_ARGS__)
#else
#define DEBUG_PRINT(...) 
#endif

IMPLEMENT_CLASS_INFO(HTTPRequestModuleWindows)

//! constructor
HTTPRequestModuleWindows::HTTPRequestModuleWindows(const std::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
	
}


//! destructor
HTTPRequestModuleWindows::~HTTPRequestModuleWindows()
{
}    

//! module init, register FilePathManager
void HTTPRequestModuleWindows::Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params)
{
    BaseInit(core,"HTTPRequestModuleWindows",params);

	DECLARE_FULL_CLASS_INFO(core, HTTPAsyncRequestWindows, HTTPAsyncRequest, HTTPRequestModule)
	DECLARE_FULL_CLASS_INFO(core, HTTPConnectWindows, HTTPConnect, HTTPRequestModule)
}

//! module close
void HTTPRequestModuleWindows::Close()
{
	BaseClose();
}    

//! module update     
void HTTPRequestModuleWindows::Update(const Timer& timer, void* addParam)
{
	BaseUpdate(timer,addParam);
}    


SP<ModuleBase> Kigs::Http::PlatformHTTPRequestModuleInit(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params)
{
	KigsCore::ModuleStaticInit(core);
	DECLARE_CLASS_INFO_WITHOUT_FACTORY(HTTPRequestModuleWindows,"HTTPRequestModuleWindows");
	auto ptr = MakeRefCounted<HTTPRequestModuleWindows>("HTTPRequestModuleWindows");
	ptr->Init(core, params);
	return ptr;
}    


VOID  HTTPRequestModuleWindows::WinInetCallback(
	HINTERNET hInternet,
	DWORD_PTR dwContext,
	DWORD dwInternetStatus,
	LPVOID lpvStatusInformation,
	DWORD dwStatusInformationLength
	)
	/*++

	Routine Description:
	Callback routine for asynchronous WinInet operations

	Arguments:
	hInternet - The handle for which the callback function is called.
	dwContext - Pointer to the application defined context.
	dwInternetStatus - Status code indicating why the callback is called.
	lpvStatusInformation - Pointer to a buffer holding callback specific data.
	dwStatusInformationLength - Specifies size of lpvStatusInformation buffer.

	Return Value:
	None.

	--*/
{
	

	InternetCookieHistory cookieHistory;
	REQUEST_CONTEXT* ReqContext = (REQUEST_CONTEXT*)dwContext;

	UNREFERENCED_PARAMETER(dwStatusInformationLength);

	DEBUG_PRINT("Callback Received for Handle %p \t", hInternet);

	switch (dwInternetStatus)
	{
	case INTERNET_STATUS_COOKIE_HISTORY:
		cookieHistory = *((InternetCookieHistory*)lpvStatusInformation);

#ifdef USE_DEBUG_PRINT
		DEBUG_PRINT("Status: Cookie History\n");
		if (cookieHistory.fAccepted)
		{
			DEBUG_PRINT("Cookie Accepted\n");
		}
		if (cookieHistory.fLeashed)
		{
			DEBUG_PRINT("Cookie Leashed\n");
		}
		if (cookieHistory.fDowngraded)
		{
			DEBUG_PRINT("Cookie Downgraded\n");
		}
		if (cookieHistory.fRejected)
		{
			DEBUG_PRINT("Cookie Rejected\n");
		}
#endif

		break;

	case INTERNET_STATUS_HANDLE_CLOSING:
		DEBUG_PRINT("Status: Handle Closing\n");

		//
		// Signal the cleanup routine that it is
		// safe to cleanup the request context
		//

		SetEvent(ReqContext->CleanUpEvent);
		break;


	case INTERNET_STATUS_REQUEST_COMPLETE:
		DEBUG_PRINT("Status: Request complete\n");
		ReqContext->mRequestCaller->ProcessRequest(ReqContext, ((LPINTERNET_ASYNC_RESULT)lpvStatusInformation)->dwError);
		break;
	case INTERNET_STATUS_RESPONSE_RECEIVED:
	{
		int nb_bytes = *((LPDWORD)lpvStatusInformation);
		ReqContext->mRequestCaller->AddDownloadProgress(nb_bytes);
		DEBUG_PRINT("Status: Response Received (%d Bytes)\n", *((LPDWORD)lpvStatusInformation));
		break;
	}
	case INTERNET_STATUS_REQUEST_SENT:
	{
		int nb_bytes = *((LPDWORD)lpvStatusInformation);
		ReqContext->mRequestCaller->AddUploadProgress(nb_bytes);
		DEBUG_PRINT("Status: Request sent (%d Bytes)\n", *((LPDWORD)lpvStatusInformation));
		break;
	}
	
#ifdef USE_DEBUG_PRINT
	case INTERNET_STATUS_COOKIE_SENT:
		DEBUG_PRINT("Status: Cookie found and will be sent with request\n");
		break;

	case INTERNET_STATUS_COOKIE_RECEIVED:
		DEBUG_PRINT("Status: Cookie Received\n");
		break;

	case INTERNET_STATUS_CLOSING_CONNECTION:
		DEBUG_PRINT("Status: Closing Connection\n");
		break;

	case INTERNET_STATUS_CONNECTED_TO_SERVER:
		DEBUG_PRINT("Status: Connected to Server\n");
		break;

	case INTERNET_STATUS_CONNECTING_TO_SERVER:
		DEBUG_PRINT("Status: Connecting to Server\n");
		break;

	case INTERNET_STATUS_CONNECTION_CLOSED:
		DEBUG_PRINT("Status: Connection Closed\n");
		break;
	case INTERNET_STATUS_HANDLE_CREATED:
		DEBUG_PRINT("Handle %x created\n",((LPINTERNET_ASYNC_RESULT)lpvStatusInformation)->dwResult);
		break;

	case INTERNET_STATUS_INTERMEDIATE_RESPONSE:
		DEBUG_PRINT("Status: Intermediate response\n");
		break;

	case INTERNET_STATUS_RECEIVING_RESPONSE:
		DEBUG_PRINT("Status: Receiving Response\n");
		break;

	case INTERNET_STATUS_REDIRECT:
		DEBUG_PRINT("Status: Redirect\n");
		break;

	case INTERNET_STATUS_DETECTING_PROXY:
		DEBUG_PRINT("Status: Detecting Proxy\n");
		break;

	case INTERNET_STATUS_RESOLVING_NAME:
		DEBUG_PRINT("Status: Resolving Name\n");
		break;

	case INTERNET_STATUS_NAME_RESOLVED:
		DEBUG_PRINT("Status: Name Resolved\n");
		break;

	case INTERNET_STATUS_SENDING_REQUEST:
		DEBUG_PRINT("Status: Sending request\n");
		break;

	case INTERNET_STATUS_STATE_CHANGE:
		DEBUG_PRINT("Status: State Change\n");
		break;

	case INTERNET_STATUS_P3P_HEADER:
		DEBUG_PRINT("Status: Received P3P header\n");
		break;
#endif
	default:
		DEBUG_PRINT("Status: Unknown (%d)\n", dwInternetStatus);
		break;
	}

	return;
}
