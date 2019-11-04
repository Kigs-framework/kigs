#ifndef _MULTITOUCHPINCH_H_
#define _MULTITOUCHPINCH_H_

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
	bool b[2];
	bool mNeedUpdate;
};

#endif
