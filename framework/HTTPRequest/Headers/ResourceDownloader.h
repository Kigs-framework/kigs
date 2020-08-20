#pragma once

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "HTTPConnect.h"

// ****************************************
// * ResourceDownloader class
// * --------------------------------------
/**
* \file	ResourceDownloader.h
* \class	ResourceDownloader
* \ingroup HTTPRequest
* \brief Download file from given URL.
*
*/
// ****************************************

class ResourceDownloader : public CoreModifiable
{
public:

	DECLARE_CLASS_INFO(ResourceDownloader, CoreModifiable,HTTPRequest)
	DECLARE_INLINE_CONSTRUCTOR(ResourceDownloader) {}
	SIGNALS(onDownloadDone);

protected:

	void InitModifiable() override;

	DECLARE_METHOD(DownloadDone);

	COREMODIFIABLE_METHODS(DownloadDone);


	maString	mURL = BASE_ATTRIBUTE(URL, "");

	SP<HTTPConnect>			mConnect = nullptr;
	SP<HTTPAsyncRequest>	mAnswer = nullptr;

};