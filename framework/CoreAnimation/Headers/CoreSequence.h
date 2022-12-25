#pragma once

#include "CoreVector.h"
#include "Timer.h"

namespace Kigs
{
	namespace Action
	{
		using namespace Kigs::Core;
		using namespace Kigs::Time;

		// ****************************************
		// * CoreSequence class
		// * --------------------------------------
		/**
		* \class	CoreSequence
		* \file		CoreSequence.h
		* \ingroup CoreAnimation
		* \brief	Manage a sequence of animation ( list of animations )
		*
		*/
		// ****************************************

		class CoreSequence : public CoreVector
		{
		public:
			CoreSequence(CMSP target, KigsID name, Timer* reftimer = 0);

			virtual ~CoreSequence();

			KigsID	getID() const
			{
				return mID;
			}

			void	startAtFirstUpdate()
			{
				mStartTime = -4.0;
			}

			inline void	start(const Timer& timer)
			{
				mRefTimer = &timer;
				protectedStart(timer.GetTime());
			}
			inline void	start()// use ref timer
			{
				protectedStart(mRefTimer->GetTime());
			}

			// return true if finished
			bool	update(const Timer& timer);

			inline void	pause(const Timer& timer)
			{
				protectedPause(timer.GetTime());
			}
			inline void	pause() // use ref timer
			{
				protectedPause(mRefTimer->GetTime());
			}

			// reset sequence
			void	stop();

			CMSP	getTarget() const
			{
				return mTarget.lock();
			}

			// remove the target if already destroyed
			void removeTarget()
			{
				mTarget.reset();
			}

			// return true if target is modifiable 
			bool	useModifiable(CoreModifiable* modifiable) const
			{
				if (mTarget.lock().get() == modifiable)
				{
					return true;
				}
				return false;
			}

		protected:

			void	protectedStart(double time);
			void	protectedPause(double time);
			void	protectedUpdate(double time);

			std::weak_ptr<CoreModifiable> mTarget;
			KigsID				mID;
			double				mStartTime;
			double				mPauseTime;
			const Timer* mRefTimer;
			unsigned int		mCurrentActionIndex;
		};


	}
}
