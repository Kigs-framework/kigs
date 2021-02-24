#include "UI/UIImage.h"

#include "AlphaMask.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIImage, UIImage, 2DLayers);
IMPLEMENT_CLASS_INFO(UIImage)

UIImage::UIImage(const kstl::string& name, CLASS_NAME_TREE_ARG) :
	UITexturedItem(name, PASS_CLASS_NAME_TREE_ARG)
	//, mTextureName(*this, false, LABEL_AND_ID(TextureName), "")
{
	mCurrentTextureName = "";
}
/*
void UIImage::NotifyUpdate(const unsigned int labelid)
{
	if (labelid == mTextureName.getLabelID())
	{
		ChangeTexture();
	}
	UITexturedItem::NotifyUpdate(labelid);
}*/

void UIImage::InitModifiable()
{
	UITexturedItem::InitModifiable();
	if (IsInit())
	{
		// load texture
		//ChangeTexture();

		// auto size
		if ((int)mDisplayMode == AUTO)
		{
			if ((mSizeX.const_ref() == 0) && (mSizeY.const_ref() == 0))
				mDisplayMode.setValue(RESIZE);
			else
				mDisplayMode.setValue(SCALE);
		}

		//mIsEnabled.changeNotificationLevel(Owner);
		//mTextureName.changeNotificationLevel(Owner);


		ComputeRealSize();
	}
}

void UIImage::ComputeRealSize()
{
	switch ((int)mDisplayMode)
	{
	case RESIZE:
		if (!mTexturePointer.isNil()) mTexturePointer->GetSize(mRealSize.x, mRealSize.y);
		break;
	case SCALE:
		UITexturedItem::ComputeRealSize();
		if (mKeepRatio && !mTexturePointer.isNil())
		{
			unsigned int tsX, tsY;
			mTexturePointer->GetSize(tsX, tsY);

			float ratioTex = (float)tsX / (float)tsY;
			float ratioUI = mRealSize.x / mRealSize.y;

			int dirTex = (ratioTex == 1) ? 0 : (ratioTex < 1) ? -1 : 1;
			int dirUI = (ratioUI == 1) ? 0 : (ratioUI < 1) ? -1 : 1;

			if (dirUI == 0 && dirTex == 0) // both square
			{
				// keep realSize as it is
			}
			else if (dirUI == 0) // scale Tex to fit in a square
			{
				if (dirTex < 0) // x smaller than y
				{
					mRealSize.x *= ratioTex;
				}
				else // y smaller than x
				{
					mRealSize.y /= ratioTex;
				}
			}
			else if (dirTex == 0) // scale Tex to fit in a rectangle
			{
				if (dirUI < 0) // x smaller than y
				{
					mRealSize.y = mRealSize.x;
				}
				else // y smaller than x
				{
					mRealSize.x = mRealSize.y;
				}
			}
			else if (dirTex*dirUI < 0) //two rectangle with different dir
			{
				if (dirTex < 0) // x smaller than y
				{
					mRealSize.x = mRealSize.y*ratioTex;
				}
				else // y smaller than x
				{
					mRealSize.y = mRealSize.x/ratioTex;
				}
			}
			else // two rectangle in the same direction
			{
				if (dirTex < 0)
				{
					if(mRealSize.y*ratioTex<mRealSize.x)
						mRealSize.x = mRealSize.y*ratioTex;
					else
						mRealSize.y = mRealSize.x / ratioTex;
				}
				else
				{
					if (mRealSize.x/ratioTex<mRealSize.y)
						mRealSize.y = mRealSize.x / ratioTex;
					else
						mRealSize.x = mRealSize.y*ratioTex;
				}
			}

		}
		break;
	default:
		if (!mTexturePointer.isNil())mTexturePointer->GetSize(mRealSize.x, mRealSize.y);
		kigsprintf("Display Mode not supported, use resize\n");
		break;
	}
}

void UIImage::ChangeTexture()
{
	// no change
	/*if (mCurrentTextureName == mTextureName.const_ref())
		return;

	mTextureName.getValue(mCurrentTextureName);

	// empty texture name >> destroy texture
	if (mCurrentTextureName == "")
	{
		mTexturePointer = nullptr;
		return;
	}

	mTexturePointer = KigsCore::GetInstanceOf(mCurrentTextureName, "TextureHandler");
	mTexturePointer->setValue("TextureName", mCurrentTextureName);
	mTexturePointer->Init();*/
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
