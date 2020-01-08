#pragma once

#include "Timer.h"

class ControlledTimer : public Timer
{
public:
	DECLARE_CLASS_INFO(ControlledTimer,Timer, Timer)

	DECLARE_INLINE_CONSTRUCTOR(ControlledTimer)
	{
		myCurrentTime = 0;
	}
	
	double	GetTime() const override
	{
		return myCurrentTime;
	}

	void SetTime(double t) override
	{
		myCurrentTime = t;
	}

protected:
	double myCurrentTime;
};

