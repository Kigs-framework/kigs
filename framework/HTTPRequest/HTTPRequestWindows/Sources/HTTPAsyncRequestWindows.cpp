#include "PrecompiledHeaders.h"
#include "HTTPAsyncRequestWindows.h"
#include "HTTPConnectWindows.h"
#include "Core.h"
#include "NotificationCenter.h"
#include "AsciiParserUtils.h"


IMPLEMENT_CLASS_INFO(HTTPAsyncRequestWindows)

IMPLEMENT_CONSTRUCTOR(HTTPAsyncRequestWindows)
, myRequestContext(0)
{
	//default value
	myFlags = INTERNET_FLAG_NO_CACHE_WRITE |
		INTERNET_FLAG_KEEP_CONNECTION |
		INTERNET_FLAG_PRAGMA_NOCACHE;
}     

HTTPAsyncRequestWindows::~HTTPAsyncRequestWindows()
{
	if (myRequestContext)
	{
		CleanUpRequestContext(myRequestContext);
		myRequestContext = 0;
	}
}    




void HTTPAsyncRequestWindows::InitModifiable()
{
	ParentClassType::InitModifiable();
	if ((CoreModifiable*)myConnection)
	{
		
		AllocateAndInitializeRequestContext(((HTTPConnectWindows*)(CoreModifiable*)myConnection)->getHandle(), &myRequestContext);
		
		ProcessRequest(myRequestContext, ERROR_SUCCESS);

		bool isSync = false;

		myConnection->getValue("IsSynchronous", isSync);

		if (isSync)
		{
			// wait till done
			WaitForRequestCompletion(myRequestContext, 2 * 60 * 1000);
		}
		
	}
}

void HTTPAsyncRequestWindows::WaitForRequestCompletion(
	REQUEST_CONTEXT* ReqContext,
	DWORD Timeout
)
/*++

Routine Description:
Wait for the request to complete or timeout to occur

Arguments:
ReqContext - Pointer to request context structure

Return Value:
None.

--*/
{
	DWORD SyncResult;

	//
	// The preferred method of doing timeouts is to
	// use the timeout options through InternetSetOption,
	// but this overall timeout is being used to show 
	// the correct way to abort and close a request.
	//

	SyncResult = WaitForSingleObject(ReqContext->CompletionEvent,
		Timeout);              // Wait until we receive the completion

	switch (SyncResult)
	{
	case WAIT_OBJECT_0:

		//printf("Done!\n");
		break;

	case WAIT_TIMEOUT:
#ifdef _DEBUG
		fprintf(stderr,
			"Timeout while waiting for completion event (request will be cancelled)\n");
#endif
		setTimedOut();
		break;

	case WAIT_FAILED:
#ifdef _DEBUG
		fprintf(stderr,
			"Wait failed with Error %d while waiting for completion event (request will be cancelled)\n",
			GetLastError());
#endif
		setTimedOut();
		break;

	default:
		// Not expecting any other error codes

		break;


	}

	return;
}

int HTTPAsyncRequestWindows::WriteResponse()
{

	ClearReceivedBuffer();

	// retreive total size

	unsigned int ReceivedRawBufferSize = 0;
	if (myReceiveFullAnswer == true)
	{
		std::vector<receivedBuffer>::iterator it;
		for (it = myBufferVector.begin(); it != myBufferVector.end(); it++)
		{
			ReceivedRawBufferSize += (*it).mSize;
		}

		// create full buffer, copy to vector and destroy buffers in vector

		if (ReceivedRawBufferSize)
		{

			unsigned char* ReceivedRawBuffer = new unsigned char[ReceivedRawBufferSize + 2]; // add two zero
			ReceivedRawBuffer[ReceivedRawBufferSize] = 0;
			ReceivedRawBuffer[ReceivedRawBufferSize + 1] = 0;
			unsigned char*	writebuff = ReceivedRawBuffer;

			for (it = myBufferVector.begin(); it != myBufferVector.end(); it++)
			{
				memcpy(writebuff, (*it).mBuffer, (*it).mSize);
				writebuff += (*it).mSize;
			}

			CoreRawBuffer*	tmp= new CoreRawBuffer(ReceivedRawBuffer, ReceivedRawBufferSize);
			myReceivedBuffer = tmp;
			tmp->Destroy();

		}
		for (it = myBufferVector.begin(); it != myBufferVector.end(); it++)
		{
			delete[](*it).mBuffer;
		}
		myBufferVector.clear();
	}
	return ReceivedRawBufferSize;
}

