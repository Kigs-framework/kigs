#ifndef _HTTPCONNECTJS_H_
#define _HTTPCONNECTJS_H_

#include "HTTPConnect.h"
// ****************************************
// * HTTPConnectJS class
// * --------------------------------------
/*!  \class HTTPConnectJS
     manage a javascript html connection
*/
// ****************************************

class HTTPConnectJS : public HTTPConnect
{
public:

	DECLARE_CLASS_INFO(HTTPConnectJS, HTTPConnect, HTTPRequestModule)

	//! constructor
    HTTPConnectJS(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	virtual void InitModifiable();


	virtual bool	Open();
	virtual void	Close();

protected:

	//! destructor
    virtual ~HTTPConnectJS();
};

#endif //_HTTPCONNECTJS_H_
