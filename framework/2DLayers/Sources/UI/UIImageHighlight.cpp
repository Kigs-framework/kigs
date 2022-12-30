#include "UI/UIImageHighlight.h"

using namespace Kigs::Draw2D;

IMPLEMENT_CLASS_INFO(UIImageHighlight)

void UIImageHighlight::InitModifiable()
{
	ParentClassType::InitModifiable();
	if (_isInit)
	{
		mPLTexturePointer = mTexturePointer;

		if (mHLTexture.const_ref() != "")
		{
			mHLTexturePointer = KigsCore::GetInstanceOf(mHLTexture, "TextureHandler");
			mHLTexturePointer->setValue("TextureName", mHLTexture);
			mHLTexturePointer->Init();
		}

		mHLTexture.changeNotificationLevel(Owner);
	}
}


void UIImageHighlight::NotifyUpdate(const unsigned int labelid)
{
	if (labelid == mHLTexture.getLabelID())
	{
		ChangeTexture();
	}
	else
	{
		ParentClassType::NotifyUpdate(labelid);
	}
}


void UIImageHighlight::ChangeTexture()
{
	ParentClassType::ChangeTexture();

	std::string hltext = mHLTexture;

	if (hltext == "")
	{
		mHLTexturePointer = nullptr;
		return;
	}
	if (mHLTexturePointer)
	{
		if (mHLTexturePointer->getValue<std::string>("TextureName") == hltext) // nothing changed
		{
			return;
		}
	}

	mHLTexturePointer = KigsCore::GetInstanceOf(hltext, "TextureHandler");
	mHLTexturePointer->setValue("TextureName", hltext);
	mHLTexturePointer->Init();
}

void UIImageHighlight::Highlight(bool b)
{
	if (b)
		mTexturePointer = mPLTexturePointer;
	else
		mTexturePointer = mHLTexturePointer;
}