void HTTPAsyncRequestWindows::protectedProcess()
{
	if (myRequestContext)
	{
		CleanUpRequestContext(myRequestContext);
		myRequestContext = 0;
	}
	HTTPAsyncRequest::protectedProcess();

}


void HTTPAsyncRequestWindows::ProcessRequest(
 REQUEST_CONTEXT* ReqContext,
 DWORD Error
)
/*++

Routine Description:
Process the request context - Sending the request and
receiving the response

Arguments:
ReqContext - Pointer to request context structure
Error - error returned from last asynchronous call

Return Value:
None.

--*/
{
	bool Eof = FALSE;

	while (Error == ERROR_SUCCESS && ReqContext->State != REQ_STATE_COMPLETE)
	{

		switch (ReqContext->State)
		{
		case REQ_STATE_POST_SEND_DATA:
		case REQ_STATE_SEND_REQ:
		
			ReqContext->State = REQ_STATE_RESPONSE_RECV_HEADER;
			Error = SendRequest(ReqContext);

			break;

		case REQ_STATE_SEND_REQ_WITH_BODY:

			ReqContext->State = REQ_STATE_POST_GET_DATA;

			break;

		case REQ_STATE_POST_GET_DATA:

			ReqContext->State = REQ_STATE_POST_SEND_DATA;
		
			break;

		case REQ_STATE_POST_COMPLETE:

			ReqContext->State = REQ_STATE_RESPONSE_RECV_DATA;
			Error = CompleteRequest(ReqContext);

			break;

		case REQ_STATE_RESPONSE_RECV_DATA:
			ReqContext->State = REQ_STATE_RESPONSE_WRITE_DATA;
			Error = RecvResponseData(ReqContext);
			break;

		case REQ_STATE_RESPONSE_RECV_HEADER:
			ReqContext->State = REQ_STATE_RESPONSE_RECV_DATA;
			Error = RecvHeader(ReqContext);
			break;

		case REQ_STATE_RESPONSE_WRITE_DATA:

			ReqContext->State = REQ_STATE_RESPONSE_RECV_DATA;
			Error = WriteResponseData(ReqContext, &Eof);

			if (Eof)
			{
				ReqContext->State = REQ_STATE_COMPLETE;
			}

			break;

		default:

			break;
		}
	}

	if (Error != ERROR_IO_PENDING)
	{
		//
		// Everything has been procesed or has failed. 
		// In either case, the signal processing has
		// completed
		//
		// Copy buffer list in myReceivedRawBuffer
		
		WriteResponse();
		SetEvent(ReqContext->CompletionEvent);
		setDone();
	}
	else
	{		
		if (static_cast<HTTPConnectWindows*>(static_cast<CoreModifiable*>(myConnection))->IsSync())
			setTimedOut();
	}

	return;
}

