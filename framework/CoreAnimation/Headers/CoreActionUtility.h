#pragma once

#include "CoreAction.h"
#include "CoreValue.h"

namespace Kigs
{
	namespace Action
	{
		using namespace Kigs::Core;

		// ****************************************
		// * CoreActionRemoveFromParent class
		// * --------------------------------------
		/**
		* \class	CoreActionRemoveFromParent
		* \file		CoreActionUtility.h
		* \ingroup CoreAnimation
		* \brief	Remove the object owning this sequence from its parent of the given type.
		*/
		// ****************************************

		class CoreActionRemoveFromParent : public CoreAction
		{
		public:
			CoreActionRemoveFromParent() : CoreAction()
			{
				mParentTypeID = 0xFFFFFFFF;
			}
			virtual void init(CoreSequence* sequence, CoreVector* params);
		protected:

			virtual bool	protectedUpdate(double time);
			unsigned int	mParentTypeID;
		};

		// ****************************************
		// * CoreActionSendMessage class
		// * --------------------------------------
		/**
		* \class	CoreActionSendMessage
		* \file		CoreActionUtility.h
		* \ingroup CoreAnimation
		* \brief	Post a message ( notification )
		*/
		// ****************************************

		class CoreActionSendMessage : public CoreAction
		{
		public:
			CoreActionSendMessage() : CoreAction()
			{
				;
			}
			virtual void init(CoreSequence* sequence, CoreVector* params);
		protected:

			virtual bool	protectedUpdate(double time);
			std::string	mMessage;
			usString		mParam;
		};

		// ****************************************
		// * CoreActionEmitSignal class
		// * --------------------------------------
		/**
		* \class	CoreActionEmitSignal
		* \file		CoreActionUtility.h
		* \ingroup CoreAnimation
		* \brief	Emit a signal.
		*/
		// ****************************************
		class CoreActionEmitSignal : public CoreAction
		{
		public:
			CoreActionEmitSignal() : CoreAction()
			{
				;
			}
			virtual void init(CoreSequence* sequence, CoreVector* params);
		protected:

			virtual bool	protectedUpdate(double time);
			std::string	mSignal;
			usString		mParam;
		};

		// ****************************************
		// * CoreActionCombo class
		// * --------------------------------------
		/**
		* \class	CoreActionCombo
		* \file		CoreActionUtility.h
		* \ingroup CoreAnimation
		* \brief	Play all children animation together.
		*/
		// ****************************************

		class CoreActionCombo : public CoreAction
		{
		public:
			CoreActionCombo() : CoreAction()
			{
				;
			}

			virtual ~CoreActionCombo();

			virtual void init(CoreSequence* sequence, CoreVector* params);

			virtual void	setStartTime(double t);


		protected:

			virtual bool	protectedUpdate(double time);
			std::vector<SP<CoreAction>>	mList;
		};

		// ****************************************
		// * CoreActionSerie class
		// * --------------------------------------
		/**
		* \class	CoreActionSerie
		* \file		CoreActionUtility.h
		* \ingroup CoreAnimation
		* \brief	Play each children animation one after the other.
		*/
		// ****************************************

		class CoreActionSerie : public CoreAction
		{
		public:
			CoreActionSerie() : CoreAction(), mCurrentActionIndex(0)
			{
				;
			}

			virtual ~CoreActionSerie();

			virtual void init(CoreSequence* sequence, CoreVector* params);

			virtual void	setStartTime(double t);

		protected:

			virtual bool				protectedUpdate(double time);
			std::vector<SP<CoreAction>>	mList;
			unsigned int				mCurrentActionIndex;
		};

		// ****************************************
		// * CoreActionForLoop class
		// * --------------------------------------
		/**
		* \class	CoreActionForLoop
		* \file		CoreActionUtility.h
		* \ingroup CoreAnimation
		* \brief	Manage a "for" loop.
		*/
		// ****************************************
		class CoreActionForLoop : public CoreAction
		{
		public:
			CoreActionForLoop() : CoreAction(), mLoopCount(0), mCurrentLoopIndex(0), mActionToLoop(0)
			{
				;
			}

			virtual ~CoreActionForLoop();

			virtual void	setStartTime(double t);

			virtual void init(CoreSequence* sequence, CoreVector* params);

		protected:

			virtual bool				protectedUpdate(double time);
			int							mLoopCount;
			int							mCurrentLoopIndex;
			SP<CoreAction>					mActionToLoop;
		};

		// ****************************************
		// * CoreActionDoWhile class
		// * --------------------------------------
		/**
		* \class	CoreActionDoWhile
		* \file		CoreActionUtility.h
		* \ingroup CoreAnimation
		* \brief	Manage a "do while" loop.
		*/
		// ****************************************
		class CoreActionDoWhile : public CoreAction
		{
		public:
			CoreActionDoWhile() : CoreAction(), mActionToLoop(0), mIsZeroDuration(false)
			{
				;
			}

			virtual ~CoreActionDoWhile();

			virtual void	setStartTime(double t);

			virtual void init(CoreSequence* sequence, CoreVector* params);

		protected:

			virtual bool				protectedUpdate(double time);
			SP<CoreAction>				mActionToLoop;
			bool						mIsZeroDuration;
		};

	}
}