#include "UI/UIAnimatedSprite.h"
#include "TextureFileManager.h"
#include "SpriteSheetTexture.h"
#include "AlphaMask.h"
#include "CoreBaseApplication.h"
#include "Timer.h"
#include "SpriteSheetTexture.h"
#include "Texture.h"
#include "UIVerticesInfo.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIAnimatedSprite, UIAnimatedSprite, 2DLayers);;
IMPLEMENT_CLASS_INFO(UIAnimatedSprite)

void UIAnimatedSprite::NotifyUpdate(const unsigned int labelid)
{
	if (labelid == mIsEnabled.getLabelID())
	{
		if (!GetSons().empty())
		{
			std::set<Node2D*, Node2D::PriorityCompare> sons = GetSons();
			std::set<Node2D*, Node2D::PriorityCompare>::iterator it = sons.begin();
			std::set<Node2D*, Node2D::PriorityCompare>::iterator end = sons.end();
			while (it != end)
			{
				(*it)->setValue("IsEnabled", mIsEnabled);
				it++;
			}
		}
	}
	else if (labelid == mCurrentAnimation.getLabelID())
	{
		ChangeAnimation(mCurrentAnimation.c_str());
	}
	else if (labelid == mFileName.getID())
	{
		ChangeSpriteSheet(mFileName);
	}
	UIDrawableItem::NotifyUpdate(labelid);
}

void UIAnimatedSprite::InitModifiable()
{
	UIDrawableItem::InitModifiable();

	if (IsInit())
	{
		if (mFileName.const_ref() != "")
		{
			ChangeSpriteSheet(mFileName);
		}
		mIsEnabled.changeNotificationLevel(Owner);
		mCurrentAnimation.changeNotificationLevel(Owner);
		mFileName.changeNotificationLevel(Owner);
	}
}

void UIAnimatedSprite::ComputeRealSize()
{
	ParentClassType::ComputeRealSize();
	if (myTexture)
	{
		if (mSizeX == 0 && mSizeModeX == DEFAULT)
			mRealSize.x = mCurrentSourceSize.x;
		if (mSizeY == 0 && mSizeModeY == DEFAULT)
			mRealSize.y = mCurrentSourceSize.y;
	}
}

void UIAnimatedSprite::ChangeSpriteSheet(std::string const& filename)
{
	auto& textureManager = KigsCore::Singleton<TextureFileManager>();
	mFileName = filename;
	myTexture = textureManager->GetSpriteSheetTexture(filename);
	ChangeAnimation(mCurrentAnimation.c_str());
}

SpriteSheetTexture* UIAnimatedSprite::GetSpriteSheetTexture()
{
	return myTexture.get();
}

bool UIAnimatedSprite::isAlpha(float X, float Y)
{
	//Try to get my mask
	if (!mAlphaMask)
	{
		std::vector<ModifiableItemStruct> sons = getItems();

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
		//Check on my mask the specified location
		return !mAlphaMask->CheckTo(X, Y);
	}

	return false;
}

void UIAnimatedSprite::ChangeAnimation(const std::string& _newAnimation)
{
	if (myTexture)
	{
		mFrameNumber = myTexture->Get_FrameNumber(_newAnimation);
		mCurrentAnimation = _newAnimation;
		mCurrentFrame = 0;
		mElpasedTime = 0;

		int L_SourceSizeX = 0;
		int L_SourceSizeY = 0;
		myTexture->Get_SourceSize(mCurrentAnimation.const_ref(), L_SourceSizeX, L_SourceSizeY);
		mCurrentSourceSize = v2f(L_SourceSizeX, L_SourceSizeY);
	}
}

void UIAnimatedSprite::Update(const Timer& _timer, void* addParam)
{
	double L_delta = ((Timer&)_timer).GetDt(this);

	if (mFrameNumber > 0)
	{
		mElpasedTime += L_delta;
		auto animationSpeed = 1.0f / mFramePerSecond;
		if (mElpasedTime >= animationSpeed)
		{
			int L_temp = (int)(mElpasedTime / animationSpeed);
			mCurrentFrame += L_temp;
			if ((bool)mLoop)
			{
				mCurrentFrame = mCurrentFrame % mFrameNumber;
			}
			else if (mCurrentFrame >= mFrameNumber)
			{
				mCurrentFrame = mFrameNumber - 1;
			}
			mElpasedTime -= animationSpeed * ((float)(L_temp));
		}
	}
	else
	{
		mElpasedTime = 0;
	}
}