DWORD HTTPAsyncRequestWindows::SendRequest(
 REQUEST_CONTEXT* ReqContext
)
/*++

Routine Description:
Send the request using HttpSendRequest

Arguments:
ReqContext - Pointer to request context structure

Return Value:
Error code for the operation.

--*/
{
	bool Success;
	DWORD Error = ERROR_SUCCESS;
	char* L_Data = NULL;
	unsigned int L_DataLength = 0;

	Success = AcquireRequestHandle(ReqContext);
	if (!Success)
	{
		Error = ERROR_OPERATION_ABORTED;
		ReleaseRequestHandle(ReqContext);
		return Error;
	}

	auto type = (const std::string&)myRequestType;
	if (type == "POST" || type == "PUT")
	{
		L_Data = myPostBuffer;
		L_DataLength = myPostBufferLength;
	}

	if (mHeaders.size())
	{
		for (auto& h : mHeaders)
		{
			if (!HttpAddRequestHeaders(ReqContext->RequestHandle, h.data(), h.size(), HTTP_ADDREQ_FLAG_ADD_IF_NEW))
			{
				Error = GetLastError();
				ReleaseRequestHandle(ReqContext);
				return Error;
			}
		}
	}
	
	
	Success = HttpSendRequest(ReqContext->RequestHandle,
		nullptr,                   // do not provide additional Headers
		0L,                      // dwHeadersLength 
		L_Data,                   // Do not send any data 
		L_DataLength);                     // dwOptionalLength 

	

	if (!Success)
	{
		Error = GetLastError();

		if (Error != ERROR_IO_PENDING)
		{
			//	LogInetError(Error, L"HttpSendRequest");
#ifdef _DEBUG
			printf("Error HttpSendRequest\n");
#endif
		}
	}

	ReleaseRequestHandle(ReqContext);
	return Error;
}




DWORD
HTTPAsyncRequestWindows::CompleteRequest(
 REQUEST_CONTEXT* ReqContext
)
/*++

Routine Description:
Perform completion of asynchronous post.

Arguments:
ReqContext - Pointer to request context structure

Return Value:
Error Code for the operation.

--*/
{

	DWORD Error = ERROR_SUCCESS;
	bool Success;
#ifdef _DEBUG
	fprintf(stderr, "Finished posting file\n");
#endif
	Success = AcquireRequestHandle(ReqContext);
	if (!Success)
	{
		Error = ERROR_OPERATION_ABORTED;
		goto Exit;
	}

	Success = HttpEndRequest(ReqContext->RequestHandle, NULL, 0, 0);

	ReleaseRequestHandle(ReqContext);

	if (!Success)
	{
		Error = GetLastError();
		if (Error == ERROR_IO_PENDING)
		{
#ifdef _DEBUG
			fprintf(stderr, "Waiting for HttpEndRequest to complete \n");
#endif
		}
		else
		{
			//LogInetError(Error, L"HttpEndRequest");
#ifdef _DEBUG
			printf("Error HttpEndRequest\n");
#endif
			goto Exit;

		}
	}

Exit:

	return Error;
}

void HTTPAsyncRequestWindows::CloseRequestHandle(
 REQUEST_CONTEXT* ReqContext
)
/*++

Routine Description:
Safely  close the request handle by synchronizing
with all threads using the handle.

When this function returns no more calls can be made with the
handle.

Arguments:
ReqContext - Pointer to Request context structure
Return Value:
None.

--*/
{
	bool Close = FALSE;

	EnterCriticalSection(&ReqContext->CriticalSection);

	//
	// Current implementation only supports the main thread
	// kicking off the request handle close
	//
	// To support multiple threads the lifetime 
	// of the request context must be carefully controlled
	// (most likely guarded by refcount/critsec)
	// so that they are not trying to abort a request
	// where the context has already been freed.
	//

	ReqContext->Closing = TRUE;

	if (ReqContext->HandleUsageCount == 0)
	{
		Close = TRUE;
	}

	LeaveCriticalSection(&ReqContext->CriticalSection);



	if (Close)
	{
		//
		// At this point there must be the guarantee that all calls
		// to wininet with this handle have returned with some value
		// including ERROR_IO_PENDING, and none will be made after
		// InternetCloseHandle.
		//        
		InternetCloseHandle(ReqContext->RequestHandle);
	}

	return;
}

