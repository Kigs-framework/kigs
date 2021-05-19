#include "UI/UISlider.h"

#include "TextureFileManager.h"
#include "Texture.h"
#include "NotificationCenter.h"
#include "AlphaMask.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UISlider, UISlider, 2DLayers);
IMPLEMENT_CLASS_INFO(UISlider)

UISlider::UISlider(const kstl::string& name,CLASS_NAME_TREE_ARG) : 
UIDrawableItem(name, PASS_CLASS_NAME_TREE_ARG),
mUpTexture(*this,false,LABEL_AND_ID(UpTexture),""),
mDownTexture(*this,false,LABEL_AND_ID(DownTexture),""),
mOverTexture(*this,false,LABEL_AND_ID(OverTexture),""),
mClickUpAction(*this, false, LABEL_AND_ID(ClickUpAction), ""),
mParameter(*this, false, LABEL_AND_ID(Parameter), kstl::string("")),
mMoveAction(*this,false,LABEL_AND_ID(MoveAction),""),
mInitialRatio(*this, true, LABEL_AND_ID(InitialRatio), 0),
mDirection(*this,true,LABEL_AND_ID(Direction),"Vertical","Horizontal")
{
	
}

UISlider::~UISlider()
{
	if(mUpTexturePointer)
		mUpTexturePointer = NULL;
	if(mOverTexturePointer)
		mOverTexturePointer = NULL;
	if(mDownTexturePointer)
		mDownTexturePointer = NULL;
}

//-----------------------------------------------------------------------------------------------------
//ReloadTexture

void	UISlider::ReloadTexture()
{
	if(mUpTexturePointer)
	{
		mUpTexturePointer->ReInit();
	}

	if(mDownTexturePointer)
	{
		mDownTexturePointer->ReInit();
	}

	if(mOverTexturePointer)
	{
		mOverTexturePointer->ReInit();
	}
}

void UISlider::NotifyUpdate(const unsigned int labelid )
{
	if(labelid==mIsEnabled.getLabelID()) 
	{
		// if disabled "reset" button state
		if(mIsEnabled == false)
		{
			mIsDown=false;
			mIsMouseOver=false;
		}
	}
	UIDrawableItem::NotifyUpdate(labelid);
}

void UISlider::InitModifiable()
{
	CoreModifiable::InitModifiable();

	if(_isInit)
	{
		kstl::vector<CoreModifiable*> parents = GetParents();

		if(parents.size()>0 )
		{
			if(parents[0]->isSubType("UISliderFill"))
			{
				if(parents[0]->IsInit())
				{
					if(parents[0]->isSubType(UIItem::mClassID))
					{
						mParent = (UIItem*)parents[0];
					}
				}
				else
				{
					UninitModifiable();
				}
			}
			else
			{
				if(parents[0]->isSubType(UIItem::mClassID))
				{
					mParent = (UIItem*)parents[0];
				}
			}
		}
		else
		{
			UninitModifiable();
		}
	}

	if(_isInit)
	{
		mIsDown      = false;
		mIsMouseOver = false;

		// load texture
		auto textureManager = KigsCore::Singleton<TextureFileManager>();
		if(mUpTexturePointer)
			mUpTexturePointer=0;
		
		mUpTexturePointer = textureManager->GetTexture(mUpTexture);

		if(mOverTexturePointer)
			mOverTexturePointer=0;

		if(mOverTexture.const_ref() !="")
		{
			mOverTexturePointer = textureManager->GetTexture(mOverTexture.const_ref());
		}

		if(mDownTexturePointer)
			mDownTexturePointer=0;

		if(mDownTexture.const_ref() !="")
		{
			mDownTexturePointer = textureManager->GetTexture(mDownTexture.const_ref());
		}


		//sauvegarde les position initiales du slider
		mInitialPosX = (int)mPosition[0];
		mInitialPosY = (int)mPosition[1];

		//Set slider Data
		float Dock[2];
		int newPos[2] = {mInitialPosX, mInitialPosY};
		v2f getL;
		if(((const kstl::string&)mDirection) == "Vertical")
		{
			//Set Anchor and dock
			Dock[0] = 0.5f;
			Dock[1] = 1.0f;

			//Set slider length 
			GetParents()[0]->getValue("Size", getL);
			mSlideLength = getL.y;
			mSlideLength -= mSize[1];

			if(mInitialRatio>0)
			{
				 newPos[1] = mInitialPosY - ((mSlideLength * mInitialRatio) / 100);
				 this->setArrayValue("Position",newPos,2);
			}
		}
		else
		{
			//Set Anchor and dock
			Dock[0] = 0.0f;
			Dock[1] = 0.5f;

			//Set slider length 
			GetParents()[0]->getValue("Size", getL);
			mSlideLength = getL.x;
			mSlideLength -= mSize[0];

			if(mInitialRatio>0)
			{
				newPos[0] = mInitialPosX + ((mSlideLength * mInitialRatio) / 100);
				this->setArrayValue("Position",newPos,2);
			}
		}
		mCurrentRatio = mInitialRatio;

		this->setArrayValue("Dock",Dock,2);
		this->setArrayValue("Anchor",Dock,2);

		

		// auto size button
		if( (((int)mSize[0])==0) && (((int)mSize[1])==0) )
		{
			if(mUpTexturePointer)
			{
				unsigned int width,height;

				mUpTexturePointer->GetSize(width,height);
				mSize=v2f(width,height);
			}
		}

		mIsEnabled.changeNotificationLevel(Owner);

	}
}

