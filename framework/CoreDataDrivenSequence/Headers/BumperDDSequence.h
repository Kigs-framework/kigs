#pragma once
#include "BaseDDSequence.h"
#include "maUSString.h"

namespace Kigs
{
	namespace DDriven
	{
		using namespace Core;
		using namespace Time;
		// ****************************************
		// * BumperDDSequence class
		// * --------------------------------------
		/**
		 * \file	BumperDDSequence.h
		 * \class	BumperDDSequence
		 * \ingroup DataDrivenApplication
		 * \brief	Splash screen
		 *
		 * ?? Obsolete ??
		 */
		 // ****************************************

		class BumperDDSequence : public BaseDDSequence
		{
		public:
			DECLARE_CLASS_INFO(BumperDDSequence, BaseDDSequence, CoreDataDrivenSequence)
				BumperDDSequence(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
			virtual ~BumperDDSequence();

		protected:
			void Update(const Timer& timer, void* /*addParam*/) override;

			maUSString mNextSequence;
			maFloat mDuration;

			double mStartTime;
		};
	}
}
