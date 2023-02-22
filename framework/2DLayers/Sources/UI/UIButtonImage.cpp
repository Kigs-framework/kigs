#include "UI/UIButtonImage.h"
#include "TextureFileManager.h"
#include "Texture.h"
#include "NotificationCenter.h"
#include "AlphaMask.h"

using namespace Kigs::Draw2D;

IMPLEMENT_CLASS_INFO(UIButtonImage)

UIButtonImage::UIButtonImage(const std::string& name,CLASS_NAME_TREE_ARG) : 
UIButton(name, PASS_CLASS_NAME_TREE_ARG)
{
}

UIButtonImage::~UIButtonImage()
{
	mUpTexturePointer = NULL;
	mOverTexturePointer = NULL;
	mDownTexturePointer = NULL;
}

void UIButtonImage::InitModifiable()
{
	UIButton::InitModifiable();
	if(_isInit)
	{
		// load texture
		mUpTexturePointer =0;
		mOverTexturePointer = 0;
		mDownTexturePointer = 0;
		
		if(mUpTexture !="")
		{
			mUpTexturePointer = KigsCore::GetInstanceOf(mUpTexture,"TextureHandler");
			mUpTexturePointer->setValue("TextureName", mUpTexture);
			mUpTexturePointer->Init();
		}

		if (mOverTexture != "")
		{
			mOverTexturePointer = KigsCore::GetInstanceOf(mOverTexture, "TextureHandler");
			mOverTexturePointer->setValue("TextureName", mOverTexture);
			mOverTexturePointer->Init();
		}

		if (mDownTexture != "")
		{
			mDownTexturePointer = KigsCore::GetInstanceOf(mDownTexture, "TextureHandler");
			mDownTexturePointer->setValue("TextureName", mDownTexture);
			mDownTexturePointer->Init();
		}

		mTexturePointer = mUpTexturePointer;

		AutoSize();

		setOwnerNotification("UpTexture", true);
		setOwnerNotification("DownTexture", true);
		setOwnerNotification("OverTexture", true);
	}

}

void UIButtonImage::NotifyUpdate(const unsigned int labelid)
{
	if (labelid == KigsID("UpTexture")._id  ||
		labelid == KigsID("DownTexture")._id ||
		labelid == KigsID("OverTexture")._id)
	{
		ChangeTexture(mUpTexture, mOverTexture, mDownTexture);
	}
	else
		UITexturedItem::NotifyUpdate(labelid);
}

void UIButtonImage::ChangeTexture(std::string _texturename, std::string _overtexturename, std::string _downtexturename)
{
	
	if(_texturename != "")
	{
		mUpTexture = _texturename;
		mUpTexturePointer->setValue("TextureName", _texturename);
	}

	if(_overtexturename != "")
	{
		mOverTexture = _overtexturename;
		mOverTexturePointer->setValue("TextureName", _overtexturename);
	}

	if(_downtexturename != "")
	{
		mDownTexture = _downtexturename;
		mDownTexturePointer->setValue("TextureName", _downtexturename);
	}

	if(mAlphaMask)
	{
		this->removeItem(mAlphaMask);

		//Make new Mask
		mAlphaMask = KigsCore::GetInstanceOf(getName(), "AlphaMask");
		mAlphaMask->setValue("Threshold",0.1);
		mAlphaMask->setValue("TextureName",mUpTexture);
		this->addItem(mAlphaMask);
		mAlphaMask->Init();
	}
	//auto Size
	if (mUpTexturePointer && mAutoResize)
	{
		float width,height;

		mUpTexturePointer->GetSize(width,height);
		if(width != mSize[0] || height != mSize[1])
		{
			mSize=v2f(width, height);
			SetNodeFlag(Node2D_SizeChanged);
		}
	}

	ChangeState();
}


//-----------------------------------------------------------------------------------------------------
//ReloadTexture

void	UIButtonImage::ReloadTexture()
{
	if (mUpTexturePointer)
	{
		mUpTexturePointer->ReInit();
		mTexturePointer = mUpTexturePointer;
	}

	if (mOverTexturePointer)
	{
		mOverTexturePointer->ReInit();
	}

	if (mDownTexturePointer)
	{
		mDownTexturePointer->ReInit();
	}
}

void UIButtonImage::ChangeState()
{
	if (mIsEnabled) // down and mouse over only when enabled
	{
		if (mIsDown && mDownTexturePointer)
		{
			mTexturePointer = mDownTexturePointer;
			return;
		}
		else if (mIsMouseOver && mOverTexturePointer)
		{
			mTexturePointer = mOverTexturePointer;
			return;
		}
	}
	mTexturePointer = mUpTexturePointer;
}
