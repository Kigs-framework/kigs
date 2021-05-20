#ifndef _UISLIDERSGROUP_H_
#define _UISLIDERSGROUP_H_

#include "UIDrawableItem.h"

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
	UISlidersGroup(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	virtual ~UISlidersGroup();

	bool addItem(const CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME)override;
	bool removeItem(const CMSP& item DECLARE_DEFAULT_LINK_NAME)override;

	inline int				GetRemainingValue() const { return mRemainingValue; }
	void					SetRemainingValue(int number) { mRemainingValue = number; };
	inline int				Get_InitialGroupValue()const { return (int)mComunalValue; }
	void					Reset();

	kstl::vector<UISlider*> GetSliderList() { return mSliderList; }

protected:
	
	void InitModifiable()override;


	kstl::vector<UISlider*>		mSliderList;
	maInt						mComunalValue; //value divided between the sliders
	unsigned int				mSliderNumber;
	int							mRemainingValue;
};

#endif //_UISLIDERSGROUP_H_