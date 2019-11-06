#include "UI/UITexture.h"
#include "TextureFileManager.h"
#include "Texture.h"
#include "AlphaMask.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UITexture, UITexture, 2DLayers);
IMPLEMENT_CLASS_INFO(UITexture)

UITexture::UITexture(const kstl::string& name, CLASS_NAME_TREE_ARG) :
UITexturedItem(name, PASS_CLASS_NAME_TREE_ARG)
{

}

UITexture::~UITexture()
{
}

void UITexture::PreDraw(TravState* state)
{
	UITexturedItem::PreDraw(state);

	if (myTexture)
	{
		Point2D texsize;
		myTexture->GetSize(texsize.x, texsize.y);
		
		if ((texsize.x > 0.0f) && (texsize.y > 0.0f))
		{
			mySizeX = texsize.x;
			mySizeY = texsize.y;
		}
		else
		{
			mySizeX = 0.0f;
			mySizeY = 0.0f;
		}
		ComputeRealSize();
	}
}