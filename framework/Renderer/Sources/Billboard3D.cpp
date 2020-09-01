#include "PrecompiledHeaders.h"

#include "Billboard3D.h"
#include "Core.h"
#include "TextureFileManager.h"
#include "SpriteSheetTexture.h"
#include "CoreBaseApplication.h"
#include "Timer.h"

IMPLEMENT_CLASS_INFO(Billboard3D)

Billboard3D::Billboard3D(const kstl::string& name,CLASS_NAME_TREE_ARG) : Drawable(name,PASS_CLASS_NAME_TREE_ARG)
, mTextureFileName(*this,false,"TextureFileName","")
, mTex(nullptr)
, mPosition(*this, false, "Position", 0.0f, 0.0f, 0.0f)
, mWidth(*this, false, "Width", 0.5f)
, mHeight(*this, false, "Height", 0.5f)
, mCamera(nullptr)
, mAnchor(*this, false, "Anchor", 0.0f, 0.0f)
, mTexture(nullptr)
, mCurrentAnimation(*this, false, "CurrentAnimation", "")
, mFramePerSecond(*this, false, "FramePerSecond", 0)
, mIsEnabled(*this, false, "IsEnabled", true)
, mIsSpriteSheet(*this, true, "IsSpriteSheet", false)
, mRatio(*this, false, "Ratio", 0.01f)
, mLoopAnimation(*this,false,"LoopAnimation",true)
, mReverse(*this,false,"Reverse",false)
{
	mColor[0] = 1.0f;
	mColor[1] = 1.0f;
	mColor[2] = 1.0f;
	mColor[3] = 1.0f;
	mHorizontalVector.Set(1,0,0);
	mVerticalVector.Set(0,0,1);
	mDistanceToCamera = FLT_MAX;
	mU1 = 0.0f;
	mU2 = 1.0f;
	mV1 = 0.0f;
	mV2 = 1.0f;
	
	setValue("TransarencyFlag", true);

	mCurrentFrame = 0;
	mFrameNumber = 0;
	mElpasedTime	= 0;
	mAnimationSpeed = 0.0f;
	mAnimationFinished = false;
}

Billboard3D::~Billboard3D()
{
	if(mIsSpriteSheet)
	{
		// remove this from auto update list in app
		CoreBaseApplication*	L_currentApp = KigsCore::GetCoreApplication();
		if(L_currentApp)
		{
			L_currentApp->RemoveAutoUpdate(this);
		}
	}
}

void Billboard3D::InitModifiable()
{
	Drawable::InitModifiable();

	if(IsInit())
	{
		kstl::string str;
		mTextureFileName.getValue(str);

		if (str!="")
		{
			auto& texfileManager = KigsCore::Singleton<TextureFileManager>();
			if(mIsSpriteSheet)
			{
				mTexture = texfileManager->GetSpriteSheetTexture(str);

				mFrameNumber = mTexture->Get_FrameNumber(mCurrentAnimation);
				mAnimationSpeed = 1.0f/(float)mFramePerSecond;

				if(!mReverse)
					mCurrentFrame = 0;
				else
					mCurrentFrame = mFrameNumber-1;

				//Auto Update
				CoreBaseApplication*	L_currentApp = KigsCore::GetCoreApplication();
				if(L_currentApp)
				{
					L_currentApp->AddAutoUpdate(this);
				}
				mCurrentAnimation.changeNotificationLevel(Owner);
			}
			else
			{
				mTex = texfileManager->GetTexture(mTextureFileName);

				float ratioX,ratioY;
				mTex->GetRatio(ratioX, ratioY);

				mU1 = 0.0f;
				mV1 = 0.0f;

				mU2 = ratioX;
				mV2 = ratioY;

				PrepareVertexBufferPos();
				PrepareVertexBufferTex();
				PrepareVertexBufferCol();
			}
		}
		mTextureFileName.changeNotificationLevel(Owner);

	}
}

void Billboard3D::NotifyUpdate(const unsigned int labelid)
{
	if (labelid == mTextureFileName.getLabelID())
	{
		auto& texfileManager = KigsCore::Singleton<TextureFileManager>();
		if (mIsSpriteSheet)
		{
			if (mTexture)
			{
				// check if changed
				kstl::string prevName;
				mTexture->getValue("FileName", prevName);
				if (prevName == mTextureFileName.const_ref())
				{
					Drawable::NotifyUpdate(labelid);
					return;
				}
				mTexture = 0;
			}
			if (mTextureFileName.const_ref() != "")
			{
				mTexture = texfileManager->GetSpriteSheetTexture(mTextureFileName.const_ref());

				mFrameNumber = mTexture->Get_FrameNumber(mCurrentAnimation.const_ref());
				mAnimationSpeed = 1.0f / (float)mFramePerSecond;

				if (!mReverse)
					mCurrentFrame = 0;
				else
					mCurrentFrame = mFrameNumber - 1;
			}
		}
		else
		{
			if (mTex)
			{
				kstl::string prevName;
				mTex->getValue("FileName", prevName);
				if (prevName == mTextureFileName.const_ref())
				{
					Drawable::NotifyUpdate(labelid);
					return;
				}
				mTex = nullptr;
			}
			
			if (mTextureFileName.const_ref() != "")
			{
				mTex = texfileManager->GetTexture(mTextureFileName.const_ref());

				float ratioX, ratioY;
				mTex->GetRatio(ratioX, ratioY);

				mU1 = 0.0f;
				mV1 = 0.0f;

				mU2 = KFLOAT_CONST(ratioX);
				mV2 = KFLOAT_CONST(ratioY);

				PrepareVertexBufferPos();
				PrepareVertexBufferTex();
				PrepareVertexBufferCol();
			}
			
		}
		
	}
	else if (labelid == mCurrentAnimation.getLabelID())
	{
		ChangeAnimation(mCurrentAnimation);
	}
	Drawable::NotifyUpdate(labelid);
}