bool
HTTPAsyncRequestWindows::AcquireRequestHandle(
 REQUEST_CONTEXT* ReqContext
)
/*++

Routine Description:
Acquire use of the request handle to make a wininet call
Arguments:
ReqContext - Pointer to Request context structure
Return Value:
TRUE - Success
FALSE - Failure
--*/
{
	bool Success = TRUE;

	EnterCriticalSection(&ReqContext->CriticalSection);

	if (ReqContext->Closing == TRUE)
	{
		Success = FALSE;
	}
	else
	{
		ReqContext->HandleUsageCount++;
	}

	LeaveCriticalSection(&ReqContext->CriticalSection);

	return Success;
}


void HTTPAsyncRequestWindows::ReleaseRequestHandle(
 REQUEST_CONTEXT* ReqContext
)
/*++

Routine Description:
release use of the request handle
Arguments:
ReqContext - Pointer to Request context structure
Return Value:
None.

--*/
{
	bool Close = FALSE;

	EnterCriticalSection(&ReqContext->CriticalSection);

	ReqContext->HandleUsageCount--;

	if (ReqContext->Closing == TRUE && ReqContext->HandleUsageCount == 0)
	{
		Close = TRUE;

	}

	LeaveCriticalSection(&ReqContext->CriticalSection);


	if (Close)
	{
		//
		// At this point there must be the guarantee that all calls
		// to wininet with this handle have returned with some value
		// including ERROR_IO_PENDING, and none will be made after
		// InternetCloseHandle.
		//        
		InternetCloseHandle(ReqContext->RequestHandle);
	}

	return;
}

DWORD HTTPAsyncRequestWindows::RecvHeader(
	REQUEST_CONTEXT* ReqContext
	)
{

	DWORD Error = ERROR_SUCCESS;
	bool Success;
	unsigned char* lpOutBuffer = NULL;
	DWORD dwSize = 0;
	Success = AcquireRequestHandle(ReqContext);
	if (!Success)
	{
		Error = ERROR_OPERATION_ABORTED;
		goto Exit;
	}


	HttpQueryInfoA(ReqContext->RequestHandle, HTTP_QUERY_RAW_HEADERS, (void*)lpOutBuffer, &dwSize, NULL);
	myContentEncoding = ANSI;
	myContentType = "";
	if (dwSize)
	{
		lpOutBuffer = new unsigned char[dwSize + 1];
		HttpQueryInfoA(ReqContext->RequestHandle, HTTP_QUERY_RAW_HEADERS, (void*)lpOutBuffer, &dwSize, NULL);

		AsciiParserUtils	parserHeader((char*)lpOutBuffer, dwSize);
		if (parserHeader.MoveToString("Content-encoding: "))
		{
			AsciiParserUtils	encoding(parserHeader);
			parserHeader.GetString(encoding, '.');

			if (strcmp(encoding.c_str(), "utf-8") == 0)
			{
				myContentEncoding = UTF8;
			}
			else if (strcmp(encoding.c_str(), "utf-16") == 0)
			{
				myContentEncoding = UTF16;
			}
		}
		parserHeader.SetPosition(0);
		myFoundCharset = ANSI;
		if (parserHeader.MoveToString("Content-Type: "))
		{
			AsciiParserUtils	contenttype(parserHeader);
			parserHeader.GetString(contenttype, '.');

			myContentType = contenttype;

			// search charset in content-type
			if (contenttype.MoveToString("charset="))
			{
				AsciiParserUtils charset(contenttype);
				contenttype.GetWord(charset, ';');
				kstl::string strcharset = (const kstl::string&)charset;
				if (strcharset == "utf-8")
				{
					myFoundCharset = UTF8;
				}
				else if (strcharset == "utf-16")
				{
					myFoundCharset = UTF16;
				}

			}
		}

		delete[] lpOutBuffer;
	}
	ReleaseRequestHandle(ReqContext);

	if (!Success)
	{
		Error = GetLastError();
#ifdef _DEBUG
		if (Error == ERROR_IO_PENDING)
		{
			fprintf(stderr, "Waiting for InternetReadFile to complete\n");
		}
		else
		{
			fprintf(stderr, "Error InternetReadFile\n");
		}
#endif
		goto Exit;
	}


Exit:

	return Error;
}


