#pragma once

#include "Node3D.h"
#include "maNumeric.h"

namespace Kigs
{
	namespace Scene
	{
		using namespace Kigs::Core;
		// ****************************************
		// * Node3DCullLodUpgrador class
		// * --------------------------------------
		/**
		 * \file	Node3DCullLodUpgrador.h
		 * \class	Node3DCullLodUpgrador
		 * \ingroup SceneGraph
		 * \brief	Don't display too small objects.
		 */
		 // ****************************************

		class Node3DCullLodUpgrador : public CullUpgrador
		{

			// create and init Upgrador if needed and add dynamic attributes
			void	Init(CoreModifiable* toUpgrade) override;

			// destroy UpgradorData and remove dynamic attributes 
			void	Destroy(CoreModifiable* toDowngrade, bool toDowngradeDeleted) override;

			START_CULL_UPGRADOR(Node3DCullLodUpgrador);
			DECLARE_CULL_UPGRADOR();

			maFloat* mLodCoef;
		};

	}
}

