#ifndef _UIImageHighlight_H_
#define _UIImageHighlight_H_

#include "UIImage.h"
#include "TextureFileManager.h"
#include "AttributePacking.h"

class UIImageHighlight : public UIImage
{
public:
	DECLARE_CLASS_INFO(UIImageHighlight, UIImage, 2DLayers);
	DECLARE_INLINE_CONSTRUCTOR(UIImageHighlight) {}

	void ChangeTexture() override;
	void Highlight(bool b);

protected:
	WRAP_METHODS(Highlight);

	SmartPointer<Texture>				myPLTexture;
	SmartPointer<Texture>				myHLTexture;

	maString	myHLTextureName = BASE_ATTRIBUTE(HLTexture);
};

#endif //_UIImageHighlight_H_