#include "UI/UIButtonImage.h"
#include "TextureFileManager.h"
#include "Texture.h"
#include "NotificationCenter.h"
#include "AlphaMask.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIButtonImage, UIButtonImage, 2DLayers);;
IMPLEMENT_CLASS_INFO(UIButtonImage)

UIButtonImage::UIButtonImage(const kstl::string& name,CLASS_NAME_TREE_ARG) : 
UIButton(name, PASS_CLASS_NAME_TREE_ARG)
,myUpTextureName(*this,false,LABEL_AND_ID(UpTexture),"")
,myDownTextureName(*this,false,LABEL_AND_ID(DownTexture),"")
,myOverTextureName(*this,false,LABEL_AND_ID(OverTexture),"")
{
}

UIButtonImage::~UIButtonImage()
{
	myUpTexture = NULL;
	myOverTexture = NULL;
	myDownTexture = NULL;
}

void UIButtonImage::InitModifiable()
{
	UIButton::InitModifiable();
	if(_isInit)
	{
		// load texture
		TextureFileManager* textureManager = (TextureFileManager*)KigsCore::GetSingleton("TextureFileManager");

		myUpTexture=0;
		myOverTexture = 0;
		myDownTexture = 0;
		
		if(myUpTextureName.const_ref() !="")
		{
			myUpTexture = textureManager->GetTextureManaged(myUpTextureName.const_ref());
		}

		if (myOverTextureName.const_ref() != "")
		{
			myOverTexture = textureManager->GetTextureManaged(myOverTextureName.const_ref());
		}

		if (myDownTextureName.const_ref() != "")
		{
			myDownTexture = textureManager->GetTextureManaged(myDownTextureName.const_ref());
		}

		myTexture = myUpTexture;

		AutoSize();


		myUpTextureName.changeNotificationLevel(Owner);
		myDownTextureName.changeNotificationLevel(Owner);
		myOverTextureName.changeNotificationLevel(Owner);
	}

}

void UIButtonImage::NotifyUpdate(const unsigned int labelid)
{
	if (labelid == myUpTextureName.getLabelID() ||
		labelid == myDownTextureName.getLabelID() ||
		labelid == myOverTextureName.getLabelID())
	{
		ChangeTexture(myUpTextureName.const_ref(), myOverTextureName.const_ref(), myDownTextureName.const_ref());
	}
	else
		UITexturedItem::NotifyUpdate(labelid);
}

void UIButtonImage::ChangeTexture(kstl::string _texturename, kstl::string _overtexturename, kstl::string _downtexturename)
{
	TextureFileManager* textureManager = (TextureFileManager*)KigsCore::GetSingleton("TextureFileManager");
	if (myUpTexture)
		myUpTexture = 0;

	if(_texturename != "")
	{
		myUpTextureName = _texturename;
		myUpTexture = textureManager->GetTextureManaged(myUpTextureName);
	}

	if(myOverTexture && _overtexturename != "")
		myOverTexture=0;

	if(_overtexturename != "")
	{
		myOverTextureName = _overtexturename;
		myOverTexture = textureManager->GetTextureManaged(myOverTextureName);
	}

	if(myDownTexture && _downtexturename != "")
		myDownTexture=0;

	if(_downtexturename != "")
	{
		myDownTextureName = _downtexturename;
		myDownTexture = textureManager->GetTextureManaged(myDownTextureName);
	}

	if(myAlphaMask)
	{
		this->removeItem(myAlphaMask);

		//Make new Mask
		myAlphaMask = (AlphaMask*)KigsCore::GetInstanceOf(getName(), "AlphaMask");
		myAlphaMask->setValue(LABEL_TO_ID(Threshold),0.1);
		myAlphaMask->setValue(LABEL_TO_ID(TextureName),myUpTextureName.c_str());
		this->addItem(myAlphaMask);
		myAlphaMask->Init();
		myAlphaMask->Destroy();
	}
	//auto Size
	if (myUpTexture && myAutoResize)
	{
		float width,height;

		myUpTexture->GetSize(width,height);
		if(width != mySizeX || height != mySizeY)
		{
			mySizeX=width;
			mySizeY=height;
			myNeedUpdatePosition=true;
		}
	}

	ChangeState();
}


//-----------------------------------------------------------------------------------------------------
//ReloadTexture

void	UIButtonImage::ReloadTexture()
{
	if (myUpTexture)
	{
		myUpTexture->ReInit();
		myTexture = myUpTexture;
	}

	if (myOverTexture)
	{
		myOverTexture->ReInit();
	}

	if (myDownTexture)
	{
		myDownTexture->ReInit();
	}
}

void UIButtonImage::ChangeState()
{
	if (myIsEnabled) // down and mouse over only when enabled
	{
		if (myIsDown && myDownTexture)
		{
			myTexture = myDownTexture;
			return;
		}
		else if (myIsMouseOver && myOverTexture)
		{
			myTexture = myOverTexture;
			return;
		}
	}
	myTexture = myUpTexture;
}
