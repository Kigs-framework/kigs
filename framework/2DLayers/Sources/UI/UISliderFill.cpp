#include "UI/UISliderFill.h"
#include "UI/UISlider.h"
#include "TextureFileManager.h"
#include "Texture.h"
#include "NotificationCenter.h"
#include "AlphaMask.h"

using namespace Kigs::Draw2D;
IMPLEMENT_CLASS_INFO(UISliderFill)

UISliderFill::UISliderFill(const std::string& name,CLASS_NAME_TREE_ARG) : 
UIDrawableItem(name, PASS_CLASS_NAME_TREE_ARG),
mVoidTexture(*this,"VoidTexture",""),
mStartTexture(*this,"StartTexture",""),
mMiddleTexture(*this,"MiddleTexture",""),
mEndTexture(*this,"EndTexture",""),
mDirection(*this,"Direction","Vertical","Horizontal"),
mStartTexturePointer(0),
mMiddleTexturePointer(0),
mEndTexturePointer(0),
mVoidTexturePointer(0)
{
	mStartPositionX = 0;
	mStartPositionY = 0;
	mMiddlePositionX = 0;
	mMiddlePositionY = 0;
	mEndPositionX = 0;
	mEndPositionY = 0;

	
}



void UISliderFill::NotifyUpdate(const unsigned int labelid )
{
	if(labelid== KigsID("IsEnabled")._id)
	{
		if(mIsEnabled == false)
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
		auto textureManager = KigsCore::Singleton<Draw::TextureFileManager>();
		mVoidTexturePointer = textureManager->GetTexture(mVoidTexture);

		// auto size button
		if( (((int)mSize[0])==0) && (((int)mSize[1])==0) )
		{
			if(mVoidTexturePointer)
			{
				unsigned int width,height;

				mVoidTexturePointer->GetSize(width,height);
				mSize=v2f(width,height);
			}
		}

		
		if(mStartTexture !="")
		{
			mStartTexturePointer = textureManager->GetTexture(mStartTexture);
		}

		
		if(mMiddleTexture !="")
		{
			mMiddleTexturePointer = textureManager->GetTexture(mMiddleTexture);
		}

		if(mEndTexture !="")
		{
			mEndTexturePointer = textureManager->GetTexture(mEndTexture);
		}

		//Init childs
		if(this->GetSons().size()>0)
		{
			std::set<Node2D*, Node2D::PriorityCompare> sons = this->GetSons();
			std::set<Node2D*, Node2D::PriorityCompare>::iterator it = sons.begin();
			std::set<Node2D*, Node2D::PriorityCompare>::iterator end = sons.end();
			while(it!=end)
			{
				if(!(*it)->IsInit())
				{
					(*it)->Init();
				}
				it++;
			}
		}
		setOwnerNotification("IsEnabled", true);
	}
}

void UISliderFill::ChangeTexture(std::string _voidtexturename,std::string _starttexturename, std::string _middletexturename, std::string _endtexturename)
{
	if(mIsEnabled) // down and mouse over only when enabled
	{
		auto textureManager = KigsCore::Singleton<Draw::TextureFileManager>();

		mVoidTexture = _voidtexturename;
		mVoidTexturePointer = textureManager->GetTexture(mVoidTexture);

		if(_starttexturename != "")
		{
			mStartTexture = _starttexturename;
			mStartTexturePointer = textureManager->GetTexture(mStartTexture);
		}

		if(_middletexturename != "")
		{
			mMiddleTexture = _middletexturename;
			mMiddleTexturePointer = textureManager->GetTexture(mMiddleTexture);
		}

		if(_endtexturename != "")
		{
			mEndTexture = _endtexturename;
			mEndTexturePointer = textureManager->GetTexture(mEndTexture);
		}

		//auto Size
		if(mVoidTexturePointer)
		{
			unsigned int width,height;

			mVoidTexturePointer->GetSize(width,height);

			if(mSize[0] != width || mSize[1] != height)
			{
				mSize = v2f(width, height);
				SetNodeFlag(Node2D_SizeChanged);
			}
		}

		ComputeInitialElementsPosition();
	}
}

