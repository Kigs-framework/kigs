#ifndef _UIImageHighlight_H_
#define _UIImageHighlight_H_

#include "UIImage.h"
#include "TextureFileManager.h"
#include "AttributePacking.h"

// ****************************************
// * UIImageHighlight class
// * --------------------------------------
/**
* \file	UIImageHighlight.h
* \class	UIImageHighlight
* \ingroup 2DLayers
* \brief	???
*/
// ****************************************

class UIImageHighlight : public UIImage
{
public:
	DECLARE_CLASS_INFO(UIImageHighlight, UIImage, 2DLayers);
	DECLARE_INLINE_CONSTRUCTOR(UIImageHighlight) {}

	void ChangeTexture() override;
	void Highlight(bool b);

protected:
	WRAP_METHODS(Highlight);

	SmartPointer<Texture>				mPLTexturePointer;
	SmartPointer<Texture>				mHLTexturePointer;

	maString	myHLTexture = BASE_ATTRIBUTE(HLTexture);
};

#endif //_UIImageHighlight_H_