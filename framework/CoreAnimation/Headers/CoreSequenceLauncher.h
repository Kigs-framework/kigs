#pragma once

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "maCoreItem.h"

#include "CoreSequence.h"

#include "AttributePacking.h"

namespace Kigs
{
	namespace Action
	{
		using namespace Kigs::Core;
		// ****************************************
		// * CoreSequenceLauncher class
		// * --------------------------------------
		/**
		* \class	CoreSequenceLauncher
		* \file		CoreSequenceLauncher.h
		* \ingroup CoreAnimation
		* \brief	CoreModifiable owning a sequence.
		*
		* The sequence is played on the parent of this CoreModifiable.
		*
		*/
		// ****************************************

		class CoreSequenceLauncher : public CoreModifiable
		{
		public:
			DECLARE_CLASS_INFO(CoreSequenceLauncher, CoreModifiable, CoreAnimation);
			DECLARE_INLINE_CONSTRUCTOR(CoreSequenceLauncher) {}
			WRAP_METHODS(Start, Stop, IsFinished);

		protected:
			void	InitModifiable() override;

			void	checkDeadSequences();
			void	addSequencesToParents();

			//! parameter for fullscreen window
			bool			mStartOnFirstUpdate = false;
			bool			mOnce = false;
			std::string		mStartMessage = "";

			WRAP_ATTRIBUTES(mStartOnFirstUpdate, mOnce, mStartMessage);

			maCoreItem		mSequence = BASE_ATTRIBUTE(Sequence);

			unordered_map<CoreModifiable*, std::weak_ptr<CoreSequence>> mSequenceMap;

			void Start();
			void Stop();
			bool IsFinished();
		};

	}
}
