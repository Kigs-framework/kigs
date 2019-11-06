#ifndef _UIGridLayout_H_
#define _UIGridLayout_H_

#include "UILayout.h"

// Adjust the position of all children according to a grid layout algorithm

class UIGridLayout : public UILayout
{
public:
	DECLARE_CLASS_INFO(UIGridLayout, UILayout, 2DLayers);

	/**
	* \brief	constructor
	* \param	name : instance name
	* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	*/
	UIGridLayout(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	
protected:
	void			NotifyUpdate(const unsigned int labelid)override;
	void			RecomputeLayout()override;
	
	//maUInt					myRows;
	maUInt					myColumns;
	maUInt					myRowHeight;
	maUInt					myColumnWidth;
	maBool					myHeaderRow;
	maVect2DI				myPadding;
	maBool					mySortByPriority;
	maBool					myResizeElements;
	maBool					myResizeMe;
};

#endif //_UIGridLayout_H_