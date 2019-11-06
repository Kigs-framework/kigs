#include "PrecompiledHeaders.h"
#include "MouseVelocityComputer.h"
#include "Core.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(MouseVelocityComputer, MouseVelocityComputer, 2DLayers);
IMPLEMENT_CLASS_INFO(MouseVelocityComputer)

MouseVelocityComputer::MouseVelocityComputer(const kstl::string& name, CLASS_NAME_TREE_ARG) :
CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG),
mVelocityIndex(0),
mVelocityCount(0),
mOldTime(0)
{

	mVelocityX = new kfloat[cMaxIndex];
	mVelocityY = new kfloat[cMaxIndex];
	for (int i = 0; i < cMaxIndex; i++)
	{
		mVelocityX[i] = 0.f;
		mVelocityY[i] = 0.f;
	}
}

MouseVelocityComputer::~MouseVelocityComputer()
{
	delete[] mVelocityX;
	delete[] mVelocityY;
}


void MouseVelocityComputer::Init(kdouble time)
{
	mOldTime = time;
	mVelocityIndex = 0;
	mVelocityCount = 0;
}

void MouseVelocityComputer::StoreDisplacement(kfloat dx, kfloat dy,kdouble time)
{
	kdouble dt = time - mOldTime;
	if (dt > 0)
	{
		if (mVelocityCount < cMaxIndex)
		{
			mVelocityX[mVelocityCount] = dx / (float)dt;
			mVelocityY[mVelocityCount] = dy / (float)dt;
			mVelocityCount++;
		}
		else
		{
			mVelocityX[mVelocityIndex] = dx / (float)dt;
			mVelocityY[mVelocityIndex] = dy / (float)dt;
			mVelocityIndex = (mVelocityIndex + 1) % cMaxIndex;
		}
		mOldTime = time;
	}
}



void MouseVelocityComputer::ComputeVelocity(kfloat &vx, kfloat &vy)
{
	vx = 0.f;
	vy = 0.f;
	if (mVelocityCount)
	{
		for (int i = 0; i < mVelocityCount; i++)
		{
			vx += mVelocityX[i];
			vy += mVelocityY[i];
		}
		vx /= (kfloat)mVelocityCount;
		vy /= (kfloat)mVelocityCount;
	}
}
