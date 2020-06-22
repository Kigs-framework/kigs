#include "UI/UISliderFill.h"
#include "UI/UISlider.h"
#include "TextureFileManager.h"
#include "Texture.h"
#include "NotificationCenter.h"
#include "AlphaMask.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UISliderFill, UISliderFill, 2DLayers);
IMPLEMENT_CLASS_INFO(UISliderFill)

UISliderFill::UISliderFill(const kstl::string& name,CLASS_NAME_TREE_ARG) : 
UIDrawableItem(name, PASS_CLASS_NAME_TREE_ARG),
myVoidTextureName(*this,false,LABEL_AND_ID(VoidTexture),""),
myStartTextureName(*this,false,LABEL_AND_ID(StartTexture),""),
myMiddleTextureName(*this,false,LABEL_AND_ID(MiddleTexture),""),
myEndTextureName(*this,false,LABEL_AND_ID(EndTexture),""),
myDirection(*this,true,LABEL_AND_ID(Direction),"Vertical","Horizontal"),
myStartTexture(0),
myMiddleTexture(0),
myEndTexture(0),
myVoidTexture(0)
{
	myStartPositionX = 0;
	myStartPositionY = 0;
	myMiddlePositionX = 0;
	myMiddlePositionY = 0;
	myEndPositionX = 0;
	myEndPositionY = 0;

	
}



void UISliderFill::NotifyUpdate(const unsigned int labelid )
{
	if(labelid==myIsEnabled.getLabelID()) 
	{
		if(myIsEnabled == false)
		{
			//Do something
		}
	}
	UIDrawableItem::NotifyUpdate(labelid);
}

void UISliderFill::InitModifiable()
{
	UIDrawableItem::InitModifiable();

	if(_isInit)
	{
		// load texture
		auto& textureManager = KigsCore::Singleton<TextureFileManager>();
		myVoidTexture = textureManager->GetTexture(myVoidTextureName);

		// auto size button
		if( (((unsigned int)mySizeX)==0) && (((unsigned int)mySizeY)==0) )
		{
			if(myVoidTexture)
			{
				unsigned int width,height;

				myVoidTexture->GetSize(width,height);
				mySizeX=width;
				mySizeY=height;
			}
		}

		
		if(myStartTextureName.const_ref() !="")
		{
			myStartTexture = textureManager->GetTexture(myStartTextureName.const_ref());
		}

		
		if(myMiddleTextureName.const_ref() !="")
		{
			myMiddleTexture = textureManager->GetTexture(myMiddleTextureName.const_ref());
		}

		if(myEndTextureName.const_ref() !="")
		{
			myEndTexture = textureManager->GetTexture(myEndTextureName.const_ref());
		}

		//Init childs
		if(this->GetSons().size()>0)
		{
			kstl::set<Node2D*, Node2D::PriorityCompare> sons = this->GetSons();
			kstl::set<Node2D*, Node2D::PriorityCompare>::iterator it = sons.begin();
			kstl::set<Node2D*, Node2D::PriorityCompare>::iterator end = sons.end();
			while(it!=end)
			{
				if(!(*it)->IsInit())
				{
					(*it)->Init();
				}
				it++;
			}
		}
		myIsEnabled.changeNotificationLevel(Owner);
	}
}

void UISliderFill::ChangeTexture(kstl::string _voidtexturename,kstl::string _starttexturename, kstl::string _middletexturename, kstl::string _endtexturename)
{
	if(myIsEnabled) // down and mouse over only when enabled
	{
		auto& textureManager = KigsCore::Singleton<TextureFileManager>();

		myVoidTextureName = _voidtexturename;
		myVoidTexture = textureManager->GetTexture(myVoidTextureName);

		if(_starttexturename != "")
		{
			myStartTextureName = _starttexturename;
			myStartTexture = textureManager->GetTexture(myStartTextureName);
		}

		if(_middletexturename != "")
		{
			myMiddleTextureName = _middletexturename;
			myMiddleTexture = textureManager->GetTexture(myMiddleTextureName);
		}

		if(_endtexturename != "")
		{
			myEndTextureName = _endtexturename;
			myEndTexture = textureManager->GetTexture(myEndTextureName);
		}

		//auto Size
		if(myVoidTexture)
		{
			unsigned int width,height;

			myVoidTexture->GetSize(width,height);

			if(mySizeX != width || mySizeY != height)
			{
				mySizeX = width;
				mySizeY = height;
				myNeedUpdatePosition = true;
			}
		}

		ComputeInitialElementsPosition();
	}
}