bool UISliderFill::isAlpha(float X, float Y)
{
	//Try to get mask
	if(!mAlphaMask)
	{
		std::vector<ModifiableItemStruct> sons = getItems();

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


void UISliderFill::ComputeInitialElementsPosition()
{
	unsigned int ratio = 0;
	UISlider* child = NULL;
	if(!this->GetSons().empty())
	{
		std::set<Node2D*, Node2D::PriorityCompare> sons = this->GetSons();
		std::set<Node2D*, Node2D::PriorityCompare>::iterator it = sons.begin();
		std::set<Node2D*, Node2D::PriorityCompare>::iterator end = sons.end();
		if(it!=end)
		{
			child = (UISlider*)(*it);
			ratio = child->Get_CurrentRatio();	
		}
	}

	if(mStartTexturePointer)
	{
		if(ratio>0)
		{
			unsigned int width,height;
			mStartTexturePointer->GetSize(width,height);
			if(((const std::string&)mDirection) == "Vertical")
				RecomputeElementsPosition(0.0f,0.0f,0.0f, 0.0f, 0, 0, width, height, mStartPositionX, mStartPositionY);
			else
				RecomputeElementsPosition(0.0f,0.0f,0.0f, 1.0f, 0, 0, width, height, mStartPositionX, mStartPositionY);
		}
		else
		{
			mStartPositionX = -1;
			mStartPositionY = -1;
		}
	}
	if(mMiddleTexturePointer)
	{
		if(ratio>0)
		{
			unsigned int width,height;
			mMiddleTexturePointer->GetSize(width,height);
			unsigned int px,py;
			mStartTexturePointer->GetSize(px,py);
			
			v2f currentS;
			int newpos[2] ={0};
			child->getArrayValue("Position",newpos,2);
			child->getValue("Size", currentS);

			if(((const std::string&)mDirection) == "Vertical")
			{
				mMiddleSizeX =  newpos[0] - px + (currentS.x/2);
				mMiddleSizeY = py;
				RecomputeElementsPosition(0.0f,0.0f,0.0f, 0.0f, px, 0, width, height, mMiddlePositionX, mMiddlePositionY);
			}
			else
			{
				mMiddleSizeY =  newpos[1] + py - (currentS.y);
				mMiddleSizeX = px;
				RecomputeElementsPosition(0.0f,0.0f,0.0f, 0.0f, 0, mRealSize.y - py, mMiddleSizeX, mMiddleSizeY, mMiddlePositionX, mMiddlePositionY);
			}
		}
		else
		{
			mMiddlePositionX = -1;
			mMiddlePositionY = -1;
		}
	}
	if(mEndTexturePointer)
	{
		if(ratio>0)
		{
			v2f size = child->GetSize();
					
			int newpos[2] ={0};
			child->getArrayValue("Position",newpos,2);

			unsigned int width,height;
			mEndTexturePointer->GetSize(width,height);
			if(((const std::string&)mDirection) == "Vertical")
				RecomputeElementsPosition(0.0f,0.0f,0.0f, 0.0f, newpos[0]+(size.x/2), 0, width, height, mEndPositionX, mEndPositionY);
			else
				RecomputeElementsPosition(0.0f,0.0f,0.0f, 0.0f, newpos[0], newpos[1] + mRealSize.y -size.y, width, height, mEndPositionX, mEndPositionY);
		}
		else
		{
			mEndPositionX = -1;
			mEndPositionY = -1;
		}
	
	}
}

void UISliderFill::RecomputeElementsPosition(float _dockX, float _dockY, float _AnchorX,float _AnchorY, int _posx, int _posy, unsigned int _sizeX, unsigned int _sizeY, int& Xresult, int& Yresult)
{
	v2f	pt((float)_posx, (float)_posy);

	TransformPoints(&pt, 1);

	Xresult = (int)(pt.x);
	Yresult = (int)(pt.y);

}
