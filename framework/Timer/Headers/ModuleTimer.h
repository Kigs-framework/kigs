#pragma once

#include "ModuleBase.h"

namespace Kigs
{
	namespace Time
	{
		using namespace Kigs::Core;
		/*! \defgroup TimerModule manage time
		*/

		// ****************************************
		// * ModuleTimer class
		// * --------------------------------------
		/**
		 * \file	ModuleTimer.h
		 * \class	ModuleTimer
		 * \ingroup TimerModule
		 * \ingroup Module
		 * \brief	Generic module for time management.
		 */
		 // ****************************************
		class ModuleTimer : public ModuleBase
		{
		public:
			DECLARE_CLASS_INFO(ModuleTimer, ModuleBase, Timer)
				ModuleTimer(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
			void Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params) override;
			void Close() override;
			void Update(const Timer& timer, void* addParam) override;
			virtual ~ModuleTimer();
		protected:
		};
	}
}

