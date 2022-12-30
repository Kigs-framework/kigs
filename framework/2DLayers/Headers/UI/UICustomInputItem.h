#pragma once

#include "UI/UIItem.h"
#include "AttributePacking.h"

namespace Kigs
{
	namespace Draw2D
	{
		// ****************************************
		// * UICustomInputItem class
		// * --------------------------------------
		/**
		* \file	UICustomInputItem.h
		* \class	UICustomInputItem
		* \ingroup 2DLayers
		* \brief	???
		*
		*/
		// ****************************************

		class UICustomInputItem : public UIItem
		{
		public:
			DECLARE_CLASS_INFO(UICustomInputItem, UIItem, 2DLayers);
			DECLARE_INLINE_CONSTRUCTOR(UICustomInputItem) {}

		protected:
			void InitModifiable() override;

			maFloat mAutoTouchDistance = BASE_ATTRIBUTE(AutoTouchDistance, 0.0f);

		};

	}
}
