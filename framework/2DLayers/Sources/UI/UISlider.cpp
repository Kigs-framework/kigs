#include "UI/UISlider.h"

#include "TextureFileManager.h"
#include "Texture.h"
#include "NotificationCenter.h"
#include "AlphaMask.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UISlider, UISlider, 2DLayers);
IMPLEMENT_CLASS_INFO(UISlider)

UISlider::UISlider(const kstl::string& name,CLASS_NAME_TREE_ARG) : 
UIDrawableItem(name, PASS_CLASS_NAME_TREE_ARG),
myUpTextureName(*this,false,LABEL_AND_ID(UpTexture),""),
myDownTextureName(*this,false,LABEL_AND_ID(DownTexture),""),
myOverTextureName(*this,false,LABEL_AND_ID(OverTexture),""),
myClickUpAction(*this, false, LABEL_AND_ID(ClickUpAction), ""),
myParameter(*this, false, LABEL_AND_ID(Parameter), kstl::string("")),
myMoveAction(*this,false,LABEL_AND_ID(MoveAction),""),
myInitialRatio(*this, true, LABEL_AND_ID(InitialRatio), 0),
myDirection(*this,true,LABEL_AND_ID(Direction),"Vertical","Horizontal")
{
	
}

UISlider::~UISlider()
{
	if(myUpTexture)
		myUpTexture = NULL;
	if(myOverTexture)
		myOverTexture = NULL;
	if(myDownTexture)
		myDownTexture = NULL;
}

//-----------------------------------------------------------------------------------------------------
//ReloadTexture

void	UISlider::ReloadTexture()
{
	if(myUpTexture)
	{
		myUpTexture->ReInit();
	}

	if(myDownTexture)
	{
		myDownTexture->ReInit();
	}

	if(myOverTexture)
	{
		myOverTexture->ReInit();
	}
}

