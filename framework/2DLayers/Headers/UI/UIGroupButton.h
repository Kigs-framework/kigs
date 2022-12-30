#pragma once

#include "UIItem.h"
#include "maUSString.h"

namespace Kigs
{
	namespace Draw2D
	{
		class UIButton;

		// ****************************************
		// * UIGroupButton class
		// * --------------------------------------
		/**
		* \file	UIGroupButton.h
		* \class	UIGroupButton
		* \ingroup 2DLayers
		* \brief	Manage buttons in a group.
		*/
		// ****************************************

		class UIGroupButton : public UIItem
		{
		public:
			DECLARE_CLASS_INFO(UIGroupButton, UIItem, 2DLayers);

			/**
			 * \brief	constructor
			 * \param	name : instance name
			 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
			 */
			UIGroupButton(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
			virtual ~UIGroupButton();
			bool	addItem(const CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME)override;
			bool	removeItem(const CMSP& item DECLARE_DEFAULT_LINK_NAME)override;

			void			reComputeSize();
		protected:

			void InitModifiable()override;
			void	computeSize(int _buttonNumber, CoreModifiable* a_item);


			std::vector<UIButton*>		mButtonList;
			maBool						mKeepOnePressed;
			maBool						mOnlyOnePressed;
			maString					mNoSelectedAction;
			maUSString					mParameter;
			unsigned int				mButtonNumber;
		};

	}
}