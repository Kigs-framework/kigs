#include "PrecompiledHeaders.h"

#include "Billboard3D.h"
#include "Core.h"
#include "TextureFileManager.h"
#include "SpriteSheetTexture.h"
#include "CoreBaseApplication.h"
#include "Timer.h"

IMPLEMENT_CLASS_INFO(Billboard3D)

Billboard3D::Billboard3D(const kstl::string& name,CLASS_NAME_TREE_ARG) : Drawable(name,PASS_CLASS_NAME_TREE_ARG)
, m_TextureFileName(*this,false,"TextureFileName","")
, m_Position(*this, false, "Position", 0.0f, 0.0f, 0.0f)
, m_Width(*this, false, "Width", 0.5f)
, m_Height(*this, false, "Height", 0.5f)
, myCamera(nullptr)
, myAnchor(*this, false, "Anchor", 0.0f, 0.0f)
, m_CurrentAnimation(*this, false, "CurrentAnimation", "")
, m_FramePerSecond(*this, false, "FramePerSecond", 0)
, m_IsEnabled(*this, false, "IsEnabled", true)
, m_IsSpriteSheet(*this, true, "IsSpriteSheet", false)
, m_ratio(*this, false, "ratio", 0.01f)
, m_bLoopAnimation(*this,false,"LoopAnimation",true)
, m_bReverse(*this,false,"Reverse",false)
{
	m_Color[0] = 1.0f;
	m_Color[1] = 1.0f;
	m_Color[2] = 1.0f;
	m_Color[3] = 1.0f;
	m_HorizontalVector.Set(1,0,0);
	m_VerticalVector.Set(0,0,1);
	m_DistanceToCamera = FLT_MAX;
	m_Tex = nullptr;
	m_u1 = 0.0f;
	m_u2 = 1.0f;
	m_v1 = 0.0f;
	m_v2 = 1.0f;
	
	setValue("TransarencyFlag", true);

	m_CurrentFrame = 0;
	m_FrameNumber = 0;
	m_dElpasedTime	= 0;
	m_AnimationSpeed = 0.0f;
	m_bAnimationFinished = false;
}

Billboard3D::~Billboard3D()
{
	if(m_IsSpriteSheet)
	{
		// remove this from auto update list in app
		CoreBaseApplication*	L_currentApp = KigsCore::GetCoreApplication();
		if(L_currentApp)
		{
			L_currentApp->RemoveAutoUpdate(this);
		}
	}

	if (m_Tex)
		m_Tex->Destroy();
	if(myTexture)
		myTexture = nullptr;
}

void Billboard3D::InitModifiable()
{
	Drawable::InitModifiable();

	if(IsInit())
	{
		kstl::string str;
		m_TextureFileName.getValue(str);

		if (str!="")
		{
			TextureFileManager*	fileManager=(TextureFileManager*)KigsCore::GetSingleton("TextureFileManager");
			if(m_IsSpriteSheet)
			{
				if(myTexture)
					myTexture = 0;
				myTexture = OwningRawPtrToSmartPtr(fileManager->GetSpriteSheetTexture(str));

				m_FrameNumber = myTexture->Get_FrameNumber(m_CurrentAnimation);
				m_AnimationSpeed = 1.0f/(float)m_FramePerSecond;

				if(!m_bReverse)
					m_CurrentFrame = 0;
				else
					m_CurrentFrame = m_FrameNumber-1;

				//Auto Update
				CoreBaseApplication*	L_currentApp = KigsCore::GetCoreApplication();
				if(L_currentApp)
				{
					L_currentApp->AddAutoUpdate(this);
				}
				m_CurrentAnimation.changeNotificationLevel(Owner);
			}
			else
			{
				m_Tex = fileManager->GetTexture(m_TextureFileName);

				float ratioX,ratioY;
				m_Tex->GetRatio(ratioX, ratioY);

				m_u1 = 0.0f;
				m_v1 = 0.0f;

				m_u2 = ratioX;
				m_v2 = ratioY;

				PrepareVertexBufferPos();
				PrepareVertexBufferTex();
				PrepareVertexBufferCol();
			}
		}
		m_TextureFileName.changeNotificationLevel(Owner);

	}
}

void Billboard3D::NotifyUpdate(const unsigned int labelid)
{
	if (labelid == m_TextureFileName.getLabelID())
	{
		TextureFileManager*	fileManager = (TextureFileManager*)KigsCore::GetSingleton("TextureFileManager");
		if (m_IsSpriteSheet)
		{
			if (myTexture)
			{
				// check if changed
				kstl::string prevName;
				myTexture->getValue("FileName", prevName);
				if (prevName == m_TextureFileName.const_ref())
				{
					Drawable::NotifyUpdate(labelid);
					return;
				}
				myTexture = 0;
			}
			if (m_TextureFileName.const_ref() != "")
			{
				myTexture = OwningRawPtrToSmartPtr(fileManager->GetSpriteSheetTexture(m_TextureFileName.const_ref()));

				m_FrameNumber = myTexture->Get_FrameNumber(m_CurrentAnimation.const_ref());
				m_AnimationSpeed = 1.0f / (float)m_FramePerSecond;

				if (!m_bReverse)
					m_CurrentFrame = 0;
				else
					m_CurrentFrame = m_FrameNumber - 1;
			}
		}
		else
		{
			if (m_Tex)
			{
				kstl::string prevName;
				m_Tex->getValue("FileName", prevName);
				if (prevName == m_TextureFileName.const_ref())
				{
					Drawable::NotifyUpdate(labelid);
					return;
				}
				m_Tex->Destroy();
				m_Tex = 0;
			}
			
			if (m_TextureFileName.const_ref() != "")
			{
				m_Tex = fileManager->GetTexture(m_TextureFileName.const_ref());

				float ratioX, ratioY;
				m_Tex->GetRatio(ratioX, ratioY);

				m_u1 = 0.0f;
				m_v1 = 0.0f;

				m_u2 = KFLOAT_CONST(ratioX);
				m_v2 = KFLOAT_CONST(ratioY);

				PrepareVertexBufferPos();
				PrepareVertexBufferTex();
				PrepareVertexBufferCol();
			}
			
		}
		
	}
	else if (labelid == m_CurrentAnimation.getLabelID())
	{
		ChangeAnimation(m_CurrentAnimation);
	}
	Drawable::NotifyUpdate(labelid);
}