DWORD HTTPAsyncRequestWindows::RecvResponseData(
REQUEST_CONTEXT* ReqContext
)
/*++

Routine Description:
Receive response

Arguments:
ReqContext - Pointer to request context structure

Return Value:
Error Code for the operation.

--*/
{
	DWORD Error = ERROR_SUCCESS;
	bool Success;


	Success = AcquireRequestHandle(ReqContext);
	if (!Success)
	{
		Error = ERROR_OPERATION_ABORTED;
		goto Exit;
	}

	//
	// The lpdwNumberOfBytesRead parameter will be
	// populated on async completion, so it must exist
	// until INTERNET_STATUS_REQUEST_COMPLETE.
	// The same is true of lpBuffer parameter.
	//
	// InternetReadFile will block until the buffer
	// is completely filled or the response is exhausted.
	//


	Success = InternetReadFile(ReqContext->RequestHandle,
		ReqContext->OutputBuffer,
		myReceiveBufferSize,
		&ReqContext->DownloadedBytes);

	ReleaseRequestHandle(ReqContext);

	if (!Success)
	{

		Error = GetLastError();
#ifdef _DEBUG
		if (Error == ERROR_IO_PENDING)
		{
			fprintf(stderr, "Waiting for InternetReadFile to complete\n");
		}
		else
		{
			fprintf(stderr, "Error InternetReadFile\n");
		}
#endif
		goto Exit;
	}


Exit:

	return Error;
}


DWORD HTTPAsyncRequestWindows::WriteResponseData(
REQUEST_CONTEXT* ReqContext,
bool* Eof
)
/*++

Routine Description:
Write response to a file

Arguments:
ReqContext - Pointer to request context structure
Eof - Done with response

Return Value:
Error Code for the operation.

--*/
{
	DWORD Error = ERROR_SUCCESS;
	DWORD BytesWritten = 0;

	*Eof = FALSE;

	//
	// Finished receiving response
	//

	if (ReqContext->DownloadedBytes == 0)
	{
		*Eof = true;
		goto Exit;

	}

	if (myReceiveFullAnswer == true)
	{
		receivedBuffer toAdd;
		toAdd.mBuffer = (char*)malloc(ReqContext->DownloadedBytes);
		memcpy(toAdd.mBuffer, ReqContext->OutputBuffer, ReqContext->DownloadedBytes);
		toAdd.mSize = ReqContext->DownloadedBytes;

		myBufferVector.push_back(toAdd);
	}
	else
	{
		EmitSignal(Signals::OnBufferPartReceived,this,(void*) ReqContext->OutputBuffer, (int)ReqContext->DownloadedBytes);
	}

Exit:

	return Error;
}


