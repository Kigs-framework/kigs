#ifndef _UIBoxLayout_H_
#define _UIBoxLayout_H_

#include "UILayout.h"

// Adjust the position of all children according to a flow layout algorithm

class UIBoxLayout : public UILayout
{
public:
	DECLARE_CLASS_INFO(UIBoxLayout, UILayout, 2DLayers);
	DECLARE_CONSTRUCTOR(UIBoxLayout);

protected:
	void	NotifyUpdate(const unsigned int labelid)override;
	void	RecomputeLayout()override;

	maVect2DI				myPadding = BASE_ATTRIBUTE(Padding, 0, 0);
	maBool					mySortByPriority = BASE_ATTRIBUTE(SortByPriority, false);
	maBool					myVertical = BASE_ATTRIBUTE(Vertical, false);

	/**
	* 0 = Left/Top, 1 = Center, 2 = Right/Bottom
	*/
	maInt					myAlignment = BASE_ATTRIBUTE(Alignment, false);
	maBool					myResizeLayoutX = BASE_ATTRIBUTE(ResizeLayoutX, false);
	maBool					myResizeLayoutY = BASE_ATTRIBUTE(ResizeLayoutY, false);

};

#endif //_UIBoxLayout_H_