#ifndef _UICursor_H_
#define _UICursor_H_

#include "UIItem.h"

#include "AttributePacking.h"
#include "TouchInputEventManager.h"


class UICursor : public UIItem
{
public:
	DECLARE_CLASS_INFO(UICursor, UIItem, 2DLayers);
	DECLARE_CONSTRUCTOR(UICursor);

	bool ManageDirectTouchEvent(DirectTouchEvent& direct_touch);
	WRAP_METHODS(ManageDirectTouchEvent);

private :
	maReference myUIOwner=BASE_ATTRIBUTE(UIOwner,"");
};


#endif //_UICursor_H_
