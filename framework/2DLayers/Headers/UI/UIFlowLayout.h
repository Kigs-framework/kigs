#ifndef _UIFlowLayout_H_
#define _UIFlowLayout_H_

#include "UILayout.h"

// Adjust the position of all children according to a flow layout algorithm

class UIFlowLayout : public UILayout
{
public:
	DECLARE_CLASS_INFO(UIFlowLayout, UILayout, 2DLayers);

	/**
	* \brief	constructor
	* \param	name : instance name
	* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	*/
	UIFlowLayout(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

protected:
	void		NotifyUpdate(const unsigned int labelid)override;
	void		RecomputeLayout()override;
	
	maVect2DI	myPadding;
	maBool		mySortByPriority;
	maBool		myRescaleToFit;
};
#endif //_UIFlowLayout_H_