void	Billboard3D::ChangeAnimation(const kstl::string& _newAnimation)
{
	
	if(mIsSpriteSheet && mTexture)
	{
		mFrameNumber = mTexture->Get_FrameNumber(_newAnimation);
		mCurrentAnimation = _newAnimation;

		mElpasedTime = 0;
		
		if(!mReverse)
			mCurrentFrame = 0;
		else
			mCurrentFrame = mFrameNumber-1;
		mAnimationFinished = false;
	}
}


void Billboard3D::Update(const Timer& _timer, void* addParam)
{
	double L_delta = ((Timer&)_timer).GetDt(this);

	if(mIsSpriteSheet && mIsEnabled)
	{
		if(mFrameNumber > 0)
		{
			mElpasedTime += L_delta;
			if(mElpasedTime >= mAnimationSpeed)
			{
				int L_temp = (int)(mElpasedTime/mAnimationSpeed);
				if(!mReverse)
				{
					mCurrentFrame += L_temp;
					unsigned int L_newFrame = mCurrentFrame % mFrameNumber;
					if(!mLoopAnimation && (mCurrentFrame > (int)L_newFrame))
					{
						mCurrentFrame = mFrameNumber-1;
						mAnimationFinished = true;
					}
					else
					{
						/*restart animation*/
						mAnimationFinished = false;
						mCurrentFrame = L_newFrame;
					}
				}
				else
				{
					mCurrentFrame -= L_temp;
					if(mCurrentFrame < 0)
					{
						mCurrentFrame = mFrameNumber-1;
						mAnimationFinished = true;
					}
					else
					{
						mAnimationFinished = false;
					}
				}


				mElpasedTime -= mAnimationSpeed*((float)(L_temp));
			}
		}
		else
		{
			mElpasedTime = 0;
		}
	}
	else
		mElpasedTime = 0;
}
void Billboard3D::SetPosition(const Point3D &Position)
{
	mPosition[0] = Position.x;
	mPosition[1] = Position.y;
	mPosition[2] = Position.z;
	PrepareVertexBufferPos();
}

void Billboard3D::SetSize(const float &Width, const float &Height)
{
	mWidth  = Width;
	mHeight = Height;
	PrepareVertexBufferPos();
}

void Billboard3D::SetTexCoord(const float &u1, const float &v1, const float &u2, const float &v2)
{
	mU1 = u1;
	mV1 = v1;
	mU2 = u2;
	mV2 = v2;
	PrepareVertexBufferTex();
}

void Billboard3D::SetPositionAndSize(const Point3D &Position, const float &Width, const float &Height)
{
	mPosition[0] = Position.x;
	mPosition[1] = Position.y;
	mPosition[2] = Position.z;
	mWidth = Width;
	mHeight = Height;
	PrepareVertexBufferPos();
}


void Billboard3D::SetColor(const float &R, const float &G, const float &B, const float &A)
{
	mColor[0] = R;
	mColor[1] = G;
	mColor[2] = B;
	mColor[3] = A;
	PrepareVertexBufferCol();
}

void Billboard3D::SetAlpha(const float &A)
{
	mColor[3] = A;
	PrepareVertexBufferCol();
}

bool Billboard3D::Draw(TravState* _state)
{
	if(Drawable::Draw(_state))
	{
		if(mCamera)
		{
			UpdateOrientation();
			return true;
		}
	}
	return false;
}

void Billboard3D::UpdateOrientation()
{
	const Matrix3x4 &Mat = mCamera->GetLocalToGlobal();
	mHorizontalVector.Set(-Mat.e[1][0],-Mat.e[1][1],-Mat.e[1][2]);
	mVerticalVector.Set(-Mat.e[2][0],-Mat.e[2][1],-Mat.e[2][2]);
	mHorizontalVector.Normalize();
	mVerticalVector.Normalize();
	mFatherNode->SetupNodeIfNeeded();

	//const Matrix3x4 &FatherMat = pFatherNode->GetLocalToGlobal();
	mFatherNode->GetGlobalToLocal().TransformVector(&mHorizontalVector);
	mFatherNode->GetGlobalToLocal().TransformVector(&mVerticalVector);
	PrepareVertexBufferPos();
}

