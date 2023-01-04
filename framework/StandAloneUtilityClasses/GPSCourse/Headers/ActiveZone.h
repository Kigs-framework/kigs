#pragma once

#include "CoreModifiable.h"

namespace Kigs
{
	namespace Gps
	{
		using namespace Kigs::Core;

		class ActiveZone : public CoreModifiable
		{
		public:
			DECLARE_CLASS_INFO(ActiveZone, CoreModifiable, Core)

				/**
				* \brief	constructor
				* \fn 		EphebeApp(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
				* \param	name : instance name
				* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
				*/
				ActiveZone(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
			virtual											~ActiveZone() {}

		protected:
		};

	}
}