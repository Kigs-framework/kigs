#include "UI/UIAnimatedButton.h"
#include "TextureFileManager.h"
#include "SpriteSheetTexture.h"
#include "Texture.h"
#include "AlphaMask.h"
#include "CoreBaseApplication.h"
#include "Timer.h"
#include "NotificationCenter.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIAnimatedButton, UIAnimatedButton, 2DLayers);;
IMPLEMENT_CLASS_INFO(UIAnimatedButton)
//-----------------------------------------------------------------------------------------------------
//Constructor

UIAnimatedButton::UIAnimatedButton(const kstl::string& name, CLASS_NAME_TREE_ARG) :
UIButton(name, PASS_CLASS_NAME_TREE_ARG)
, mUpTexture(*this, false, LABEL_AND_ID(UpTextureName), "")
, mDownTextureName(*this, false, LABEL_AND_ID(DownTextureName), "")
, mOverTextureName(*this, false, LABEL_AND_ID(OverTextureName), "")
, mDefaultUpAnimation(*this, false, LABEL_AND_ID(DefaultUpAnimation), "")
, mDefaultDownAnimation(*this, false, LABEL_AND_ID(DefaultDownAnimation), "")
, mDefaultOverAnimation(*this, false, LABEL_AND_ID(DefaultOverAnimation), "")
, mLoop(*this, false, LABEL_AND_ID(Loop), true)
, mFramePerSecond(*this, false, LABEL_AND_ID(FramePerSecond), 0)
, mCurrentFrame(0)
, mFrameNumber(0)
{
	mElpasedTime	= 0;
	mAnimationSpeed = 0.0f;
}

//-----------------------------------------------------------------------------------------------------
//Destructor

UIAnimatedButton::~UIAnimatedButton()
{
	if (mUpTexturePointer)
		mUpTexturePointer = NULL;
	if (mDownTexturePointer)
		mDownTexturePointer = NULL;
	if (mOverTexturePointer)
		mOverTexturePointer = NULL;
	if (mCurrentTexture)
		mCurrentTexture = NULL;

	if(mAlphaMask)
		mAlphaMask = NULL;
}

//-----------------------------------------------------------------------------------------------------
//NotifyUpdate

void UIAnimatedButton::NotifyUpdate(const unsigned int labelid)
{
	if(labelid==mIsEnabled.getLabelID()) 
	{
		if(!GetSons().empty())
		{
			kstl::set<Node2D*,Node2D::PriorityCompare> sons = GetSons();
			kstl::set<Node2D*,Node2D::PriorityCompare>::iterator it = sons.begin();
			kstl::set<Node2D*,Node2D::PriorityCompare>::iterator end = sons.end();
			while(it!=end)
			{
				(*it)->setValue(LABEL_TO_ID(IsEnabled),mIsEnabled);
				it++;
			}
		}
	}
	if (labelid == mDefaultUpAnimation.getLabelID() ||
		labelid == mDefaultDownAnimation.getLabelID() || 
		labelid == mDefaultOverAnimation.getLabelID()) {
		UpdateAnimatedValues();
	}
	UIButton::NotifyUpdate(labelid);
}

//-----------------------------------------------------------------------------------------------------
//LoadTexture

void	UIAnimatedButton::LoadTexture(SmartPointer<SpriteSheetTexture>& a_textptr, const kstl::string& a_filename)
{
	if (a_filename != "")
	{
		// load texture
		auto& textureManager = KigsCore::Singleton<TextureFileManager>();
	
		a_textptr = textureManager->GetSpriteSheetTexture(a_filename);
	}
}

//-----------------------------------------------------------------------------------------------------
//InitModifiable

void UIAnimatedButton::InitModifiable()
{
	UIButton::InitModifiable();
	
	if(_isInit)
	{
		LoadTexture(mUpTexturePointer, mUpTexture);
		LoadTexture(mDownTexturePointer, mDownTextureName);
		LoadTexture(mOverTexturePointer, mOverTextureName); 
		ChangeState();
		
		if (mUpTexturePointer)
		{
			if (mDefaultUpAnimation.const_ref() != "")
			{
				mFrameNumber = mUpTexturePointer->Get_FrameNumber(mDefaultUpAnimation.const_ref());
			}

			// auto size
			if ((unsigned int)mSizeX == 0 && (unsigned int)mSizeY == 0 && mDefaultUpAnimation.const_ref() != "")
			{
				if (mUpTexturePointer)
				{

					int L_SourceSizeX = 0;
					int L_SourceSizeY = 0;

					mUpTexturePointer->Get_SourceSize(mDefaultUpAnimation.c_str(), L_SourceSizeX, L_SourceSizeY);
					mSizeX = (float)L_SourceSizeX;
					mSizeY = (float)L_SourceSizeY;


				}
			}

			mAnimationSpeed = 1.0f / (float)mFramePerSecond;
		}
		mDefaultUpAnimation.changeNotificationLevel(Owner);
		mDefaultDownAnimation.changeNotificationLevel(Owner);
		mDefaultOverAnimation.changeNotificationLevel(Owner);
	}
}


