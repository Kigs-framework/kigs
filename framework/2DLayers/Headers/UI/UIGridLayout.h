#ifndef _UIGridLayout_H_
#define _UIGridLayout_H_

#include "UILayout.h"

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
	
	maUInt					mColumns;
	maUInt					mRowHeight;
	maUInt					mColumnWidth;
	maBool					mHeaderRow;
	maVect2DI				mPadding;
	maBool					mSortByPriority;
	maBool					mResizeElements;
	maBool					mAutoResize;
};

#endif //_UIGridLayout_H_