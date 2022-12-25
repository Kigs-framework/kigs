#pragma once
#include "DataDrivenBaseApplication.h"

namespace Kigs
{
	namespace DDriven
	{
		using namespace Core;

		// ****************************************
		// * BaseDDSequence class
		// * --------------------------------------
		/**
		 * \file	BaseDDSequence.h
		 * \class	BaseDDSequence
		 * \ingroup DataDrivenApplication
		 * \brief	A data driven sequence.
		 *
		 * ?? Obsolete ?? just a DataDrivenSequence ?
		 */
		 // ****************************************

		class BaseDDSequence : public DataDrivenSequence
		{
		public:
			DECLARE_CLASS_INFO(BaseDDSequence, DataDrivenSequence, CoreDataDrivenSequence)
				BaseDDSequence(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
			virtual ~BaseDDSequence();

		protected:
			maString mTransitionData;
		};

	}
}
