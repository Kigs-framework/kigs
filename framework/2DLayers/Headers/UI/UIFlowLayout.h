#pragma once

#include "UILayout.h"

namespace Kigs
{

	namespace Draw2D
	{
		// ****************************************
		// * UIFlowLayout class
		// * --------------------------------------
		/**
		* \file	UIFlowLayout.h
		* \class	UIFlowLayout
		* \ingroup 2DLayers
		* \brief	Adjust the position of all children according to a flow layout algorithm.
		*/
		// ****************************************

		class UIFlowLayout : public UILayout
		{
		public:
			DECLARE_CLASS_INFO(UIFlowLayout, UILayout, 2DLayers);

			/**
			* \brief	constructor
			* \param	name : instance name
			* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
			*/
			UIFlowLayout(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

		protected:
			void		NotifyUpdate(const unsigned int labelid)override;
			void		RecomputeLayout()override;

			maVect2DI	mPadding;
			maBool		mSortByPriority;
			maBool		mRescaleToFit;
		};
	}
}