#ifndef _MULTITOUCHPINCH_H_
#define _MULTITOUCHPINCH_H_

// ****************************************
// * MultiTouchPinch class
// * --------------------------------------
/**
* \file	MultiTouchPinch.h
* \class	MultiTouchPinch
* \ingroup 2DLayers
* \brief	Manage pinch with two touches.
*
*  Obsolete ?
*/
// ****************************************

class MultiTouchPinch
{
public:
	MultiTouchPinch();
	void init(int index, float x, float y);
	void set(int index, float x, float y);
	void release(int index);
	bool update();

private:
	float mOx[2];
	float mOy[2];
	float mX[2];
	float mY[2];

public:
	float mDx, mDy, mDZ;
	bool mB[2];
	bool mNeedUpdate;
};

#endif
