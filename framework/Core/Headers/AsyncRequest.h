#ifndef _ASYNCREQUEST_H_
#define _ASYNCREQUEST_H_

#include "CoreModifiable.h"
#include "Core.h"

// ****************************************
// * AsyncRequest class
// * --------------------------------------
/**
* \file	AsyncRequest.h
* \class	AsyncRequest
* \ingroup Core
* \brief	Manage asynchronous request. 
*
* Base class for HTTP request.
*/
// ****************************************
class AsyncRequest : public CoreModifiable
{
public:

	// instanciable class derived from CoreModifiable
	DECLARE_ABSTRACT_CLASS_INFO(AsyncRequest, CoreModifiable, KigsCore)

	/**
	* \brief	constructor
	* \fn 		AsyncRequest(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	* \param	name : instance name
	* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	*/
	AsyncRequest(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	/**
	* \fn 		virtual ~AsyncRequest(){};
	* \brief	destructor
	*
	* Do nothing
	*/
	virtual ~AsyncRequest(){};

	bool	isDone()
	{
		return mProcessingFlag & 1;
	}

	void	setDone()
	{
		mProcessingFlag |= 1;
	}

	void Process();

	bool isProcessed()
	{
		return ((mProcessingFlag & 3) == 3); // done and processed
	}

	bool isTimedOut()
	{
		return ((mProcessingFlag & 4)!=0);
	}

	void setTimedOut()
	{
		mProcessingFlag |= 5; // timed out and done flag
	}

	bool isCancelled()
	{
		return ((mProcessingFlag & 8)!=0);
	}

	void setCancelled()
	{
		mProcessingFlag |= 9;
	}

protected:

	virtual void	protectedProcess() = 0;

	void setProcessed()
	{
		mProcessingFlag |= 2;
	}

	volatile unsigned int mProcessingFlag;
};

#endif //_ASYNCREQUEST_H_
