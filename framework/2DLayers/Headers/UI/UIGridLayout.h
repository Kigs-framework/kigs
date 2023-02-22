#pragma once

#include "UILayout.h"

namespace Kigs
{
	namespace Draw2D
	{
		// ****************************************
		// * UIGridLayout class
		// * --------------------------------------
		/**
		* \file	UIGridLayout.h
		* \class	UIGridLayout
		* \ingroup 2DLayers
		* \brief	Adjust the position of all children according to a grid layout algorithm.
		*/
		// ****************************************

		class UIGridLayout : public UILayout
		{
		public:
			DECLARE_CLASS_INFO(UIGridLayout, UILayout, 2DLayers);

			/**
			* \brief	constructor
			* \param	name : instance name
			* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
			*/
			UIGridLayout(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

		protected:
			void			NotifyUpdate(const unsigned int labelid)override;
			void			RecomputeLayout()override;

			bool					mSortByPriority = false;
			bool					mResizeElements = false;
			bool					mAutoResize = true;
			bool					mHeaderRow = false;
			u32						mColumns = 0;
			u32						mRowHeight = 0;
			u32						mColumnWidth = 0;
			v2i						mPadding = {0,0};

			WRAP_ATTRIBUTES(mSortByPriority	,mResizeElements,mAutoResize,mHeaderRow	,mColumns,mRowHeight,mColumnWidth,mPadding);
		};

	}
}