void UIAnimatedSprite::SetVertexArray(UIVerticesInfo * aQI)
{
	aQI->Flag |= UIVerticesInfo_Vertex;
	aQI->Resize(4);
	VInfo2D::Data* buf = reinterpret_cast<VInfo2D::Data*>(aQI->Buffer());

	const SpriteSheetFrame * f = myTexture ? myTexture->Get_AnimFrame(mCurrentAnimation, mCurrentFrame) : nullptr;
	if (!f || !f->Trimmed)
	{
		ParentClassType::SetVertexArray(aQI);
		return;
	}

	Point2D pt[4] = 
	{
		v2f(f->Decal_X, f->Decal_Y),
		v2f( f->Decal_X, f->Decal_Y + f->FrameSize_Y ),
		v2f(f->Decal_X + f->FrameSize_X, f->Decal_Y + f->FrameSize_Y),
		v2f(f->Decal_X + f->FrameSize_X,	f->Decal_Y)
	};
	for (auto& p : pt) p *= mRealSize * v2f(1.0f / f->SourceSize_X, 1.0f / f->SourceSize_Y);

	TransformPoints(pt, 4);
	// triangle strip order
	buf[0].setVertex(pt[0].x, pt[0].y);
	buf[1].setVertex(pt[1].x, pt[1].y);
	buf[3].setVertex(pt[2].x, pt[2].y);
	buf[2].setVertex(pt[3].x, pt[3].y);
}

void UIAnimatedSprite::SetTexUV(UIVerticesInfo * aQI)
{
	if (myTexture && myTexture->Get_Texture())
	{
		v2f s, r;
		myTexture->Get_Texture()->GetSize(s.x, s.y);
		myTexture->Get_Texture()->GetRatio(r.x, r.y);
		s /= r;

		float dx = 0.5f / s.x;
		float dy = 0.5f / s.y;

		const SpriteSheetFrame * f = myTexture->Get_AnimFrame(mCurrentAnimation, mCurrentFrame);
		if (f == nullptr)
			return;

		bool is_bgr = false;
		if (myTexture->getValue("IsBGR", is_bgr) && is_bgr)
		{
			aQI->Flag |= UIVerticesInfo_BGRTexture;
		}

		v2f pMin(f->FramePos_X, f->FramePos_Y);
		pMin /= s;

		v2f pMax = f->Rotated ? v2f(f->FrameSize_Y, f->FrameSize_X) : v2f(f->FrameSize_X, f->FrameSize_Y);
		pMax /= s;
		pMax = pMin + pMax;

		VInfo2D::Data* buf = reinterpret_cast<VInfo2D::Data*>(aQI->Buffer());

		aQI->Flag |= UIVerticesInfo_Texture;
		// triangle strip order
		if (f->Rotated)
		{
			buf[1].setTexUV(pMin.x + dx, pMin.y + dy);
			buf[3].setTexUV(pMin.x + dx, pMax.y - dy);
			buf[2].setTexUV(pMax.x - dx, pMax.y - dy);
			buf[0].setTexUV(pMax.x - dx, pMin.y + dy);
		}
		else
		{

			buf[0].setTexUV(pMin.x + dx, pMin.y + dy);
			buf[1].setTexUV(pMin.x + dx, pMax.y - dy);
			buf[3].setTexUV(pMax.x - dx, pMax.y - dy);
			buf[2].setTexUV(pMax.x - dx, pMin.y + dy);
		}
	}
}

void UIAnimatedSprite::PreDraw(TravState* state)
{
	if (myTexture  &&myTexture->Get_Texture())
		myTexture->Get_Texture()->DoPreDraw(state);
}

void UIAnimatedSprite::PostDraw(TravState* state)
{
	if (myTexture &&myTexture->Get_Texture())
		myTexture->Get_Texture()->DoPostDraw(state);
}

int UIAnimatedSprite::GetTransparencyType()
{
	if (myTexture &&myTexture->Get_Texture() && GetOpacity() == 1.0f)
		return myTexture->Get_Texture()->GetTransparency();
	else // overall transparency
		return 2;
}