DWORD
HTTPAsyncRequestWindows::AllocateAndInitializeRequestContext(
 HINTERNET SessionHandle,
 REQUEST_CONTEXT**ReqContext
)
/*++

Routine Description:
Allocate the request context and initialize it values

Arguments:
ReqContext - Pointer to Request context structure
Configuration - Pointer to configuration structure
SessionHandle - Wininet session handle to use when creating
connect handle

Return Value:
Error Code for the operation.

--*/
{
	DWORD Error = ERROR_SUCCESS;
	bool Success;
	REQUEST_CONTEXT* LocalReqContext;

	*ReqContext = NULL;

	LocalReqContext = (REQUEST_CONTEXT*)malloc(sizeof(REQUEST_CONTEXT));
	bool isSecure = false;
	int connecttype = 0;
	if (LocalReqContext == NULL)
	{
		Error = ERROR_NOT_ENOUGH_MEMORY;
		goto Exit;
	}

	LocalReqContext->RequestHandle = NULL;
	LocalReqContext->ConnectHandle = NULL;
	LocalReqContext->DownloadedBytes = 0;
	LocalReqContext->WrittenBytes = 0;
	LocalReqContext->ReadBytes = 0;
	LocalReqContext->HandleUsageCount = 0;
	LocalReqContext->Closing = FALSE;
	LocalReqContext->Method = (int)(myRequestType);
	LocalReqContext->CompletionEvent = NULL;
	LocalReqContext->CleanUpEvent = NULL;
	LocalReqContext->OutputBuffer = NULL;
	LocalReqContext->State =
		(LocalReqContext->Method == 0) ? REQ_STATE_SEND_REQ : REQ_STATE_SEND_REQ_WITH_BODY;
	LocalReqContext->CritSecInitialized = FALSE;

	LocalReqContext->myRequestCaller = this;


	// initialize critical section

	Success = InitializeCriticalSectionAndSpinCount(&LocalReqContext->CriticalSection, 4000);

	if (!Success)
	{
		Error = GetLastError();
	//	LogSysError(Error, L"InitializeCriticalSectionAndSpinCount");
#ifdef _DEBUG
		printf("Error InitializeCriticalSectionAndSpinCount\n");
#endif
		goto Exit;
	}

	LocalReqContext->CritSecInitialized = TRUE;

	LocalReqContext->OutputBuffer = (LPSTR)malloc(myReceiveBufferSize);

	if (LocalReqContext->OutputBuffer == NULL)
	{
		Error = ERROR_NOT_ENOUGH_MEMORY;
		goto Exit;
	}

	// create events
	LocalReqContext->CompletionEvent = CreateEvent(NULL,  // Sec attrib
		FALSE, // Auto reset
		FALSE, // Initial state unsignalled
		NULL); // Name
	if (LocalReqContext->CompletionEvent == NULL)
	{
		Error = GetLastError();
		//LogSysError(Error, L"CreateEvent CompletionEvent");
#ifdef _DEBUG
		printf("Error CreateEvent CompletionEvent\n");
#endif
		goto Exit;
	}

	// create events
	LocalReqContext->CleanUpEvent = CreateEvent(NULL,  // Sec attrib
		FALSE, // Auto reset
		FALSE, // Initial state unsignalled
		NULL); // Name
	if (LocalReqContext->CleanUpEvent == NULL)
	{
		Error = GetLastError();
		//LogSysError(Error, L"CreateEvent CleanUpEvent");
#ifdef _DEBUG
		printf("ErrorCreateEvent CleanUpEvent\n");
#endif
		goto Exit;
	}


	((CoreModifiable*)myConnection)->getValue("Type", connecttype);

	if (connecttype == 1)
	{
		isSecure = true;
	}

	Error = CreateWininetHandles(LocalReqContext,
		SessionHandle,
		isSecure);

	if (Error != ERROR_SUCCESS)
	{
#ifdef _DEBUG
		fprintf(stderr, "CreateWininetHandles failed with %d\n", Error);
#endif
		goto Exit;
	}


	*ReqContext = LocalReqContext;

Exit:

	if (Error != ERROR_SUCCESS)
	{
		CleanUpRequestContext(LocalReqContext);
	}

	return Error;
}


