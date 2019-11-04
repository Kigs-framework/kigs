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
* \ingroup KigsCore
* \brief	base class manage asynchronous request
* \author	ukn
* \version ukn
* \date	ukn
*
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
		return myProcessingFlag & 1;
	}

	void	setDone()
	{
		myProcessingFlag |= 1;
	}

	void Process();

	bool isProcessed()
	{
		return ((myProcessingFlag & 3) == 3); // done and processed
	}

	bool isTimedOut()
	{
		return ((myProcessingFlag & 4)!=0);
	}

	void setTimedOut()
	{
		myProcessingFlag |= 5; // timed out and done flag
	}

	bool isCancelled()
	{
		return ((myProcessingFlag & 8)!=0);
	}

	void setCancelled()
	{
		myProcessingFlag |= 9;
	}

protected:

	virtual void	protectedProcess() = 0;

	void setProcessed()
	{
		myProcessingFlag |= 2;
	}

	volatile unsigned int myProcessingFlag;
};

#endif //_ASYNCREQUEST_H_
