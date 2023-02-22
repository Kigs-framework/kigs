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

			bool					mSortByPriority = false;
			bool					mVertical = false;

			bool					mResizeLayoutX = false;
			bool					mResizeLayoutY = false;
			/**
			* 0 = Left/Top, 1 = Center, 2 = Right/Bottom
			*/
			s32						mAlignment = 0;
			v2i						mPadding = { 0, 0 };

			WRAP_ATTRIBUTES(mSortByPriority, mVertical, mResizeLayoutX, mResizeLayoutY, mAlignment, mPadding);
		};

	}
}