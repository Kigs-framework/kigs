#include "UI/UIVideo.h"
#include "TextureFileManager.h"
#include "Texture.h"
#include "AlphaMask.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIVideo, UIVideo, 2DLayers);
IMPLEMENT_CLASS_INFO(UIVideo)

UIVideo::UIVideo(const kstl::string& name,CLASS_NAME_TREE_ARG) : 
UIItem(name, PASS_CLASS_NAME_TREE_ARG)
,myBLoop(*this,false,"Loop",false)
,myBHideAtTheEnd(*this,false,"HideAtTheEnd",false)
,mybHasAlpha(*this,false,"HasAlpha",false)
, mybIsMuted(*this, false, "IsMuted", false)
, myVolume(*this, false, "Volume", 1.0f)
, myVideoName(*this, false, "Video", "")
, myBFinished(false)
,m_VideoOrder(*this,false,"Order","Play","Pause","Stop")


{
	myIsEnabled.changeNotificationLevel(Owner);
	m_VideoOrder.changeNotificationLevel(Owner);

	myVolume.changeNotificationLevel(Owner);
	mybIsMuted.changeNotificationLevel(Owner);
	myOpacity = 1.0f;
}

UIVideo::~UIVideo()
{
	if(myTexture)
		myTexture = nullptr;

	if(myBufferTexture)
		myBufferTexture = nullptr;

	if(myAlphaMask)
		myAlphaMask = nullptr;
}

//-----------------------------------------------------------------------------------------------------
//ReloadTexture

void	UIVideo::ReloadTexture()
{
	if(myTexture)
		myTexture->ReInit();
	if(myBufferTexture)
		myBufferTexture->ReInit();
}

void UIVideo::NotifyUpdate(const unsigned int labelid )
{
	if(labelid==myIsEnabled.getLabelID()) 
	{
		if(!GetSons().empty())
		{
			kstl::set<Node2D*, Node2D::PriorityCompare> sons = GetSons();
			kstl::set<Node2D*, Node2D::PriorityCompare>::iterator it = sons.begin();
			kstl::set<Node2D*, Node2D::PriorityCompare>::iterator end = sons.end();
			while(it!=end)
			{
				(*it)->setValue("IsEnabled",myIsEnabled);
				it++;
			}
		}
	}
	else if (labelid==m_VideoOrder.getLabelID())
	{
		if((const kstl::string&)m_VideoOrder == "Play")
		{
			myBFinished = false;
			myIsHidden = false;
		}
		else if((const kstl::string&)m_VideoOrder == "Pause")
		{
			myBFinished = true;
			myIsHidden = false;
		}
		else if((const kstl::string&)m_VideoOrder == "Stop")
		{
			myBFinished = true;
			myIsHidden = true;
		}
	}

	UIItem::NotifyUpdate(labelid);
}

void UIVideo::InitModifiable()
{
	UIItem::InitModifiable();
	if(IsInit())
	{
		if((const kstl::string&)m_VideoOrder == "Play")
		{
			myBFinished = false;
			myIsHidden = false;
		}
		else if((const kstl::string&)m_VideoOrder == "Pause")
		{
			myBFinished = true;
			myIsHidden = false;
		}
		else if((const kstl::string&)m_VideoOrder == "Stop")
		{
			myBFinished = true;
			myIsHidden = true;
		}

		// Display texture
		if(myTexture)
			myTexture=nullptr;

		myTexture = KigsCore::GetInstanceOf(myVideoName, "Texture");
		myTexture->setValue("Width",mySizeX);
		myTexture->setValue("Height",mySizeY);
		myTexture->Init();

		//Buffer Texture
		if(myBufferTexture)
			myBufferTexture=0;

		kstl::string LBufferName = myVideoName;
		LBufferName = LBufferName + "Buffer";
		myBufferTexture = KigsCore::GetInstanceOf(LBufferName, "Texture");
		myBufferTexture->setValue("Width",mySizeX);
		myBufferTexture->setValue("Height",mySizeY*2);
		myBufferTexture->Init();

		// auto size
#ifdef _DEBUG
		if( (((unsigned int)mySizeX)==0) && (((unsigned int)mySizeY)==0) )
		{
			printf("You must specify a size for init a video element\n");
		}
#endif
	}
}

Texture* UIVideo::GetTexture()
{
	if(myTexture)
		return myTexture.get();
	
	return nullptr;
}

Texture*	UIVideo::Get_BufferTexture()
{
	if(myBufferTexture)
		return myBufferTexture.get();

	return nullptr;
}

bool UIVideo::isAlpha(float X, float Y)
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