void	Billboard3D::ChangeAnimation(const kstl::string& _newAnimation)
{
	
	if(m_IsSpriteSheet && myTexture)
	{
		m_FrameNumber = myTexture->Get_FrameNumber(_newAnimation);
		m_CurrentAnimation = _newAnimation;

		m_dElpasedTime = 0;
		
		if(!m_bReverse)
			m_CurrentFrame = 0;
		else
			m_CurrentFrame = m_FrameNumber-1;
		m_bAnimationFinished = false;
	}
}


void Billboard3D::Update(const Timer& _timer, void* addParam)
{
	double L_delta = ((Timer&)_timer).GetDt(this);

	if(m_IsSpriteSheet && m_IsEnabled)
	{
		if(m_FrameNumber > 0)
		{
			m_dElpasedTime += L_delta;
			if(m_dElpasedTime >= m_AnimationSpeed)
			{
				int L_temp = (int)(m_dElpasedTime/m_AnimationSpeed);
				if(!m_bReverse)
				{
					m_CurrentFrame += L_temp;
					unsigned int L_newFrame = m_CurrentFrame % m_FrameNumber;
					if(!m_bLoopAnimation && (m_CurrentFrame > (int)L_newFrame))
					{
						m_CurrentFrame = m_FrameNumber-1;
						m_bAnimationFinished = true;
					}
					else
					{
						/*restart animation*/
						m_bAnimationFinished = false;
						m_CurrentFrame = L_newFrame;
					}
				}
				else
				{
					m_CurrentFrame -= L_temp;
					if(m_CurrentFrame < 0)
					{
						m_CurrentFrame = m_FrameNumber-1;
						m_bAnimationFinished = true;
					}
					else
					{
						m_bAnimationFinished = false;
					}
				}


				m_dElpasedTime -= m_AnimationSpeed*((float)(L_temp));
			}
		}
		else
		{
			m_dElpasedTime = 0;
		}
	}
	else
		m_dElpasedTime = 0;
}
void Billboard3D::SetPosition(const Point3D &Position)
{
	m_Position[0] = Position.x;
	m_Position[1] = Position.y;
	m_Position[2] = Position.z;
	PrepareVertexBufferPos();
}

void Billboard3D::SetSize(const float &Width, const float &Height)
{
	m_Width  = Width;
	m_Height = Height;
	PrepareVertexBufferPos();
}

void Billboard3D::SetTexCoord(const float &u1, const float &v1, const float &u2, const float &v2)
{
	m_u1 = u1;
	m_v1 = v1;
	m_u2 = u2;
	m_v2 = v2;
	PrepareVertexBufferTex();
}

void Billboard3D::SetPositionAndSize(const Point3D &Position, const float &Width, const float &Height)
{
	m_Position[0] = Position.x;
	m_Position[1] = Position.y;
	m_Position[2] = Position.z;
	m_Width = Width;
	m_Height = Height;
	PrepareVertexBufferPos();
}


void Billboard3D::SetColor(const float &R, const float &G, const float &B, const float &A)
{
	m_Color[0] = R;
	m_Color[1] = G;
	m_Color[2] = B;
	m_Color[3] = A;
	PrepareVertexBufferCol();
}

void Billboard3D::SetAlpha(const float &A)
{
	m_Color[3] = A;
	PrepareVertexBufferCol();
}

bool Billboard3D::Draw(TravState* _state)
{
	if(Drawable::Draw(_state))
	{
		if(myCamera)
		{
			UpdateOrientation();
			return true;
		}
	}
	return false;
}

void Billboard3D::UpdateOrientation()
{
	const Matrix3x4 &Mat = myCamera->GetLocalToGlobal();
	m_HorizontalVector.Set(-Mat.e[1][0],-Mat.e[1][1],-Mat.e[1][2]);
	m_VerticalVector.Set(-Mat.e[2][0],-Mat.e[2][1],-Mat.e[2][2]);
	m_HorizontalVector.Normalize();
	m_VerticalVector.Normalize();
	pFatherNode->SetupNodeIfNeeded();

	//const Matrix3x4 &FatherMat = pFatherNode->GetLocalToGlobal();
	pFatherNode->GetGlobalToLocal().TransformVector(&m_HorizontalVector);
	pFatherNode->GetGlobalToLocal().TransformVector(&m_VerticalVector);
	PrepareVertexBufferPos();
}

