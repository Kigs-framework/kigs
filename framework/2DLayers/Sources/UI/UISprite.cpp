#include "UI/UISprite.h"
#include "TextureFileManager.h"
#include "Texture.h"
#include "AlphaMask.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UISprite, UISprite, 2DLayers);
IMPLEMENT_CLASS_INFO(UISprite)


UISprite::UISprite(const kstl::string& name, CLASS_NAME_TREE_ARG) :
UITexturedItem(name, PASS_CLASS_NAME_TREE_ARG)
, mTexture(*this, false, LABEL_AND_ID(Texture), "")
, mSprite(*this, false, LABEL_AND_ID(Sprite), "")
{

	mSpriteSheet = 0;
	hasSprite = false;
}

UISprite::~UISprite()
{
}

//-----------------------------------------------------------------------------------------------------
//ReloadTexture

void UISprite::NotifyUpdate(const unsigned int labelid)
{
	if (labelid == mIsEnabled.getLabelID())
	{
		if (!GetSons().empty())
		{
			kstl::set<Node2D*, Node2D::PriorityCompare> sons = GetSons();
			kstl::set<Node2D*, Node2D::PriorityCompare>::iterator it = sons.begin();
			kstl::set<Node2D*, Node2D::PriorityCompare>::iterator end = sons.end();
			while (it != end)
			{
				(*it)->setValue(LABEL_TO_ID(IsEnabled), mIsEnabled);
				it++;
			}
		}
	}
	else if (labelid == mTexture.getLabelID() && _isInit)
	{
		ChangeTexture();
	}
	else if (labelid == mSprite.getLabelID() && _isInit)
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
		mIsEnabled.changeNotificationLevel(Owner);
		mTexture.changeNotificationLevel(Owner);
		mSprite.changeNotificationLevel(Owner);
	}
}

void UISprite::ChangeTexture()
{
	hasSprite = false;

	const kstl::string& lTexName = mTexture.const_ref();
	if (lTexName == "")
		return;

	auto& textureManager = KigsCore::Singleton<TextureFileManager>();

	
	mSpriteSheet =  textureManager->GetSpriteSheetTexture(lTexName);

	SetTexture(mSpriteSheet->Get_Texture());
	if (!mTexturePointer)
		return;

	mTexturePointer->Init();

	const SpriteSheetFrame * f = mSpriteSheet->Get_Frame(mSprite);
	if (f == nullptr)
		return;

	hasSprite = true;


	Point2D s, r;
	mTexturePointer->GetSize(s.x, s.y);
	mTexturePointer->GetRatio(r.x, r.y);
	s /= r;

	myUVMin.Set((kfloat)f->FramePos_X+0.5f, (kfloat)f->FramePos_Y+0.5f);
	myUVMin /= s;

	myUVMax.Set((kfloat)(f->FramePos_X+f->FrameSize_X-0.5f), (kfloat)(f->FramePos_Y+f->FrameSize_Y-0.5f));
	myUVMax /= s;
	//myUVMax = myUVMin + myUVMax;

	// auto size
	if ((((unsigned int)mSizeX) == 0) && (((unsigned int)mSizeY) == 0))
	{
		mSizeX = f->FrameSize_X;
		mSizeY = f->FrameSize_Y;
	}
}

bool UISprite::isAlpha(float X, float Y)
{
	//Try to get my mask
	if (!mAlphaMask)
	{
		kstl::vector<ModifiableItemStruct> sons = getItems();

		for (unsigned int i = 0; i < sons.size(); i++)
		{
			if (sons[i].mItem->isSubType("AlphaMask"))
			{
				mAlphaMask = sons[i].mItem;
				break;
			}
		}
	}

	if (mAlphaMask)
	{
		return !mAlphaMask->CheckTo(X, Y);
	}

	return false;
}

void UISprite::SetTexUV(UIVerticesInfo * aQI)
{
	if (hasSprite)
	{
		kfloat ratioX, ratioY, sx, sy;
		mTexturePointer->GetSize(sx, sy);
		mTexturePointer->GetRatio(ratioX, ratioY);

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