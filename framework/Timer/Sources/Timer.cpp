#include "PrecompiledHeaders.h"
#include "Timer.h"

#include <thread>

#if __has_include("time.h")
#include <time.h>

void Timer::GetDate(kstl::string& a_value, DateFormat a_format) const
{
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	switch (a_format)
	{
	case DAY_MONTH_YEAR:
	{
		strftime(buf, sizeof(buf), "%d/%m/%y", &tstruct);
		break;
	}
	case YEAR_MONTH_DAY:
	{
		strftime(buf, sizeof(buf), "%y/%m/%d", &tstruct);
		break;
	}
	case DAY_MONTH_YEAR_HOURS_MINS:
	{
		strftime(buf, sizeof(buf), "%d/%m/%t - %H:%M", &tstruct);
		break;
	}
	case YEAR_MONTH_DAY_HOURS_MINS:
	{
		strftime(buf, sizeof(buf), "%y/%m/%d - %H:%M", &tstruct);
		break;
	}
	case DAY_MONTH_YEAR_HOURS_MINS_SEC:
	{
		strftime(buf, sizeof(buf), "%d/%m/%y - %H:%M:%S", &tstruct);
		break;
	}
	case YEAR_MONTH_DAY_HOURS_MINS_SEC:
	{
		strftime(buf, sizeof(buf), "%y/%m/%d - %H:%M:%S", &tstruct);
		break;
	}
	case _DAY_MONTH_YEAR_HOURS_MINS_SEC:
	{
		strftime(buf, sizeof(buf), "%d_%m_%y - %H %M %S", &tstruct);
		break;
	}
	case _YEAR_MONTH_DAY_HOURS_MINS_SEC:
	{
		strftime(buf, sizeof(buf), "%y_%m_%d - %H %M %S", &tstruct);
		break;
	}
	}

	a_value = buf;
}

void Timer::GetDate(unsigned int& a_year, unsigned int& a_month, unsigned int& a_day, unsigned int& a_hour, unsigned int& a_min, unsigned int& a_sec) const
{
	time_t     now = time(0);
	struct tm  tstruct;
	//char       buf[80];
	tstruct = *localtime(&now);

	a_year = tstruct.tm_year;
	a_month = tstruct.tm_mon;
	a_day = tstruct.tm_mday;
	a_hour = tstruct.tm_hour;
	a_min = tstruct.tm_min;
	a_sec = tstruct.tm_sec;
}

#else
void Timer::GetDate(kstl::string& a_value, DateFormat a_format) const { a_value = "01/01/2010"; }

void Timer::GetDate(unsigned int& a_year, unsigned int& a_month, unsigned int& a_day, unsigned int& a_hour, unsigned int& a_min, unsigned int& a_sec) const {}

#endif

IMPLEMENT_CLASS_INFO(Timer)

void Timer::InitModifiable()
{
	mT0 = Clock::now();
	mPauseTime = mT0;
	mCurrentState = NORMAL;
}

double Timer::GetTime() const
{
	if (mCurrentState == PAUSED)
	{
		return mPauseTime.time_since_epoch().count() / 1'000'000'000.0;
	}
	auto t1 = Clock::now();
	auto t = t1 - mT0;
	return t.count() / 1'000'000'000.0;
}

void Timer::SetTime(double t)
{
	auto t1 = Clock::now();
	auto dt = t1 - mT0;
	
	long long diff_t = t-dt.count();
	
	mT0 += std::chrono::nanoseconds(diff_t);

}

void Timer::StartPause()
{
	mPauseTime = Clock::now();
}

void Timer::EndPause()
{
	mT0 += Clock::now() - mPauseTime;
}

void Timer::Sleep(unsigned int ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void Timer::SetState(State newstate)
{
	if (mCurrentState == NORMAL && newstate == PAUSED)
	{
		StartPause();
	}
	else if (mCurrentState == PAUSED && newstate == NORMAL)
	{
		EndPause();
	}
	mCurrentState = newstate;
}

double Timer::GetDt(CoreModifiable* caller)
{
	if (mCurrentState == PAUSED)
	{
		return (0.0);
	}
	double current = GetTime();

	if (mTimerMap.find(caller) != mTimerMap.end())
	{
		double dt = current - mTimerMap[caller];
		mTimerMap[caller] = current;
		return dt;
	}
	else
	{
		mTimerMap[caller] = current;
		return (0.0);
	}
}

void Timer::ResetDt(CoreModifiable* caller)
{
	auto It = mTimerMap.find(caller);
	if (It != mTimerMap.end())
	{
		mTimerMap.erase(It);
	}
}
