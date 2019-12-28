#pragma once

#include "CoreModifiable.h"
#include "AttributePacking.h"
#include "maNumeric.h"

#include <map>
#include <chrono>

using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;
using Duration = Clock::duration;

class Timer : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(Timer, CoreModifiable, Timer);
	DECLARE_INLINE_CONSTRUCTOR(Timer)
	, mTime(*this, false, "Time", "GetTime", "SetTime")
	{}
	WRAP_METHODS(GetTime,SetTime);

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

	State	GetState(){return myCurrentState;}
	virtual void SetState(State newstate);

	void GetDate(kstl::string& a_value, DateFormat a_format) const;
	void GetDate(unsigned int& a_year, unsigned int& a_month, unsigned int& a_day, unsigned int& a_hour, unsigned int& a_min, unsigned int& a_sec) const;
	
	double GetDt(const CheckUniqueObject& caller); 
	void ResetDt(const CheckUniqueObject& caller);
	
	void Sleep(unsigned int ms);

protected:
	void InitModifiable() override;
	
	State myCurrentState = UNINITIALISED;
	TimePoint myT0;
	TimePoint myPauseTime;
	
	void StartPause();
	void EndPause();

 	std::map<CheckUniqueObject, double> myTimerMap;

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
