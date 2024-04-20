#include "PrecompiledHeaders.h"

#include "Billboard3D.h"
#include "Core.h"
#include "TextureFileManager.h"
#include "CoreBaseApplication.h"
#include "Timer.h"

using namespace Kigs::Draw;

IMPLEMENT_CLASS_INFO(Billboard3D)

Billboard3D::Billboard3D(const std::string& name,CLASS_NAME_TREE_ARG) : Drawable(name,PASS_CLASS_NAME_TREE_ARG)
, mCamera(nullptr)
, mHorizontalVector(1.0f, 0.0f, 0.0f)
, mVerticalVector(0.0f, 0.0f, 1.0f)
, mColor{1.0,1.0, 1.0, 1.0}
, mDistanceToCamera(FLT_MAX)
{

	mU1 = 0.0f;
	mU2 = 1.0f;
	mV1 = 0.0f;
	mV2 = 1.0f;
	
	setValue("TransarencyFlag", true);

}

Billboard3D::~Billboard3D()
{
	
}


void Billboard3D::SetPosition(const v3f &Position)
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

void Billboard3D::SetPositionAndSize(const v3f &Position, const float &Width, const float &Height)
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
	const mat4 &Mat = mCamera->GetLocalToGlobal();
	mHorizontalVector = glm::normalize(v3f( -Mat[1][0], -Mat[1][1], -Mat[1][2] ));
	mVerticalVector = glm::normalize(v3f(-Mat[2][0], -Mat[2][1], -Mat[2][2]));
	mFatherNode->SetupNodeIfNeeded();

	//const mat4 &FatherMat = pFatherNode->GetLocalToGlobal();
	mHorizontalVector = transformVector3(mFatherNode->GetGlobalToLocal() ,mHorizontalVector);
	mVerticalVector = transformVector3(mFatherNode->GetGlobalToLocal(),mVerticalVector);
	PrepareVertexBufferPos();
}

