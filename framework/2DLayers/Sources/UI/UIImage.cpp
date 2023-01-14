#include "UI/UIImage.h"

#include "AlphaMask.h"

using namespace Kigs::Draw2D;

IMPLEMENT_CLASS_INFO(UIImage)

UIImage::UIImage(const std::string& name, CLASS_NAME_TREE_ARG) :
	UITexturedItem(name, PASS_CLASS_NAME_TREE_ARG)
{

}

void UIImage::InitModifiable()
{
	UITexturedItem::InitModifiable();
	if (IsInit())
	{
		if ((mSizeModeX == DEFAULT) && (mSize[0] == 0))
		{
			mSizeModeX.setValue(CONTENT,this);
		}

		if ((mSizeModeY == DEFAULT) && (mSize[1] == 0))
		{
			mSizeModeY.setValue(CONTENT, this);
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
		std::vector<ModifiableItemStruct> sons = getItems();

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
