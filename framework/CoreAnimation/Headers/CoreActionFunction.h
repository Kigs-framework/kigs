#pragma once

#include "CoreAction.h"
#include "CoreValue.h"
#include "CoreItemOperator.h"

namespace Kigs
{
	namespace Action
	{
		using namespace Kigs::Core;
		// ****************************************
		// * CoreItemAnimationContext class
		// * --------------------------------------
		/**
		* \file	CoreActionFunction.h
		* \class	CoreItemAnimationContext
		* \ingroup CoreAnimation
		* \brief	Special evaluation context for animations
		*
		*/
		// ****************************************

		class CoreItemAnimationContext : public CoreItemEvaluationContext
		{
		public:
			// current "global" time
			double		mTime;
			// current action start time
			double		mActionStartTime;
		};

		// ****************************************
		// * CoreActionFunction class
		// * --------------------------------------
		/**
		* \file	CoreActionFunction.h
		* \class	CoreActionFunction
		* \ingroup CoreAnimation
		* \brief	Manage a CoreItemOperator kind of computation.
		*
		*/
		// ****************************************

		template<typename dataType, int dimension>
		class CoreActionFunction : public CoreAction
		{
		public:

			CoreActionFunction() : CoreAction()
			{
				int i;
				for (i = 0; i < dimension; i++)
				{
					mFunctions[i] = nullptr;
				}
				mContext.mTime = mContext.mActionStartTime = -1.0;
			}

			virtual ~CoreActionFunction()
			{
				int i;
				for (i = 0; i < dimension; i++)
				{
					mFunctions[i] = nullptr;
				}

			}

			virtual void init(CoreSequence* sequence, CoreVector* params);

			virtual void	setStartTime(double t)
			{
				CoreAction::setStartTime(t);
				if (mContext.mActionStartTime < -1.0)
				{
					mContext.mActionStartTime = t;
				}
			}

		protected:

			virtual bool	protectedUpdate(double time)
			{
				CoreAction::protectedUpdate(time);
				CoreItemEvaluationContext::SetContext(&mContext);
				mContext.mTime = time;
				dataType result;

				auto ptr = mTarget.lock();

				if (ptr && ptr->getValue(mParamID, result))
				{

					if (mHasUniqueMultidimensionnalFunc)
					{
						result = mFunctions[0]->operator dataType();
					}
					else
					{
						int i;
						for (i = 0; i < dimension; i++)
						{
							if (mFunctions[i])
							{
								result[i] = (float)*mFunctions[i];
							}
						}
					}
					ptr->setValue(mParamID, result);
				}
				CoreItemEvaluationContext::ReleaseContext();
				return false;
			}

			CoreItemSP	mFunctions[dimension];

			bool		mHasUniqueMultidimensionnalFunc = false;

			CoreItemAnimationContext	mContext;
		};


		template<>
		inline bool	CoreActionFunction<float, 1>::protectedUpdate(double time)
		{
			CoreAction::protectedUpdate(time);
			CoreItemEvaluationContext::SetContext(&mContext);
			mContext.mTime = time;
			if (mFunctions[0])
			{
				float result = (float)*mFunctions[0];
				auto ptr = mTarget.lock(); if (ptr) ptr->setValue(mParamID, result);
			}
			CoreItemEvaluationContext::ReleaseContext();
			return false;
		}


		typedef CoreActionFunction < float, 1 > CoreActionFunction1D;
		typedef CoreActionFunction < v2f, 2 > CoreActionFunction2D;
		typedef CoreActionFunction < v3f, 3 > CoreActionFunction3D;
		typedef CoreActionFunction < v4f, 4 > CoreActionFunction4D;


		// ****************************************
		// * ActionTimeOperator class
		// * --------------------------------------
		/**
		* \file	CoreActionFunction.h
		* \class	ActionTimeOperator
		* \ingroup CoreAnimation
		* \brief	Return current context time if available
		*
		*/
		// ****************************************

		template<typename operandType>
		class ActionTimeOperator : public CoreItemOperator<operandType>
		{
		public:

			virtual inline operator operandType() const
			{
				if (CoreItemEvaluationContext::GetContext())
				{
					CoreItemAnimationContext& currentContext = *((CoreItemAnimationContext*)CoreItemEvaluationContext::GetContext());

					return (operandType)(currentContext.mTime - currentContext.mActionStartTime);
				}
				return ((operandType)0);
			}
			static std::unique_ptr<CoreVector> create()
			{
				return std::unique_ptr<CoreVector>(new ActionTimeOperator<operandType>());
			}

		protected:
		};

	}
}