#include "PrecompiledHeaders.h"
#include "MultiTouchPinch.h"
#include "CoreModifiable.h"
#include <math.h>

MultiTouchPinch::MultiTouchPinch() :
mDx(0.f)
, mDy(0.f)
, mDZ(0.f)
{
	release(0);
	release(1);
	mNeedUpdate = false;
}

void MultiTouchPinch::set(int index, float x, float y)
{
	if (index >= 0 && index <= 1)
	{		
		if (mB[index] == false)
		{
			mOx[index] = x;
			mOy[index] = y;
			mX[index] = x;
			mY[index] = y;
			mNeedUpdate = true;
			mB[index] = true;
		}
		else
		{
			if (mX[index] != x || mY[index] != y)
			{
				mOx[index] = mX[index];
				mOy[index] = mY[index];
				mX[index] = x;
				mY[index] = y;
				mNeedUpdate = true;
			}
		}
	}
}

bool MultiTouchPinch::update()
{
	if (mNeedUpdate && mB[0] && mB[1])
	{
		float COX = (mOx[0] + mOx[1]) / 2.f;
		float CNX = (mX[0] + mX[1]) / 2.f;
		float COY = (mOy[0] + mOy[1]) / 2.f;
		float CNY = (mY[0] + mY[1]) / 2.f;
		float DOX = mOx[1] - mOx[0];
		float DNX = mX[1] - mX[0];
		float DOY = mOy[1] - mOy[0];
		float DNY = mY[1] - mY[0];

		float OD = sqrtf(DOX*DOX + DOY*DOY);
		float ND = sqrtf(DNX*DNX + DNY*DNY);

		float dx = CNX - COX;
		float dy = CNY - COY;
		float z = ND - OD;

		if (dx != mDx || dy != mDy || z != mDZ)
		{
			mDx = dx;
			mDy = dy;
			mDZ = z;		
			return true;
		}
	}
	mNeedUpdate = false;
	return false;
}

void MultiTouchPinch::release(int index)
{
	if (index >= 0 && index <= 1)
	{
		mB[index] = false;
	}
}





