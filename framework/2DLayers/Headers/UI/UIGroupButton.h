#ifndef _UIGROUPBUTTON_H_
#define _UIGROUPBUTTON_H_

#include "UIItem.h"
#include "maUSString.h"
class UIButton;

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

	bool	PermissionToClicRequiredFromParent(bool toPressed, UIItem* sender)override;

	void	TreatClick(bool toPressed, UIItem* sender)override;
	bool	addItem(CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME)override;
	bool	removeItem(CMSP& item DECLARE_DEFAULT_LINK_NAME)override;

	void			reComputeSize();
protected:
	virtual ~UIGroupButton();
	void InitModifiable()override;
	void	computeSize(int _buttonNumber, CoreModifiable* a_item);


	kstl::vector<UIButton*>		myButtonList;
	maBool						myKeepOnePressed;
	maBool						myOnlyOnePressed;
	maString					myNoSelectedAction;
	maUSString					myParameter;
	unsigned int				myButtonNumber;
};

#endif //_UIGROUPBUTTON_H_