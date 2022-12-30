#pragma once

#include "Node3D.h"

namespace Kigs
{
	namespace Scene
	{
		// ****************************************
		// * Node3DLodDecorator class
		// * --------------------------------------
		/**
		 * \file	Node3DLodDecorator.h
		 * \class	Node3DLodDecorator
		 * \ingroup SceneGraph
		 * \brief	Decorate Cull method to accep LOD.
		 *
		 * ?? Obsolete ??
		 *
		 */
		 // ****************************************

		class	Node3DLodDecorator : public Node3D
		{
		public:

			DECLARE_DECORATOR_DECORATE()
			{
				DECORATE_METHOD(Cull, Node3D, Node3DLodDecorator);
				cm->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::FLOAT, "LodCoef");
				cm->setValue("LodCoef", 1.0);
				return true;
			}
			DECLARE_DECORATOR_UNDECORATE()
			{
				cm->RemoveDynamicAttribute("LodCoef");
				UNDECORATE_METHOD(Cull, Node3D, Node3DLodDecorator);
				return true;
			}

		protected:
			DECLARE_DECORABLE_DEFINITION(bool, Cull, TravState* state, unsigned int cullingMask);
		};

	}
}