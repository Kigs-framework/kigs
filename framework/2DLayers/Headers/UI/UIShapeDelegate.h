#pragma once

#include "CoreModifiable.h"

namespace Kigs
{

	namespace Draw2D
	{
		using namespace Kigs::Core;
		// ****************************************
		// * UIShapeDelegate class
		// * --------------------------------------
		/**
		* \file	UIShapeDelegate.h
		* \class	UIShapeDelegate
		* \ingroup 2DLayers
		* \brief	draw parent ui as a shape
		*
		*/
		// ****************************************

		class UIItem;
		class UIVerticesInfo;

		class UIShapeDelegate : public CoreModifiable
		{
		public:
			DECLARE_ABSTRACT_CLASS_INFO(UIShapeDelegate, CoreModifiable, 2DLayers);

			/**
			* \brief	constructor
			* \param	name : instance name
			* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
			*/
			UIShapeDelegate(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			virtual void SetTexUV(UIItem* item, UIVerticesInfo* aQI) = 0;
			virtual void SetVertexArray(UIItem* item, UIVerticesInfo* aQI) = 0;
		};
	}
}
