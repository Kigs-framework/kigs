#include "UI/UIImageHighlight.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIImageHighlight, UIImageHighlight, 2DLayers);
IMPLEMENT_CLASS_INFO(UIImageHighlight)

void UIImageHighlight::ChangeTexture()
{
	ParentClassType::ChangeTexture();

	auto& textureManager = KigsCore::Singleton<TextureFileManager>();
	if (mTexture.const_ref() != "")
	{
		mPLTexturePointer = textureManager->GetTexture(mTexture);
	}

	if (myHLTexture.const_ref() != "")
	{
		mHLTexturePointer = textureManager->GetTexture(myHLTexture);
	}
}

void UIImageHighlight::Highlight(bool b)
{
	if (b)
		mTexturePointer = mPLTexturePointer;
	else
		mTexturePointer = mHLTexturePointer;
}