#pragma once

#include "UIDrawableItem.h"

namespace Kigs
{

	namespace Draw2D
	{
		class UISlider;

		// ****************************************
		// * UISlidersGroup class
		// * --------------------------------------
		/**
		* \file	UISlidersGroup.h
		* \class	UISlidersGroup
		* \ingroup 2DLayers
		* \brief	TODO. Manage a slider.
		*
		* Obsolete ??
		*/
		// ****************************************

		class UISlidersGroup : public UIDrawableItem
		{
		public:
			DECLARE_CLASS_INFO(UISlidersGroup, UIDrawableItem, 2DLayers);

			/**
			 * \brief	constructor
			 * \param	name : instance name
			 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
			 */
			UISlidersGroup(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
			virtual ~UISlidersGroup();

			bool addItem(const CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME)override;
			bool removeItem(const CMSP& item DECLARE_DEFAULT_LINK_NAME)override;

			inline int				GetRemainingValue() const { return mRemainingValue; }
			void					SetRemainingValue(int number) { mRemainingValue = number; };
			inline int				Get_InitialGroupValue()const { return (int)mComunalValue; }
			void					Reset();

			std::vector<UISlider*> GetSliderList() { return mSliderList; }

		protected:

			void InitModifiable()override;


			std::vector<UISlider*>		mSliderList;
			s32							mComunalValue = 0; //value divided between the sliders

			WRAP_ATTRIBUTES(mComunalValue);
			unsigned int				mSliderNumber;
			int							mRemainingValue;
		};

	}
}