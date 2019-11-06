#include "UI/UIImageHighlight.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIImageHighlight, UIImageHighlight, 2DLayers);
IMPLEMENT_CLASS_INFO(UIImageHighlight)

void UIImageHighlight::ChangeTexture()
{
	ParentClassType::ChangeTexture();

	TextureFileManager* textureManager = (TextureFileManager*)KigsCore::GetSingleton("TextureFileManager");
	if (myTextureName.const_ref() != "")
	{
		myPLTexture = textureManager->GetTextureManaged(myTextureName);
	}

	if (myHLTextureName.const_ref() != "")
	{
		myHLTexture = textureManager->GetTextureManaged(myHLTextureName);
	}
}

void UIImageHighlight::Highlight(bool b)
{
	if (b)
		myTexture = myPLTexture;
	else
		myTexture = myHLTexture;
}