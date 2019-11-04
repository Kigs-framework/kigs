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
	if (labelid == myIsEnabled.getLabelID())
	{
		if (!GetSons().empty())
		{
			std::set<Node2D*, Node2D::PriorityCompare> sons = GetSons();
			std::set<Node2D*, Node2D::PriorityCompare>::iterator it = sons.begin();
			std::set<Node2D*, Node2D::PriorityCompare>::iterator end = sons.end();
			while (it != end)
			{
				(*it)->setValue("IsEnabled", myIsEnabled);
				it++;
			}
		}
	}
	else if (labelid == myCurrentAnimation.getLabelID())
	{
		ChangeAnimation(myCurrentAnimation.c_str());
	}
	else if (labelid == myFileName.getID())
	{
		ChangeSpriteSheet(myFileName);
	}
	UIDrawableItem::NotifyUpdate(labelid);
}

void UIAnimatedSprite::InitModifiable()
{
	UIDrawableItem::InitModifiable();

	if (IsInit())
	{
		if (myFileName.const_ref() != "")
		{
			ChangeSpriteSheet(myFileName);
		}
		myIsEnabled.changeNotificationLevel(Owner);
		myCurrentAnimation.changeNotificationLevel(Owner);
		myFileName.changeNotificationLevel(Owner);
	}
}

void UIAnimatedSprite::ComputeRealSize()
{
	ParentClassType::ComputeRealSize();
	if (myTexture)
	{
		if (mySizeX == 0 && mySizeModeX == DEFAULT)
			myRealSize.x = mCurrentSourceSize.x;
		if (mySizeY == 0 && mySizeModeY == DEFAULT)
			myRealSize.y = mCurrentSourceSize.y;
	}
}

void UIAnimatedSprite::ChangeSpriteSheet(std::string const& filename)
{
	TextureFileManager* textureManager = KigsCore::GetSingleton<TextureFileManager>();
	myFileName = filename;
	myTexture = OwningRawPtrToSmartPtr(textureManager->GetSpriteSheetTexture(filename));
	ChangeAnimation(myCurrentAnimation.c_str());
}

SpriteSheetTexture* UIAnimatedSprite::GetSpriteSheetTexture()
{
	return myTexture.get();
}

bool UIAnimatedSprite::isAlpha(float X, float Y)
{
	//Try to get my mask
	if (!myAlphaMask)
	{
		std::vector<ModifiableItemStruct> sons = getItems();

		for (unsigned int i = 0; i < sons.size(); i++)
		{
			if (sons[i].myItem->isSubType("AlphaMask"))
			{
				myAlphaMask = (AlphaMask*)sons[i].myItem;
				break;
			}
		}
	}

	if (myAlphaMask)
	{
		//Check on my mask the specified location
		return !myAlphaMask->CheckTo(X, Y);
	}

	return false;
}

void UIAnimatedSprite::ChangeAnimation(const std::string& _newAnimation)
{
	if (myTexture)
	{
		m_FrameNumber = myTexture->Get_FrameNumber(_newAnimation);
		myCurrentAnimation = _newAnimation;
		m_CurrentFrame = 0;
		m_dElpasedTime = 0;

		int L_SourceSizeX = 0;
		int L_SourceSizeY = 0;
		myTexture->Get_SourceSize(myCurrentAnimation.const_ref(), L_SourceSizeX, L_SourceSizeY);
		mCurrentSourceSize = v2f(L_SourceSizeX, L_SourceSizeY);
	}
}

void UIAnimatedSprite::Update(const Timer& _timer, void* addParam)
{
	double L_delta = ((Timer&)_timer).GetDt(this);

	if (m_FrameNumber > 0)
	{
		m_dElpasedTime += L_delta;
		auto animationSpeed = 1.0f / m_FramePerSecond;
		if (m_dElpasedTime >= animationSpeed)
		{
			int L_temp = (int)(m_dElpasedTime / animationSpeed);
			m_CurrentFrame += L_temp;
			if ((bool)m_AnimationLoop)
			{
				m_CurrentFrame = m_CurrentFrame % m_FrameNumber;
			}
			else if (m_CurrentFrame >= m_FrameNumber)
			{
				m_CurrentFrame = m_FrameNumber - 1;
			}
			m_dElpasedTime -= animationSpeed * ((float)(L_temp));
		}
	}
	else
	{
		m_dElpasedTime = 0;
	}
}

void UIAnimatedSprite::SetVertexArray(UIVerticesInfo * aQI)
{
	aQI->Flag |= UIVerticesInfo_Vertex;
	aQI->Resize(4);
	VInfo2D::Data* buf = reinterpret_cast<VInfo2D::Data*>(aQI->Buffer());

	const SpriteSheetFrame * f = myTexture ? myTexture->Get_AnimFrame(myCurrentAnimation, m_CurrentFrame) : nullptr;
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
	for (auto& p : pt) p *= myRealSize * v2f(1.0f / f->SourceSize_X, 1.0f / f->SourceSize_Y);

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

		const SpriteSheetFrame * f = myTexture->Get_AnimFrame(myCurrentAnimation, m_CurrentFrame);
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
	if (myTexture &&myTexture->Get_Texture() && myOpacity == 1.0f)
		return myTexture->Get_Texture()->GetTransparency();
	else // overall transparency
		return 2;
}