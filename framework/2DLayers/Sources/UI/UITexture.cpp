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

	if (mTexturePointer)
	{
		Point2D texsize;
		mTexturePointer->GetSize(texsize.x, texsize.y);
		
		if ((texsize.x > 0.0f) && (texsize.y > 0.0f))
		{
			mSizeX = texsize.x;
			mSizeY = texsize.y;
		}
		else
		{
			mSizeX = 0.0f;
			mSizeY = 0.0f;
		}

		Point2D prevRealSize = mRealSize;

		ComputeRealSize();

		if (mRealSize != prevRealSize)
		{
			mNeedUpdatePosition = true;
		}
	}
}