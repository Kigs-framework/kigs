#ifndef _ALPHAMASK_H_
#define _ALPHAMASK_H_

#include "CoreModifiableAttribute.h"
#include "CoreModifiable.h"
#include <stdlib.h>

// ****************************************
// * AlphaMask class
// * --------------------------------------
/**
* \file	AlphaMask.h
* \class	AlphaMask
* \ingroup 2DLayers
* \brief	Picture mask for touch management.
*
* Obsolete ?
*/
// ****************************************
class AlphaMask : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(AlphaMask,CoreModifiable,2DLayers)
	DECLARE_INLINE_CONSTRUCTOR(AlphaMask) {}

	
	virtual bool CheckTo(float x, float y);
	
protected:
	
	void InitModifiable() override;

	void CreateMask(u8* pixelData, int stride, int pixel_size);
	
	maFloat					mThreshold = BASE_ATTRIBUTE(Threshold, 0.0f);
	maString				mTextureName = BASE_ATTRIBUTE(TextureName, "");


	std::vector<u8> mTab;

	v2i mSize;
};
#endif //_ALPHAMASK_H_
