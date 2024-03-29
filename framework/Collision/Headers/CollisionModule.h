#pragma once

#include "ModuleBase.h"

namespace Kigs
{
	namespace Collide
	{
		using namespace Kigs::Core;

		/*! \defgroup Collision Collision module
		 *  manage intersection and collisions
		*/


		// ****************************************
		// * CollisionModule class
		// * --------------------------------------
		/**
		* \file	CollisionModule.h
		* \class	CollisionModule
		* \ingroup Collision
		* \ingroup Module
		* \brief Generic Module collision / intersection management
		*/
		// ****************************************


		class CollisionModule : public ModuleBase
		{
		public:

			DECLARE_CLASS_INFO(CollisionModule, ModuleBase, Collision)
				CollisionModule(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
			~CollisionModule() override;

			//! module init
			void Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params) override;

			//! module close
			void Close() override;

			//! module update
			void Update(const Time::Timer& timer, void* addParam) override;
		};
	}
}