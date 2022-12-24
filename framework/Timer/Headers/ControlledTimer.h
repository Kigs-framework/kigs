#pragma once

#include "Timer.h"

namespace Kigs
{
	namespace Time
	{
		// ****************************************
		// * ControlledTimer class
		// * --------------------------------------
		/**
		 * \class	ControlledTimer
		 * \file	ControlledTimer.h
		 * \ingroup TimerModule
		 * \brief	A "fake" timer where time is given by the user.
		 */
		 // ****************************************
		class ControlledTimer : public Timer
		{
		public:
			DECLARE_CLASS_INFO(ControlledTimer, Timer, Timer)

				DECLARE_INLINE_CONSTRUCTOR(ControlledTimer)
			{
				mCurrentTime = 0;
			}

			double	GetTime() const override
			{
				return mCurrentTime;
			}

			void SetTime(double t) override
			{
				mCurrentTime = t;
			}

		protected:
			double mCurrentTime;
		};

	}
}