bool UISliderFill::isAlpha(float X, float Y)
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


void UISliderFill::ComputeInitialElementsPosition()
{
	unsigned int ratio = 0;
	UISlider* child = NULL;
	if(!this->GetSons().empty())
	{
		kstl::set<Node2D*, Node2D::PriorityCompare> sons = this->GetSons();
		kstl::set<Node2D*, Node2D::PriorityCompare>::iterator it = sons.begin();
		kstl::set<Node2D*, Node2D::PriorityCompare>::iterator end = sons.end();
		if(it!=end)
		{
			child = (UISlider*)(*it);
			ratio = child->Get_CurrentRatio();	
		}
	}

	if(myStartTexture)
	{
		if(ratio>0)
		{
			unsigned int width,height;
			myStartTexture->GetSize(width,height);
			if(((const kstl::string&)myDirection) == "Vertical")
				RecomputeElementsPosition(0.0f,0.0f,0.0f, 0.0f, 0, 0, width, height, myStartPositionX, myStartPositionY);
			else
				RecomputeElementsPosition(0.0f,0.0f,0.0f, 1.0f, 0, 0, width, height, myStartPositionX, myStartPositionY);
		}
		else
		{
			myStartPositionX = -1;
			myStartPositionY = -1;
		}
	}
	if(myMiddleTexture)
	{
		if(ratio>0)
		{
			unsigned int width,height;
			myMiddleTexture->GetSize(width,height);
			unsigned int px,py;
			myStartTexture->GetSize(px,py);
			
			int currentx,currenty;
			int newpos[2] ={0};
			child->getArrayValue(LABEL_TO_ID(Position),newpos,2);
			child->getValue(LABEL_TO_ID(SizeX),currentx);
			child->getValue(LABEL_TO_ID(SizeY),currenty);

			if(((const kstl::string&)myDirection) == "Vertical")
			{
				myMiddleSizeX =  newpos[0] - px + (currentx/2);
				myMiddleSizeY = py;
				RecomputeElementsPosition(0.0f,0.0f,0.0f, 0.0f, px, 0, width, height, myMiddlePositionX, myMiddlePositionY);
			}
			else
			{
				myMiddleSizeY =  newpos[1] + py - (currenty);
				myMiddleSizeX = px;
				RecomputeElementsPosition(0.0f,0.0f,0.0f, 0.0f, 0, myRealSize.y - py, myMiddleSizeX, myMiddleSizeY, myMiddlePositionX, myMiddlePositionY);
			}
		}
		else
		{
			myMiddlePositionX = -1;
			myMiddlePositionY = -1;
		}
	}
	if(myEndTexture)
	{
		if(ratio>0)
		{
			v2f size = child->GetSize();
					
			int newpos[2] ={0};
			child->getArrayValue(LABEL_TO_ID(Position),newpos,2);

			unsigned int width,height;
			myEndTexture->GetSize(width,height);
			if(((const kstl::string&)myDirection) == "Vertical")
				RecomputeElementsPosition(0.0f,0.0f,0.0f, 0.0f, newpos[0]+(size.x/2), 0, width, height, myEndPositionX, myEndPositionY);
			else
				RecomputeElementsPosition(0.0f,0.0f,0.0f, 0.0f, newpos[0], newpos[1] + myRealSize.y -size.y, width, height, myEndPositionX, myEndPositionY);
		}
		else
		{
			myEndPositionX = -1;
			myEndPositionY = -1;
		}
	
	}
}

void UISliderFill::RecomputeElementsPosition(float _dockX, float _dockY, float _AnchorX,float _AnchorY, int _posx, int _posy, unsigned int _sizeX, unsigned int _sizeY, int& Xresult, int& Yresult)
{
	Point2D	pt((float)_posx, (float)_posy);

	TransformPoints(&pt, 1);

	Xresult = (int)(pt.x);
	Yresult = (int)(pt.y);

}
