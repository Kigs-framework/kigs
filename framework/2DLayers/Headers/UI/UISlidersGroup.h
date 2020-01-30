#ifndef _UISLIDERSGROUP_H_
#define _UISLIDERSGROUP_H_

#include "UIDrawableItem.h"

class UISlider;

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

	bool addItem(const CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME)override;
	bool removeItem(const CMSP& item DECLARE_DEFAULT_LINK_NAME)override;
	void TreatMove(int X, int Y, int oldValue, int desiredValue, UIItem* sender)override;

	inline int				GetRemainingValue() const { return myRemainingValue; }
	void					SetRemainingValue(int number) { myRemainingValue = number; };
	inline int				Get_InitialGroupValue()const { return (int)myComunalValue; }
	void					Reset();

	kstl::vector<UISlider*> GetmySliderList() { return mySliderList; }

protected:
	virtual ~UISlidersGroup();
	void InitModifiable()override;


	kstl::vector<UISlider*>		mySliderList;
	maInt						myComunalValue; //value divided between the sliders
	unsigned int				mySliderNumber;
	int							myRemainingValue;
};

#endif //_UISLIDERSGROUP_H_