void	UISlider::ForcedRatio(unsigned int value)
{
	//Set slider Data
	int newPos[2] = {mInitialPosX, mInitialPosY};
	if(((const kstl::string&)mDirection) == "Vertical")
	{
		newPos[1] = mInitialPosY - ((mSlideLength * value) / 100);
		this->setArrayValue("Position",newPos,2);
	}
	else
	{
		newPos[0] = mInitialPosX + ((mSlideLength * value) / 100);
		this->setArrayValue("Position",newPos,2);
	}
	mCurrentRatio = value;
}


void	UISlider::ResetSlider()
{
	int newPos[2] = {mInitialPosX, mInitialPosY};
	if(((const kstl::string&)mDirection) == "Vertical")
	{
		if(mInitialRatio>0)
		{
			newPos[1] = mInitialPosY - ((mSlideLength * mInitialRatio) / 100);
		}
	}
	else
	{
		if(mInitialRatio>0)
		{
			newPos[0] = mInitialPosX + ((mSlideLength * mInitialRatio) / 100);
		}
	}
	this->setArrayValue("Position",newPos,2);
}

void UISlider::ChangeTexture(kstl::string _texturename, kstl::string _overtexturename, kstl::string _downtexturename)
{
	if(mIsEnabled) // down and mouse over only when enabled
	{
		auto textureManager = KigsCore::Singleton<TextureFileManager>();
		if(mUpTexturePointer)
			mUpTexturePointer=0;

		mUpTexture = _texturename;
		mUpTexturePointer = textureManager->GetTexture(mUpTexture);

		if(mOverTexturePointer && _overtexturename != "")
			mOverTexturePointer=0;

		if(_overtexturename != "")
		{
			mOverTexture = _overtexturename;
			mOverTexturePointer = textureManager->GetTexture(mOverTexture);
		}

		if(mDownTexturePointer && _downtexturename != "")
			mDownTexturePointer=0;

		if(_downtexturename != "")
		{
			mOverTexture = _downtexturename;
			mDownTexturePointer = textureManager->GetTexture(mDownTexture);
		}

		//auto Size
		if(mUpTexturePointer)
		{
			unsigned int width,height;

			mUpTexturePointer->GetSize(width,height);

			if(mSize[0] != width || mSize[1] != height)
			{
				mSize = v2f(width, height);
				SetNodeFlag(Node2D_SizeChanged);
			}
		}
	}
}

Texture* UISlider::GetTexture()
{
	if(mIsEnabled) // down and mouse over only when enabled
	{
		if(mIsDown)
			return (mDownTexturePointer) ? mDownTexturePointer.get() : mUpTexturePointer.get();
		if(mIsMouseOver)
			return (mOverTexturePointer) ? mOverTexturePointer.get() : mUpTexturePointer.get();
	}
	return mUpTexturePointer.get();
}

bool UISlider::isAlpha(float X, float Y)
{
	//Try to get mask
	if(!mAlphaMask)
	{
		kstl::vector<ModifiableItemStruct> sons = getItems();

		for(unsigned int i=0; i < sons.size(); i++)
		{
			if(sons[i].mItem->isSubType("AlphaMask"))
			{
				mAlphaMask = sons[i].mItem;
				break;
			}
		}
	}

	if(mAlphaMask)
	{
		//Check on mask the specified location
		return !mAlphaMask->CheckTo(X,Y);
	}

	return false;
}