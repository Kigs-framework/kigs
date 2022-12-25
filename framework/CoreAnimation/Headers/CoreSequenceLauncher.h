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
			maBool			mStartOnFirstUpdate = BASE_ATTRIBUTE(StartOnFirstUpdate, false);
			maCoreItem		mSequence = BASE_ATTRIBUTE(Sequence);
			maString		mStartMessage = BASE_ATTRIBUTE(StartMessage, "");
			maBool			mOnce = BASE_ATTRIBUTE(Once, false);

			unordered_map<CoreModifiable*, std::weak_ptr<CoreSequence>> mSequenceMap;

			void Start();
			void Stop();
			bool IsFinished();
		};

	}
}