DWORD
HTTPAsyncRequestWindows::CreateWininetHandles(
 REQUEST_CONTEXT* ReqContext,
 HINTERNET SessionHandle,
 bool IsSecureConnection
)
/*++

Routine Description:
Create connect and request handles

Arguments:
ReqContext - Pointer to Request context structure
SessionHandle - Wininet session handle used to create
connect handle
HostName - Hostname to connect
Resource - Resource to get/post
IsSecureConnection - SSL?

Return Value:
Error Code for the operation.

--*/
{
	DWORD Error = ERROR_SUCCESS;
	
	DWORD RequestFlags = myFlags;
	kstl::string L_version = (myVersion) ? "HTTP/1.0" : "HTTP/1.1";
	//
	// Set the correct server port if using SSL
	// Also set the flag for HttpOpenRequest 
	//

	if (IsSecureConnection)
	{
		RequestFlags |= INTERNET_FLAG_SECURE;
	}

	// Create Connection handle and provide context for async operations
	ReqContext->ConnectHandle = ((HTTPConnectWindows*)(CoreModifiable*)myConnection)->getHandle();
	LPTSTR AcceptTypes[2] = { (LPTSTR)TEXT("*/*"), NULL };

	// used during the callbacks
	//                                                                        
	// For HTTP InternetConnect returns synchronously because it does not
	// actually make the connection.
	//
	// For FTP InternetConnect connects the control channel, and therefore
	// can be completed asynchronously.  This sample would have to be
	// changed, so that the InternetConnect's asynchronous completion
	// is handled correctly to support FTP.
	//

	if (ReqContext->ConnectHandle == NULL)
	{
		Error = GetLastError();
		//LogInetError(Error, L"InternetConnect");
#ifdef _DEBUG
		printf("Error InternetConnect\n");
#endif
		goto Exit;
	}


	//
	// We're overriding WinInet's default behavior.
	// Setting these flags, we make sure we get the response from the server and not the cache.
	// Also ask WinInet not to store the response in the cache.
	//
	// These flags are NOT performant and are only used to show case WinInet's Async I/O.
	// A real WinInet application would not want to use this flags.
	//

	
	
	// Create a Request handle
	ReqContext->RequestHandle = HttpOpenRequestA(ReqContext->ConnectHandle,
		((const kstl::string&)myRequestType).c_str(),                     // GET, POST, PUT, DELETE
		myRequestURL.c_str(),                 // root "/" by default
		L_version.c_str(),                     // Use default HTTP/1.1 as the version
		NULL,                     // Do not provide any referrer
		(LPCTSTR*)AcceptTypes,                     // Do not provide Accept types
		RequestFlags,
		(DWORD_PTR)ReqContext);



	if (ReqContext->RequestHandle == NULL)
	{
		Error = GetLastError();

#ifdef _DEBUG
		printf("Error HttpOpenRequest\n");
#endif
		goto Exit;
	}


Exit:

	return Error;
}


 VOID	 HTTPAsyncRequestWindows::CleanUpRequestContext(
	REQUEST_CONTEXT* ReqContext
	 )
	 /*++

	 Routine Description:
	 Used to cleanup the request context before exiting.

	 Arguments:
	 ReqContext - Pointer to request context structure

	 Return Value:
	 None.

	 --*/
 {
	 if (ReqContext == NULL)
	 {
		 goto Exit;
	 }

	 if (ReqContext->RequestHandle)
	 {
		 CloseRequestHandle(ReqContext);

		 //
		 // Wait for the closing of the handle to complete
		 // (waiting for all async operations to complete)
		 //
		 // This is the only safe way to get rid of the context
		 //

		 (VOID)WaitForSingleObject(ReqContext->CleanUpEvent, INFINITE);
	 }

	/* if (ReqContext->ConnectHandle)
	 {
		 //
		 // Remove the callback from the ConnectHandle since
		 // we don't want the closing notification
		 // The callback was inherited from the session handle
		 //
		 (VOID)InternetSetStatusCallback(ReqContext->ConnectHandle,
			 NULL);

		 (VOID)InternetCloseHandle(ReqContext->ConnectHandle);
	 }
	 */
	 if (ReqContext->CompletionEvent)
	 {
		 CloseHandle(ReqContext->CompletionEvent);
	 }

	 if (ReqContext->CleanUpEvent)
	 {
		 CloseHandle(ReqContext->CleanUpEvent);
	 }

	 if (ReqContext->CritSecInitialized)
	 {
		 DeleteCriticalSection(&ReqContext->CriticalSection);
	 }

	 if (ReqContext->OutputBuffer)
	 {
		 free(ReqContext->OutputBuffer);
	 }

	 free(ReqContext);


 Exit:

	 return;
 }

