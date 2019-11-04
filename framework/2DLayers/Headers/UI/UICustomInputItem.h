#pragma once

#include "UI/UIItem.h"
#include "AttributePacking.h"

class UICustomInputItem : public UIItem
{
public:
	DECLARE_CLASS_INFO(UICustomInputItem, UIItem, 2DLayers);
	DECLARE_INLINE_CONSTRUCTOR(UICustomInputItem){}

protected:
	void InitModifiable();

};


