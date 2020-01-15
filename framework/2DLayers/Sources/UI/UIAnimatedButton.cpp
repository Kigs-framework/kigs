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
, myUpTextureName(*this, false, LABEL_AND_ID(UpTextureName), "")
, myDownTextureName(*this, false, LABEL_AND_ID(DownTextureName), "")
, myOverTextureName(*this, false, LABEL_AND_ID(OverTextureName), "")
, myDefaultUpAnimation(*this, false, LABEL_AND_ID(DefaultUpAnimation), "")
, myDefaultDownAnimation(*this, false, LABEL_AND_ID(DefaultDownAnimation), "")
, myDefaultOverAnimation(*this, false, LABEL_AND_ID(DefaultOverAnimation), "")
, m_AnimationLoop(*this, false, LABEL_AND_ID(Loop), true)
, m_FramePerSecond(*this, false, LABEL_AND_ID(FramePerSecond), 0)
, m_CurrentFrame(0)
, m_FrameNumber(0)
{
	m_dElpasedTime	= 0;
	m_AnimationSpeed = 0.0f;
}

//-----------------------------------------------------------------------------------------------------
//Destructor

UIAnimatedButton::~UIAnimatedButton()
{
	if (myUpTexture)
		myUpTexture = NULL;
	if (myDownTexture)
		myDownTexture = NULL;
	if (myOverTexture)
		myOverTexture = NULL;
	if (myCurrentTexture)
		myCurrentTexture = NULL;

	if(myAlphaMask)
		myAlphaMask = NULL;
}

//-----------------------------------------------------------------------------------------------------
//NotifyUpdate

void UIAnimatedButton::NotifyUpdate(const unsigned int labelid)
{
	if(labelid==myIsEnabled.getLabelID()) 
	{
		if(!GetSons().empty())
		{
			kstl::set<Node2D*,Node2D::PriorityCompare> sons = GetSons();
			kstl::set<Node2D*,Node2D::PriorityCompare>::iterator it = sons.begin();
			kstl::set<Node2D*,Node2D::PriorityCompare>::iterator end = sons.end();
			while(it!=end)
			{
				(*it)->setValue(LABEL_TO_ID(IsEnabled),myIsEnabled);
				it++;
			}
		}
	}
	if (labelid == myDefaultUpAnimation.getLabelID() ||
		labelid == myDefaultDownAnimation.getLabelID() || 
		labelid == myDefaultOverAnimation.getLabelID()) {
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
		SP<TextureFileManager> textureManager = KigsCore::GetSingleton("TextureFileManager");
	
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
		LoadTexture(myUpTexture, myUpTextureName);
		LoadTexture(myDownTexture, myDownTextureName);
		LoadTexture(myOverTexture, myOverTextureName); 
		ChangeState();
		
		if (myUpTexture)
		{
			if (myDefaultUpAnimation.const_ref() != "")
			{
				//myCurrentAnimation = (const kstl::string&)myDefaultUpAnimation;
				m_FrameNumber = myUpTexture->Get_FrameNumber(myDefaultUpAnimation.const_ref());
			}

			// auto size
			if ((unsigned int)mySizeX == 0 && (unsigned int)mySizeY == 0 && myDefaultUpAnimation.const_ref() != "")
			{
				if (myUpTexture)
				{

					int L_SourceSizeX = 0;
					int L_SourceSizeY = 0;

					myUpTexture->Get_SourceSize(myDefaultUpAnimation.c_str(), L_SourceSizeX, L_SourceSizeY);
					mySizeX = (float)L_SourceSizeX;
					mySizeY = (float)L_SourceSizeY;


				}
			}

			m_AnimationSpeed = 1.0f / (float)m_FramePerSecond;
		}
		myDefaultUpAnimation.changeNotificationLevel(Owner);
		myDefaultDownAnimation.changeNotificationLevel(Owner);
		myDefaultOverAnimation.changeNotificationLevel(Owner);
	}
}


//-----------------------------------------------------------------------------------------------------
//isAlpha

bool UIAnimatedButton::isAlpha(float X, float Y)
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

//-----------------------------------------------------------------------------------------------------
//Update

void UIAnimatedButton::Update(const Timer& _timer, void* addParam)
{
	
	double L_delta = ((Timer&)_timer).GetDt(this);

	if(m_FrameNumber > 0)
	{
		m_dElpasedTime += L_delta;
		if(m_dElpasedTime >= m_AnimationSpeed)
		{
			int L_temp = (int)(m_dElpasedTime/m_AnimationSpeed);
			m_CurrentFrame += L_temp;
			if ((bool)m_AnimationLoop)
			{
				m_CurrentFrame = m_CurrentFrame % m_FrameNumber;
			}
			else if (m_CurrentFrame >= m_FrameNumber)
			{
				m_CurrentFrame =m_FrameNumber-1;
			}
			m_dElpasedTime -= m_AnimationSpeed*((float)(L_temp));
		}
	}
	else
	{
		m_dElpasedTime = 0;
	}
}

//-------------------------------------------------------------------------
//UpdateAnimatedValues

void	UIAnimatedButton::UpdateAnimatedValues() {
	kstl::string anim = myCurrentAnimation;
	if (myCurrentTexture && anim != "")
		m_FrameNumber = myCurrentTexture->Get_FrameNumber(myCurrentAnimation);
}




void UIAnimatedButton::ChangeState()
{
	if (myIsEnabled) // down and mouse over only when enabled
	{
		if (myIsDown)
		{
			if (myDownTexture)
			{
				myTexture = NonOwningRawPtrToSmartPtr(myDownTexture->Get_Texture());
				myCurrentTexture = myDownTexture;
				myCurrentAnimation = myDefaultDownAnimation.const_ref();
			return;
			}
		}

		if (myIsMouseOver)
		{
			if (myOverTexture)
			{
				myTexture = NonOwningRawPtrToSmartPtr(myOverTexture->Get_Texture());
				myCurrentTexture = myOverTexture;
				myCurrentAnimation = myDefaultOverAnimation.const_ref();
				return;
			}
		}
	}

	// set default color and texture
	myTexture = NonOwningRawPtrToSmartPtr(myUpTexture->Get_Texture());
	myCurrentTexture = myUpTexture;
	myCurrentAnimation = myDefaultUpAnimation.const_ref();
}

void UIAnimatedButton::SetTexUV(UIVerticesInfo * aQI)
{
	if (!myTexture.isNil())
	{
		Point2D s, r;
		myTexture->GetSize(s.x, s.y);
		myTexture->GetRatio(r.x, r.y);
		s /= r;

		kfloat dx = 0.5f / s.x;
		kfloat dy = 0.5f / s.y;

		const SpriteSheetFrame * f = myCurrentTexture->Get_AnimFrame(myCurrentAnimation, m_CurrentFrame);

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
	return myCurrentTexture.get();
}