#pragma once

#include "UILayout.h"

namespace Kigs
{
	namespace Draw2D
	{
		// ****************************************
		// * UIBoxLayout class
		// * --------------------------------------
		/**
		* \file	UIBoxLayout.h
		* \class	UIBoxLayout
		* \ingroup 2DLayers
		* \brief	Adjust the position of all children according to the box layout
		*
		*/
		// ****************************************

		class UIBoxLayout : public UILayout
		{
		public:
			DECLARE_CLASS_INFO(UIBoxLayout, UILayout, 2DLayers);
			DECLARE_CONSTRUCTOR(UIBoxLayout);

		protected:
			void	NotifyUpdate(const unsigned int labelid)override;
			void	RecomputeLayout()override;

			maVect2DI				mPadding = BASE_ATTRIBUTE(Padding, 0, 0);
			maBool					mSortByPriority = BASE_ATTRIBUTE(SortByPriority, false);
			maBool					mVertical = BASE_ATTRIBUTE(Vertical, false);

			/**
			* 0 = Left/Top, 1 = Center, 2 = Right/Bottom
			*/
			maInt					mAlignment = BASE_ATTRIBUTE(Alignment, false);
			maBool					mResizeLayoutX = BASE_ATTRIBUTE(ResizeLayoutX, false);
			maBool					mResizeLayoutY = BASE_ATTRIBUTE(ResizeLayoutY, false);

		};

	}
}