void UISlider::NotifyUpdate(const unsigned int labelid )
{
	if(labelid==myIsEnabled.getLabelID()) 
	{
		// if disabled "reset" button state
		if(myIsEnabled == false)
		{
			myIsDown=false;
			myIsMouseOver=false;
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
					if(parents[0]->isSubType(UIItem::myClassID))
					{
						myParent = (UIItem*)parents[0];
					}
				}
				else
				{
					UninitModifiable();
				}
			}
			else
			{
				if(parents[0]->isSubType(UIItem::myClassID))
				{
					myParent = (UIItem*)parents[0];
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
		myIsDown      = false;
		myIsMouseOver = false;

		// load texture
		auto& textureManager = KigsCore::Singleton<TextureFileManager>();
		if(myUpTexture)
			myUpTexture=0;
		
		myUpTexture = textureManager->GetTexture(myUpTextureName);

		if(myOverTexture)
			myOverTexture=0;

		if(myOverTextureName.const_ref() !="")
		{
			myOverTexture = textureManager->GetTexture(myOverTextureName.const_ref());
		}

		if(myDownTexture)
			myDownTexture=0;

		if(myDownTextureName.const_ref() !="")
		{
			myDownTexture = textureManager->GetTexture(myDownTextureName.const_ref());
		}


		//sauvegarde les position initiales du slider
		myInitialPosX = (int)myPosition[0];
		myInitialPosY = (int)myPosition[1];

		//Set slider Data
		float Dock[2];
		int newPos[2] = {myInitialPosX, myInitialPosY};
		if(((const kstl::string&)myDirection) == "Vertical")
		{
			//Set Anchor and dock
			Dock[0] = 0.5f;
			Dock[1] = 1.0f;

			//Set slider length 
			GetParents()[0]->getValue(LABEL_TO_ID(SizeY),mySlideLength);
			mySlideLength -= mySizeY;

			if(myInitialRatio>0)
			{
				 newPos[1] = myInitialPosY - ((mySlideLength * myInitialRatio) / 100);
				 this->setArrayValue(LABEL_TO_ID(Position),newPos,2);
			}
		}
		else
		{
			//Set Anchor and dock
			Dock[0] = 0.0f;
			Dock[1] = 0.5f;

			//Set slider length 
			GetParents()[0]->getValue(LABEL_TO_ID(SizeX),mySlideLength);
			mySlideLength -= mySizeX;

			if(myInitialRatio>0)
			{
				newPos[0] = myInitialPosX + ((mySlideLength * myInitialRatio) / 100);
				this->setArrayValue(LABEL_TO_ID(Position),newPos,2);
			}
		}
		myCurrentRatio = myInitialRatio;

		this->setArrayValue(LABEL_TO_ID(Dock),Dock,2);
		this->setArrayValue(LABEL_TO_ID(Anchor),Dock,2);

		

		// auto size button
		if( (((unsigned int)mySizeX)==0) && (((unsigned int)mySizeY)==0) )
		{
			if(myUpTexture)
			{
				unsigned int width,height;

				myUpTexture->GetSize(width,height);
				mySizeX=width;
				mySizeY=height;
			}
		}

		myIsEnabled.changeNotificationLevel(Owner);

	}
}

void	UISlider::ForcedRatio(unsigned int value)
{
	//Set slider Data
	int newPos[2] = {myInitialPosX, myInitialPosY};
	if(((const kstl::string&)myDirection) == "Vertical")
	{
		newPos[1] = myInitialPosY - ((mySlideLength * value) / 100);
		this->setArrayValue(LABEL_TO_ID(Position),newPos,2);
	}
	else
	{
		newPos[0] = myInitialPosX + ((mySlideLength * value) / 100);
		this->setArrayValue(LABEL_TO_ID(Position),newPos,2);
	}
	myCurrentRatio = value;
}


void	UISlider::ResetSlider()
{
	int newPos[2] = {myInitialPosX, myInitialPosY};
	if(((const kstl::string&)myDirection) == "Vertical")
	{
		if(myInitialRatio>0)
		{
			newPos[1] = myInitialPosY - ((mySlideLength * myInitialRatio) / 100);
		}
	}
	else
	{
		if(myInitialRatio>0)
		{
			newPos[0] = myInitialPosX + ((mySlideLength * myInitialRatio) / 100);
		}
	}
	this->setArrayValue(LABEL_TO_ID(Position),newPos,2);
}

void UISlider::ChangeTexture(kstl::string _texturename, kstl::string _overtexturename, kstl::string _downtexturename)
{
	if(myIsEnabled) // down and mouse over only when enabled
	{
		auto& textureManager = KigsCore::Singleton<TextureFileManager>();
		if(myUpTexture)
			myUpTexture=0;

		myUpTextureName = _texturename;
		myUpTexture = textureManager->GetTexture(myUpTextureName);

		if(myOverTexture && _overtexturename != "")
			myOverTexture=0;

		if(_overtexturename != "")
		{
			myOverTextureName = _overtexturename;
			myOverTexture = textureManager->GetTexture(myOverTextureName);
		}

		if(myDownTexture && _downtexturename != "")
			myDownTexture=0;

		if(_downtexturename != "")
		{
			myOverTextureName = _downtexturename;
			myDownTexture = textureManager->GetTexture(myDownTextureName);
		}

		//auto Size
		if(myUpTexture)
		{
			unsigned int width,height;

			myUpTexture->GetSize(width,height);

			if(mySizeX != width || mySizeY != height)
			{
				mySizeX = width;
				mySizeY = height;
				myNeedUpdatePosition = true;
			}
		}
	}
}

Texture* UISlider::GetTexture()
{
	if(myIsEnabled) // down and mouse over only when enabled
	{
		if(myIsDown)
			return (myDownTexture) ? myDownTexture.get() : myUpTexture.get();
		if(myIsMouseOver)
			return (myOverTexture) ? myOverTexture.get() : myUpTexture.get();
	}
	return myUpTexture.get();
}

bool UISlider::isAlpha(float X, float Y)
{
	//Try to get my mask
	if(!myAlphaMask)
	{
		kstl::vector<ModifiableItemStruct> sons = getItems();

		for(unsigned int i=0; i < sons.size(); i++)
		{
			if(sons[i].myItem->isSubType("AlphaMask"))
			{
				myAlphaMask = sons[i].myItem;
				break;
			}
		}
	}

	if(myAlphaMask)
	{
		//Check on my mask the specified location
		return !myAlphaMask->CheckTo(X,Y);
	}

	return false;
}