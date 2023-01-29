#pragma once

#include "CoreModifiable.h"
#include "AttributePacking.h"
#include "maNumeric.h"

#include <map>
#include <chrono>
#include <functional>

namespace Kigs
{
	namespace Time
	{
		using namespace Kigs::Core;

		using Clock = std::chrono::high_resolution_clock;
		using TimePoint = Clock::time_point;
		using Duration = Clock::duration;
		using DurationF64 = std::chrono::duration<double>;

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
				, mTime(*this, "Time", "GetTime", "")
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

			//! return current time for this timer.
			virtual double GetTime() const;
			//! set current time
			virtual void SetTime(double t);

			//! get current state (UNINITIALISED, NORMAL, PAUSED)
			State	GetState() { return mCurrentState; }
			//! change state : pause or restart timer
			virtual void SetState(State newstate);

			//! get current date as string, using given format
			void GetDate(std::string& a_value, DateFormat a_format) const;
			//! get all values for the current date
			void GetDate(unsigned int& a_year, unsigned int& a_month, unsigned int& a_day, unsigned int& a_hour, unsigned int& a_min, unsigned int& a_sec) const;

			//! get elapsed time since this method was called for the given caller
			double GetDt(CoreModifiable* caller) const;
			//! reset elapsed time for this caller
			void ResetDt(CoreModifiable* caller);
			//! remove this caller from dt map
			void RemoveDt(CoreModifiable* caller);


			//! get elapsed time since the first time this method was called for the given caller
			double GetDelay(CoreModifiable* caller) const;
			//! reset elapsed time for this caller
			void ResetDelay(CoreModifiable* caller);
			//! remove this caller from delay map
			void RemoveDelay(CoreModifiable* caller);

			//! sleep this thread for given ms
			void Sleep(unsigned int ms);

		protected:
			void InitModifiable() override;

			State mCurrentState = UNINITIALISED;
			TimePoint mT0;
			TimePoint mPauseTime;

			void StartPause();
			void EndPause();

			std::unordered_map<CoreModifiable*, double> mTimerDtMap;
			std::unordered_map<CoreModifiable*, double> mTimerDelayMap;

			//! Time coremodifiable attribute
			maComputedNumericInit<double>	mTime;
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

		struct TimeTracker
		{
			TimeTracker() : mStartTime{ TimePoint::clock::now() } {}
			void PrintTimeElapsed(const char* txt_before = "", bool use_cout = false);
			auto GetTimeElapsed() { return TimePoint::clock::now() - mStartTime; }
			TimePoint mStartTime;
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

	}
}