#include "PrecompiledHeaders.h"
#include "AsyncRequest.h"
#include "Core.h"

//! a do nothing object is just an instance of CoreModifiable with empty methods
IMPLEMENT_CLASS_INFO(AsyncRequest)

//! constructor
AsyncRequest::AsyncRequest(const std::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
, mProcessingFlag(0)
{
	
}


void AsyncRequest::Process()
{
#ifdef _DEBUG
	if (!isDone()) // cannot process a request if it's not done
	{
		KIGS_ERROR("trying to process a not already done request", 2);
	}
#endif
	// don't process if cancelled or timed out
	if ((!isCancelled()) && (!isTimedOut()))
	{
		protectedProcess();
	}
	setProcessed();

}