#include "UI/UIImageHighlight.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIImageHighlight, UIImageHighlight, 2DLayers);
IMPLEMENT_CLASS_INFO(UIImageHighlight)

void UIImageHighlight::ChangeTexture()
{
	ParentClassType::ChangeTexture();

	SP<TextureFileManager> textureManager = KigsCore::GetSingleton("TextureFileManager");
	if (myTextureName.const_ref() != "")
	{
		myPLTexture = textureManager->GetTexture(myTextureName);
	}

	if (myHLTextureName.const_ref() != "")
	{
		myHLTexture = textureManager->GetTexture(myHLTextureName);
	}
}

void UIImageHighlight::Highlight(bool b)
{
	if (b)
		myTexture = myPLTexture;
	else
		myTexture = myHLTexture;
}