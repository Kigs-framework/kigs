#pragma once

#include "ModuleBase.h"

namespace Kigs
{
	namespace Thread
	{
		using namespace Kigs::Core;
		/*! \defgroup Thread Thread module
		 *  base thread management
		*/


		// ****************************************
		// * ModuleThread class
		// * --------------------------------------
		/**
		 * \file	ModuleThread.h
		 * \class	ModuleThread
		 * \ingroup Thread
		 * \ingroup Module
		 * \brief	Generic module for thread management.
		 */
		 // ****************************************
		class ModuleThread : public ModuleBase
		{
		public:

			DECLARE_CLASS_INFO(ModuleThread, ModuleBase, Thread)
				ModuleThread(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
			virtual ~ModuleThread();
			//! init module
			void Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params) override;
			//! close module
			void Close() override;

			//! update module
			void Update(const Time::Timer& timer, void* addParam) override;
		};
	}
}
