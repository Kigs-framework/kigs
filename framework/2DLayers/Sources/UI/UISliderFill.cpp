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
mVoidTexture(*this,false,LABEL_AND_ID(VoidTexture),""),
mStartTexture(*this,false,LABEL_AND_ID(StartTexture),""),
mMiddleTexture(*this,false,LABEL_AND_ID(MiddleTexture),""),
mEndTexture(*this,false,LABEL_AND_ID(EndTexture),""),
mDirection(*this,true,LABEL_AND_ID(Direction),"Vertical","Horizontal"),
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
	if(labelid==mIsEnabled.getLabelID()) 
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
		auto& textureManager = KigsCore::Singleton<TextureFileManager>();
		mVoidTexturePointer = textureManager->GetTexture(mVoidTexture);

		// auto size button
		if( (((unsigned int)mSizeX)==0) && (((unsigned int)mSizeY)==0) )
		{
			if(mVoidTexturePointer)
			{
				unsigned int width,height;

				mVoidTexturePointer->GetSize(width,height);
				mSizeX=width;
				mSizeY=height;
			}
		}

		
		if(mStartTexture.const_ref() !="")
		{
			mStartTexturePointer = textureManager->GetTexture(mStartTexture.const_ref());
		}

		
		if(mMiddleTexture.const_ref() !="")
		{
			mMiddleTexturePointer = textureManager->GetTexture(mMiddleTexture.const_ref());
		}

		if(mEndTexture.const_ref() !="")
		{
			mEndTexturePointer = textureManager->GetTexture(mEndTexture.const_ref());
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
		mIsEnabled.changeNotificationLevel(Owner);
	}
}

void UISliderFill::ChangeTexture(kstl::string _voidtexturename,kstl::string _starttexturename, kstl::string _middletexturename, kstl::string _endtexturename)
{
	if(mIsEnabled) // down and mouse over only when enabled
	{
		auto& textureManager = KigsCore::Singleton<TextureFileManager>();

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

			if(mSizeX != width || mSizeY != height)
			{
				mSizeX = width;
				mSizeY = height;
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

	if(mStartTexturePointer)
	{
		if(ratio>0)
		{
			unsigned int width,height;
			mStartTexturePointer->GetSize(width,height);
			if(((const kstl::string&)mDirection) == "Vertical")
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
			
			int currentx,currenty;
			int newpos[2] ={0};
			child->getArrayValue(LABEL_TO_ID(Position),newpos,2);
			child->getValue(LABEL_TO_ID(SizeX),currentx);
			child->getValue(LABEL_TO_ID(SizeY),currenty);

			if(((const kstl::string&)mDirection) == "Vertical")
			{
				mMiddleSizeX =  newpos[0] - px + (currentx/2);
				mMiddleSizeY = py;
				RecomputeElementsPosition(0.0f,0.0f,0.0f, 0.0f, px, 0, width, height, mMiddlePositionX, mMiddlePositionY);
			}
			else
			{
				mMiddleSizeY =  newpos[1] + py - (currenty);
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
			child->getArrayValue(LABEL_TO_ID(Position),newpos,2);

			unsigned int width,height;
			mEndTexturePointer->GetSize(width,height);
			if(((const kstl::string&)mDirection) == "Vertical")
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
	Point2D	pt((float)_posx, (float)_posy);

	TransformPoints(&pt, 1);

	Xresult = (int)(pt.x);
	Yresult = (int)(pt.y);

}
