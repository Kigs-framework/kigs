#pragma once

#include "CoreAction.h"
#include "CoreValue.h"

namespace Kigs
{
	namespace Action
	{
		using namespace Kigs::Core;
		// ****************************************
		// * CoreActionKeyFrameBase class
		// * --------------------------------------
		/**
		* \file	CoreActionKeyFrame.h
		* \class	CoreActionKeyFrameBase
		* \ingroup CoreAnimation
		* \brief	Base class for Keyframe CoreAction
		*
		*/
		// ****************************************

		class CoreActionKeyFrameBase : public CoreAction
		{
		public:

			CoreActionKeyFrameBase() : CoreAction(), mKeyFrameCount(0), mLastKeyIndex(-1), mTimeArray(0)
			{
			}

			virtual void	setStartTime(double t)
			{
				CoreAction::setStartTime(t);
				mLastKeyIndex = -1;
			}

			virtual ~CoreActionKeyFrameBase()
			{
				if (mTimeArray)
				{
					delete[]	mTimeArray;
				}
			}


		protected:

			virtual bool	protectedUpdate(double time);

			virtual void	protectedSetValue(int index) = 0;

			unsigned int		mKeyFrameCount;
			int					mLastKeyIndex;

			double* mTimeArray;

		};

		// ****************************************
		// * CoreActionKeyFrame class
		// * --------------------------------------
		/**
		* \file	CoreActionKeyFrame.h
		* \class	CoreActionKeyFrame
		* \ingroup CoreAnimation
		* \brief	Keyframe CoreAction
		*
		*/
		// ****************************************

		template<typename dataType>
		class CoreActionKeyFrame : public CoreActionKeyFrameBase
		{
		public:

			CoreActionKeyFrame() : CoreActionKeyFrameBase(), mKeyFrameArray(0)
			{}

			virtual void init(CoreSequence* sequence, CoreVector* params)
			{
				mTarget = sequence->getTarget();

				std::string readstring;
				(*params)[0]->getValue(readstring);
				mTarget = checkSubTarget(readstring);

				mParamID = CharToID::GetID(readstring);

				// stock in list before creating the final array
				std::vector<dataType>	L_values;
				std::vector<double>	L_times;
				float readfloat;
				dataType	readPoint;

				unsigned int i;
				for (i = 1; i < params->size(); i += 2) // read each (time + val) couples
				{
					(*params)[i]->getValue(readfloat);
					L_times.push_back(readfloat);
					(*params)[i + 1]->getValue(readPoint);
					L_values.push_back(readPoint);

				}

				if (L_values.size() == 0)
				{
					// mhh not good
					return;
				}

				mKeyFrameCount = (unsigned int)L_values.size();
				mDuration = L_times[L_times.size() - 1];

				mKeyFrameArray = new dataType[L_values.size()];
				mTimeArray = new double[L_values.size()];

				for (i = 0; i < L_values.size(); i++)
				{
					mKeyFrameArray[i] = L_values[i];
					mTimeArray[i] = L_times[i];
				}
			}


			virtual ~CoreActionKeyFrame()
			{
				if (mKeyFrameArray)
				{
					delete[]	mKeyFrameArray;
				}
			}

		protected:

			inline void	protectedSetValue(int index)
			{
				auto ptr = mTarget.lock();
				if (ptr)
					ptr->setValue(mParamID, mKeyFrameArray[index]);
			}

			dataType* mKeyFrameArray;

		};

	}
}