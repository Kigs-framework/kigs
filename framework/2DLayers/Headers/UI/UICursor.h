#ifndef _UICursor_H_
#define _UICursor_H_

#include "UIItem.h"

#include "AttributePacking.h"
#include "TouchInputEventManager.h"

// ****************************************
// * UICursor class
// * --------------------------------------
/**
* \file	UICursor.h
* \class	UICursor
* \ingroup 2DLayers
* \brief	???
*
*/
// ****************************************

class UICursor : public UIItem
{
public:
	DECLARE_CLASS_INFO(UICursor, UIItem, 2DLayers);
	DECLARE_CONSTRUCTOR(UICursor);

	bool ManageDirectTouchEvent(DirectTouchEvent& direct_touch);
	WRAP_METHODS(ManageDirectTouchEvent);

private :
	maReference mUIOwner=BASE_ATTRIBUTE(UIOwner,"");
};


#endif //_UICursor_H_
