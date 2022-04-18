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
	//! destructor
    virtual ~HTTPConnectJS();
	virtual void InitModifiable() override;


	virtual bool	Open() override;
	virtual void	Close() override;

protected:


};

#endif //_HTTPCONNECTJS_H_
