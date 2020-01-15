#include "UI/UISprite.h"
#include "TextureFileManager.h"
#include "Texture.h"
#include "AlphaMask.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UISprite, UISprite, 2DLayers);
IMPLEMENT_CLASS_INFO(UISprite)


UISprite::UISprite(const kstl::string& name, CLASS_NAME_TREE_ARG) :
UITexturedItem(name, PASS_CLASS_NAME_TREE_ARG)
, myTextureName(*this, false, LABEL_AND_ID(Texture), "")
, mySpriteName(*this, false, LABEL_AND_ID(Sprite), "")
{

	mySpriteSheet = 0;
	hasSprite = false;
}

UISprite::~UISprite()
{
}

//-----------------------------------------------------------------------------------------------------
//ReloadTexture

void UISprite::NotifyUpdate(const unsigned int labelid)
{
	if (labelid == myIsEnabled.getLabelID())
	{
		if (!GetSons().empty())
		{
			kstl::set<Node2D*, Node2D::PriorityCompare> sons = GetSons();
			kstl::set<Node2D*, Node2D::PriorityCompare>::iterator it = sons.begin();
			kstl::set<Node2D*, Node2D::PriorityCompare>::iterator end = sons.end();
			while (it != end)
			{
				(*it)->setValue(LABEL_TO_ID(IsEnabled), myIsEnabled);
				it++;
			}
		}
	}
	else if (labelid == myTextureName.getLabelID() && _isInit)
	{
		ChangeTexture();
	}
	else if (labelid == mySpriteName.getLabelID() && _isInit)
	{
		ChangeTexture();
	}
	UITexturedItem::NotifyUpdate(labelid);
}

void UISprite::InitModifiable()
{
	if (_isInit)
		return;
	UITexturedItem::InitModifiable();
	if (_isInit)
	{
		ChangeTexture();
		myIsEnabled.changeNotificationLevel(Owner);
		myTextureName.changeNotificationLevel(Owner);
		mySpriteName.changeNotificationLevel(Owner);
	}
}

void UISprite::ChangeTexture()
{
	hasSprite = false;

	const kstl::string& lTexName = myTextureName.const_ref();
	if (lTexName == "")
		return;

	SP<TextureFileManager> textureManager = KigsCore::GetSingleton("TextureFileManager");

	
	mySpriteSheet =  textureManager->GetSpriteSheetTexture(lTexName);

	SetTexture(mySpriteSheet->Get_Texture());
	if (!myTexture)
		return;

	myTexture->Init();

	const SpriteSheetFrame * f = mySpriteSheet->Get_Frame(mySpriteName);
	if (f == nullptr)
		return;

	hasSprite = true;


	Point2D s, r;
	myTexture->GetSize(s.x, s.y);
	myTexture->GetRatio(r.x, r.y);
	s /= r;

	myUVMin.Set((kfloat)f->FramePos_X+0.5f, (kfloat)f->FramePos_Y+0.5f);
	myUVMin /= s;

	myUVMax.Set((kfloat)(f->FramePos_X+f->FrameSize_X-0.5f), (kfloat)(f->FramePos_Y+f->FrameSize_Y-0.5f));
	myUVMax /= s;
	//myUVMax = myUVMin + myUVMax;

	// auto size
	if ((((unsigned int)mySizeX) == 0) && (((unsigned int)mySizeY) == 0))
	{
		mySizeX = f->FrameSize_X;
		mySizeY = f->FrameSize_Y;
	}
}

bool UISprite::isAlpha(float X, float Y)
{
	//Try to get my mask
	if (!myAlphaMask)
	{
		kstl::vector<ModifiableItemStruct> sons = getItems();

		for (unsigned int i = 0; i < sons.size(); i++)
		{
			if (sons[i].myItem->isSubType("AlphaMask"))
			{
				myAlphaMask = sons[i].myItem;
				break;
			}
		}
	}

	if (myAlphaMask)
	{
		return !myAlphaMask->CheckTo(X, Y);
	}

	return false;
}

void UISprite::SetTexUV(UIVerticesInfo * aQI)
{
	if (hasSprite)
	{
		kfloat ratioX, ratioY, sx, sy;
		myTexture->GetSize(sx, sy);
		myTexture->GetRatio(ratioX, ratioY);

		kfloat dx = 0.5f / sx;
		kfloat dy = 0.5f / sy;

		VInfo2D::Data* buf = reinterpret_cast<VInfo2D::Data*>(aQI->Buffer());

		aQI->Flag |= UIVerticesInfo_Texture;
		// triangle strip order
		buf[0].setTexUV(myUVMin.x + dx, myUVMin.y + dy);
		buf[1].setTexUV(myUVMin.x + dx, myUVMax.y - dy);
		buf[3].setTexUV(myUVMax.x - dx, myUVMax.y - dy);
		buf[2].setTexUV(myUVMax.x - dx, myUVMin.y + dy);
	}
}