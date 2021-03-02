#include "UI/UIImage.h"

#include "AlphaMask.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIImage, UIImage, 2DLayers);
IMPLEMENT_CLASS_INFO(UIImage)

UIImage::UIImage(const kstl::string& name, CLASS_NAME_TREE_ARG) :
	UITexturedItem(name, PASS_CLASS_NAME_TREE_ARG)
{

}

void UIImage::InitModifiable()
{
	UITexturedItem::InitModifiable();
	if (IsInit())
	{
		if ((mSizeModeX == DEFAULT) && (mSizeX.const_ref() == 0))
		{
			mSizeModeX.setValue(CONTENT);
		}

		if ((mSizeModeY == DEFAULT) && (mSizeY.const_ref() == 0))
		{
			mSizeModeY.setValue(CONTENT);
		}
	}
}

void UIImage::ChangeTexture()
{
	if (mTexturePointer)
	{
		mTexturePointer->changeTexture();
	}
}

bool UIImage::isAlpha(float X, float Y)
{
	//Try to get mask
	if (!mAlphaMask)
	{
		kstl::vector<ModifiableItemStruct> sons = getItems();

		for (unsigned int i = 0; i < sons.size(); i++)
		{
			if (sons[i].mItem->isSubType("AlphaMask"))
			{
				mAlphaMask = sons[i].mItem;
				break;
			}
		}
	}

	if (mAlphaMask)
	{
		return !mAlphaMask->CheckTo(X, Y);
	}

	return false;
}
