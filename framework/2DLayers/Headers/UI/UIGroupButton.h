#ifndef _UIGROUPBUTTON_H_
#define _UIGROUPBUTTON_H_

#include "UIItem.h"
#include "maUSString.h"
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
	UIGroupButton(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	bool	addItem(const CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME)override;
	bool	removeItem(const CMSP& item DECLARE_DEFAULT_LINK_NAME)override;

	void			reComputeSize();
protected:
	virtual ~UIGroupButton();
	void InitModifiable()override;
	void	computeSize(int _buttonNumber, CoreModifiable* a_item);


	kstl::vector<UIButton*>		mButtonList;
	maBool						mKeepOnePressed;
	maBool						mOnlyOnePressed;
	maString					mNoSelectedAction;
	maUSString					mParameter;
	unsigned int				mButtonNumber;
};

#endif //_UIGROUPBUTTON_H_