#include "UI/UIVideo.h"
#include "TextureFileManager.h"
#include "Texture.h"
#include "AlphaMask.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIVideo, UIVideo, 2DLayers);
IMPLEMENT_CLASS_INFO(UIVideo)

UIVideo::UIVideo(const kstl::string& name,CLASS_NAME_TREE_ARG) : 
UIItem(name, PASS_CLASS_NAME_TREE_ARG)
,mLoop(*this,false,"Loop",false)
,mHideAtTheEnd(*this,false,"HideAtTheEnd",false)
,mHasAlpha(*this,false,"HasAlpha",false)
, mIsMuted(*this, false, "IsMuted", false)
, mVolume(*this, false, "Volume", 1.0f)
, mVideoName(*this, false, "Video", "")
, mFinished(false)
,mOrder(*this,false,"Order","Play","Pause","Stop")


{
	mIsEnabled.changeNotificationLevel(Owner);
	mOrder.changeNotificationLevel(Owner);

	mVolume.changeNotificationLevel(Owner);
	mIsMuted.changeNotificationLevel(Owner);
	mOpacity = 1.0f;
}

UIVideo::~UIVideo()
{
	if(mTexturePointer)
		mTexturePointer = nullptr;

	if(mBufferTexturePointer)
		mBufferTexturePointer = nullptr;

	if(mAlphaMask)
		mAlphaMask = nullptr;
}

//-----------------------------------------------------------------------------------------------------
//ReloadTexture

void	UIVideo::ReloadTexture()
{
	if(mTexturePointer)
		mTexturePointer->ReInit();
	if(mBufferTexturePointer)
		mBufferTexturePointer->ReInit();
}

void UIVideo::NotifyUpdate(const unsigned int labelid )
{
	if(labelid==mIsEnabled.getLabelID()) 
	{
		if(!GetSons().empty())
		{
			kstl::set<Node2D*, Node2D::PriorityCompare> sons = GetSons();
			kstl::set<Node2D*, Node2D::PriorityCompare>::iterator it = sons.begin();
			kstl::set<Node2D*, Node2D::PriorityCompare>::iterator end = sons.end();
			while(it!=end)
			{
				(*it)->setValue("IsEnabled",mIsEnabled);
				it++;
			}
		}
	}
	else if (labelid==mOrder.getLabelID())
	{
		if((const kstl::string&)mOrder == "Play")
		{
			mFinished = false;
			mIsHidden = false;
		}
		else if((const kstl::string&)mOrder == "Pause")
		{
			mFinished = true;
			mIsHidden = false;
		}
		else if((const kstl::string&)mOrder == "Stop")
		{
			mFinished = true;
			mIsHidden = true;
		}
	}

	UIItem::NotifyUpdate(labelid);
}

void UIVideo::InitModifiable()
{
	UIItem::InitModifiable();
	if(IsInit())
	{
		if((const kstl::string&)mOrder == "Play")
		{
			mFinished = false;
			mIsHidden = false;
		}
		else if((const kstl::string&)mOrder == "Pause")
		{
			mFinished = true;
			mIsHidden = false;
		}
		else if((const kstl::string&)mOrder == "Stop")
		{
			mFinished = true;
			mIsHidden = true;
		}

		// Display texture
		if(mTexturePointer)
			mTexturePointer=nullptr;

		mTexturePointer = KigsCore::GetInstanceOf(mVideoName, "Texture");
		mTexturePointer->setValue("Width",mSizeX);
		mTexturePointer->setValue("Height",mSizeY);
		mTexturePointer->Init();

		//Buffer Texture
		if(mBufferTexturePointer)
			mBufferTexturePointer=0;

		kstl::string LBufferName = mVideoName;
		LBufferName = LBufferName + "Buffer";
		mBufferTexturePointer = KigsCore::GetInstanceOf(LBufferName, "Texture");
		mBufferTexturePointer->setValue("Width",mSizeX);
		mBufferTexturePointer->setValue("Height",mSizeY*2);
		mBufferTexturePointer->Init();

		// auto size
#ifdef _DEBUG
		if( (((unsigned int)mSizeX)==0) && (((unsigned int)mSizeY)==0) )
		{
			printf("You must specify a size for init a video element\n");
		}
#endif
	}
}

Texture* UIVideo::GetTexture()
{
	if(mTexturePointer)
		return mTexturePointer.get();
	
	return nullptr;
}

Texture*	UIVideo::Get_BufferTexture()
{
	if(mBufferTexturePointer)
		return mBufferTexturePointer.get();

	return nullptr;
}

bool UIVideo::isAlpha(float X, float Y)
{
	//Try to get my mask
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
		//Check on my mask the specified location
		return !mAlphaMask->CheckTo(X,Y);
	}

	return false;
}