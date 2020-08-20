#pragma once

#include "CoreModifiable.h"
#include "AttributePacking.h"
#include "maNumeric.h"

#include <map>
#include <chrono>
#include <functional>

using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;
using Duration = Clock::duration;

// ****************************************
// * TimeProfiler class
// * --------------------------------------
/**
 * \class	Timer
 * \file	Timer.h
 * \ingroup TimerModule
 * \brief	Mesure time.
 *
 */
 // ****************************************

class Timer : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(Timer, CoreModifiable, Timer);
	DECLARE_INLINE_CONSTRUCTOR(Timer)
	, mTime(*this, true, "Time", "GetTime", "")
	{}
	WRAP_METHODS(GetTime);

	enum DateFormat
	{
		DAY_MONTH_YEAR = 0,
		YEAR_MONTH_DAY,
		DAY_MONTH_YEAR_HOURS_MINS,
		YEAR_MONTH_DAY_HOURS_MINS,
		DAY_MONTH_YEAR_HOURS_MINS_SEC,
		YEAR_MONTH_DAY_HOURS_MINS_SEC,
		_DAY_MONTH_YEAR_HOURS_MINS_SEC,
		_YEAR_MONTH_DAY_HOURS_MINS_SEC
	};

	enum State
	{
		UNINITIALISED = 0,
		NORMAL,
		PAUSED
	};

	virtual double GetTime() const;
	virtual void SetTime(double t);

	State	GetState(){return mCurrentState;}
	virtual void SetState(State newstate);

	void GetDate(kstl::string& a_value, DateFormat a_format) const;
	void GetDate(unsigned int& a_year, unsigned int& a_month, unsigned int& a_day, unsigned int& a_hour, unsigned int& a_min, unsigned int& a_sec) const;
	
	double GetDt(CoreModifiable* caller);
	void ResetDt(CoreModifiable* caller);
	
	void Sleep(unsigned int ms);

protected:
	void InitModifiable() override;
	
	State mCurrentState = UNINITIALISED;
	TimePoint mT0;
	TimePoint mPauseTime;
	
	void StartPause();
	void EndPause();

 	std::map<CoreModifiable*, double> mTimerMap;

	maComputedNumeric<double>	mTime;
};


class DeltaTimeHelper
{
	double last_time = DBL_MAX;
public:
	void Reset() { last_time = DBL_MAX; }
	double Update(const Timer& timer)
	{
		auto t = timer.GetTime();
		if (last_time == DBL_MAX) last_time = t;
		auto dt = t - last_time;
		last_time = t;
		return dt;
	}
};

struct DelayedExecutor
{
	template<typename F>
	void ExecuteIn(F&& func, Duration d)
	{
		mFunc = FWD(func);
		mExecuteTime = TimePoint::clock::now() + d;
	}
	void Cancel()
	{
		mFunc = {};
	}
	bool IsActive() const { return (bool)mFunc; }
	void Update()
	{
		if (mFunc)
		{
			if (TimePoint::clock::now() >= mExecuteTime)
			{
				mFunc();
				mFunc = {};
			}
		}
	}
private:
	std::function<void()> mFunc;
	TimePoint mExecuteTime;
};