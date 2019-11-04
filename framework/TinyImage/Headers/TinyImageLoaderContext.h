#ifndef _TINYIMAGELOADERCONTEXT_H_
#define _TINYIMAGELOADERCONTEXT_H_

#include "TinyImage.h"


class TinyImageLoaderContext;
// default context has to be defined by platform
extern	TinyImageLoaderContext	gDefaultContext;

class TinyImageLoaderContext 
{
public:	

	static TinyImageLoaderContext&	GetDefault()
	{
		return gDefaultContext;
	}

	TinyImage::ImageFormat	myFormatConvertion[TinyImage::SUPPORTED_FORMAT_COUNT];

};


#endif //_TINYIMAGELOADERCONTEXT_H_

