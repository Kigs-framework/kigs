#pragma once

#include "CoreModifiableAttribute.h"
#include "CoreModifiable.h"
#include "AttributePacking.h"
#include <thread>
#include <string>

namespace Kigs
{
	namespace Thread
	{
		using namespace Kigs::Core;
		// ****************************************
		// * Thread class
		// * --------------------------------------
		/**
		 * \file	Thread.h
		 * \class	Thread
		 * \ingroup Thread
		 * \brief	Manage a thread.
		 */
		 // ****************************************
		class Thread : public CoreModifiable
		{
		public:

			DECLARE_CLASS_INFO(Thread, CoreModifiable, Thread)
				Thread(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
			virtual ~Thread();

			enum class State
			{
				UNINITIALISED = 0,
				RUNNING,
				FINISHED
			};

			//! return current state (uninitialised, normal or paused)
			State	GetState() { return mCurrentState; }

			template<typename... T>
			void	Start(T&&... params);

			virtual void	Start();

			float	GetProgress() { return 	mProgress; }
			inline State GetCurrentState() const { return mCurrentState; }

			void	setMethod(CMSP localthis, const std::string& method)
			{
				if (mFunctionWasInserted)
				{
					mCallee.lock()->RemoveMethod(mMethod);
					mFunctionWasInserted = false;
				}

				setValue("Callee",localthis);
				mMethod = method;
			}
			template<typename F>
			void	setMethod(CMSP localthis, const std::string& method, F&& func)
			{
				if (mFunctionWasInserted)
				{
					mCallee->RemoveMethod(mMethod);
					mFunctionWasInserted = false;
				}

				mCallee = localthis;
				mMethod = method;

				mCallee->InsertFunction(method, func);
				mFunctionWasInserted = true;
			}

			void	Kill();

		protected:

			// Init start thread if Method && Callee parameters are set
			virtual void InitModifiable() override;

			// reset all states
			void	Done();

			CMSP					mKeepAlive;
			State					mCurrentState;
			float					mProgress;
			std::thread				mCurrentThread;


			bool					mFunctionWasInserted = false;

			std::string						mMethod = "";
			std::weak_ptr<CoreModifiable>	mCallee;

			WRAP_ATTRIBUTES(mMethod, mCallee);
		};

	}
}
