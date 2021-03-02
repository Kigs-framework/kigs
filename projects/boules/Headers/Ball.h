#pragma once

#include "CoreModifiable.h"
#include "Wall.h"


// manage a ball
class Ball
{
protected:
	float	mR = 1.0f;	// radius
	float   mM = 1.0f;  // mass
	v2f		mP0 = { 0.0f,0.0f }; // initial pos
	v2f		mSpeed = { 0.0f,0.0f }; // constant speed

	// manage ball display
	CMSP	mGraphicBall;

	// the time the last "reset occurs"
	double	mLastResetTime = 0.0;

public:

	// return time to the next collision with the other ball or -1 if no collision found in the future 
	double	getCollisionTimeWithOther(const Ball& other);
	// return time to the next collision with the given wall
	double	getCollisionTimeWithWall(const Wall& other);

	// update display pos
	void	Update(double time);

	Ball(float r,float m) : mR(r), mM(m)
	{

	}

	// getters & setters
	void	SetPos(const v2f& p)
	{
		mP0 = p;
	}

	v2f GetPos(double t = 0.0) const
	{
		return (mP0 + (t - mLastResetTime) * mSpeed);
	}

	float	GetRadius()
	{
		return mR;
	}

	float	GetMass()
	{
		return mM;
	}

	void SetSpeed(const v2f& s)
	{
		mSpeed = s;
	}

	v2f GetSpeed() const
	{
		return mSpeed;
	}

	void	SetUI(CMSP ui)
	{
		mGraphicBall = ui;
	}

	void	ResetTime(double t)
	{
		mLastResetTime = t;
	}
};