//-----------------------------------------------------------------------------------------------------
//isAlpha

bool UIAnimatedButton::isAlpha(float X, float Y)
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

//-----------------------------------------------------------------------------------------------------
//Update

void UIAnimatedButton::Update(const Timer& _timer, void* addParam)
{
	
	double L_delta = ((Timer&)_timer).GetDt(this);

	if(mFrameNumber > 0)
	{
		mElpasedTime += L_delta;
		if(mElpasedTime >= mAnimationSpeed)
		{
			int L_temp = (int)(mElpasedTime/mAnimationSpeed);
			mCurrentFrame += L_temp;
			if ((bool)mLoop)
			{
				mCurrentFrame = mCurrentFrame % mFrameNumber;
			}
			else if (mCurrentFrame >= mFrameNumber)
			{
				mCurrentFrame =mFrameNumber-1;
			}
			mElpasedTime -= mAnimationSpeed*((float)(L_temp));
		}
	}
	else
	{
		mElpasedTime = 0;
	}
}

//-------------------------------------------------------------------------
//UpdateAnimatedValues

void	UIAnimatedButton::UpdateAnimatedValues() {
	kstl::string anim = mCurrentAnimation;
	if (mCurrentTexture && anim != "")
		mFrameNumber = mCurrentTexture->Get_FrameNumber(mCurrentAnimation);
}




void UIAnimatedButton::ChangeState()
{
	if (mIsEnabled) // down and mouse over only when enabled
	{
		if (mIsDown)
		{
			if (mDownTexturePointer)
			{
				mTexturePointer = NonOwningRawPtrToSmartPtr(mDownTexturePointer->Get_Texture());
				mCurrentTexture = mDownTexturePointer;
				mCurrentAnimation = mDefaultDownAnimation.const_ref();
			return;
			}
		}

		if (mIsMouseOver)
		{
			if (mOverTexturePointer)
			{
				mTexturePointer = NonOwningRawPtrToSmartPtr(mOverTexturePointer->Get_Texture());
				mCurrentTexture = mOverTexturePointer;
				mCurrentAnimation = mDefaultOverAnimation.const_ref();
				return;
			}
		}
	}

	// set default color and texture
	mTexturePointer = NonOwningRawPtrToSmartPtr(mUpTexturePointer->Get_Texture());
	mCurrentTexture = mUpTexturePointer;
	mCurrentAnimation = mDefaultUpAnimation.const_ref();
}

void UIAnimatedButton::SetTexUV(UIVerticesInfo * aQI)
{
	if (!mTexturePointer.isNil())
	{
		Point2D s, r;
		mTexturePointer->GetSize(s.x, s.y);
		mTexturePointer->GetRatio(r.x, r.y);
		s /= r;

		kfloat dx = 0.5f / s.x;
		kfloat dy = 0.5f / s.y;

		const SpriteSheetFrame * f = mCurrentTexture->Get_AnimFrame(mCurrentAnimation, mCurrentFrame);

		Point2D pMin((kfloat)f->FramePos_X, (kfloat)f->FramePos_Y);
		pMin /= s;

		Point2D pMax((kfloat)f->FrameSize_X, (kfloat)f->FrameSize_Y);
		pMax /= s;
		pMax = pMin + pMax;

		VInfo2D::Data* buf = reinterpret_cast<VInfo2D::Data*>(aQI->Buffer());

		aQI->Flag |= UIVerticesInfo_Texture;
		// triangle strip order
		buf[0].setTexUV(pMin.x + dx, pMin.y + dy);
		buf[1].setTexUV(pMin.x + dx, pMax.y - dy);
		buf[3].setTexUV(pMax.x - dx, pMax.y - dy);
		buf[2].setTexUV(pMax.x - dx, pMin.y + dy);
	}
}

SpriteSheetTexture* UIAnimatedButton::GetSpriteSheetTexture()
{
	return mCurrentTexture.get();
}