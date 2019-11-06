#pragma once

#include "HTTPConnect.h"

class HTTPConnectWUP : public HTTPConnect
{
public:
	DECLARE_CLASS_INFO(HTTPConnectWUP, HTTPConnect, HTTPRequestModule)
	DECLARE_INLINE_CONSTRUCTOR(HTTPConnectWUP) {}
	
	virtual void InitModifiable();

	virtual bool	Open();
	virtual void	Close();

protected:

	//! destructor
    virtual ~HTTPConnectWUP();
};


