#ifndef _TINYIMAGELOADERCONTEXT_H_
#define _TINYIMAGELOADERCONTEXT_H_

#include "TinyImage.h"


class TinyImageLoaderContext;
// default context has to be defined by platform
extern	TinyImageLoaderContext	gDefaultContext;

// ****************************************
// * TinyImageLoaderContext class
// * --------------------------------------
/**
* \file	TinyImageLoaderContext.h
* \class	TinyImageLoaderContext
* \ingroup TinyImageModule
* \brief Support conversion depending on the context ( ie used to import in supported texture format for given platform )
*
*/
// ****************************************
class TinyImageLoaderContext 
{
public:	

	static TinyImageLoaderContext&	GetDefault()
	{
		return gDefaultContext;
	}

	TinyImage::ImageFormat	mFormatConvertion[TinyImage::SUPPORTED_FORMAT_COUNT];

};


#endif //_TINYIMAGELOADERCONTEXT_H_

