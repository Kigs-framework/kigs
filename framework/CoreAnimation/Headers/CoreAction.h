#pragma once

#include "CoreSequence.h"
#include "CoreItem.h"
#include "kTypes.h"
#include "TecLibs/Tec3D.h"

namespace Kigs
{
	namespace Core
	{
		class CoreVector;
	}
	namespace Action
	{
		using namespace Kigs::Core;


#define	TimeEpsilon	(0.005)

		// ****************************************
		// * CoreAction class
		// * --------------------------------------
		/**
		* \file	CoreAction.h
		* \class	CoreAction
		* \ingroup CoreAnimation
		* \brief	Special CoreItem type used to animate attributes
		*
		*/
		// ****************************************

		class CoreAction : public CoreItem
		{
		public:

			friend class CoreSequence;
			friend class ModuleCoreAnimation;

			virtual ~CoreAction();

			// return true if finished
			bool	update(double time);

			virtual void init(CoreSequence* sequence, CoreVector* params) = 0;

			void	setIsDone()
			{
				mType = (COREITEM_TYPE)(11);
			}

			bool	isDone()
			{
				return (((unsigned int)mType) == 11);
			}

			double getEndTime()
			{
				return mStartTime + mDuration;
			}

			// action is started by sequence
			virtual void	setStartTime(double t)
			{
				mStartTime = t;
			}

			// undone
			virtual void reset()
			{
				mType = (COREITEM_TYPE)(10);
			}

			inline double	getDuration()
			{
				return mDuration;
			}

			virtual void* getContainerStruct()
			{
				return 0;
			}

		protected:

			// default constructor called by sons only
			CoreAction() : CoreItem((COREITEM_TYPE)10), mStartTime(-1.0f), mDuration(-1.0f), mActionFlags(0), mParamID(0)
			{
				mTargetPath = "";
			}

			// if paramstring contains -> then extract param name part and return real target (son on current target)
			CMSP	checkSubTarget(std::string& paramstring);

			inline void CheckDelayTarget()
			{
				// if delayed target
				if (mTargetPath != "")
				{
					auto ptr = mTarget.lock();
					if (ptr)
					{
						CMSP findTarget = ptr->GetInstanceByPath(mTargetPath);
						if (findTarget)
						{
							mTarget = findTarget;
							mTargetPath = "";
						}
					}
					else mTarget.reset();
				}
			}

			// return true if action asks itself to be closed
			virtual bool	protectedUpdate(double time)
			{
				CheckDelayTarget();
				return false; // no used
			}

			std::weak_ptr<CoreModifiable> mTarget;
			double				mStartTime;
			double				mDuration;

			unsigned int		mActionFlags;
			std::string		mTargetPath;
			unsigned int		mParamID;
		};


		// ****************************************
		// * CoreActionWait class
		// * --------------------------------------
		/**
		* \file	CoreAction.h
		* \class	CoreActionWait
		* \ingroup CoreAnimation
		* \brief	Do nothing CoreAction
		*
		* Usefull to delay another animation start.
		*/
		// ****************************************
		class CoreActionWait : public CoreAction
		{
		public:

			CoreActionWait() : CoreAction()
			{}
			virtual void init(CoreSequence* sequence, CoreVector* params);
		protected:

			virtual bool	protectedUpdate(double time) { CoreAction::protectedUpdate(time);  return false; };
		};

	}
}