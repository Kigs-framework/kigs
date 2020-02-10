#ifndef _UIBUTTON_H_
#define _UIBUTTON_H_

#include "UITexturedItem.h"
#include "maUSString.h"
#include "AttributePacking.h"
#include "TouchInputEventManager.h"

class UIButton : public UITexturedItem
{
public:
	DECLARE_CLASS_INFO(UIButton, UITexturedItem, 2DLayers);
	DECLARE_CONSTRUCTOR(UIButton);
	WRAP_METHODS(ManageClickTouchEvent, ManageDirectTouchEvent);
	SIGNALS(TouchUp, ClickUp, ClickDown, MouseOver, MouseOut, UnSelect);
	
	virtual void						ChangeState() {} ;

	bool								ManageClickTouchEvent(ClickEvent& click_event);
	bool								ManageDirectTouchEvent(DirectTouchEvent& direct_touch);
	
	[[deprecated("just don't")]]
	void								ForcedChangeState(bool isdown) { myIsDown = isdown; ChangeState(); }
	[[deprecated("connect to signals instead")]]
	bool								isPressed() const { return myIsDown; }
protected:
	void								InitModifiable() override;
	void								NotifyUpdate(const unsigned int /* labelid */) override;

	bool								isAlpha(float X, float Y) override;

	void								AutoSize();

	maString							myClickDownAction;
	maString							myClickUpAction;
	maString							myMouseOverAction;
	maString							myMouseOutAction;
	maString							myUnSelectAction;
	maUSString							myParameter;
	maBool								myStayPressed;
	maBool								mybKeepClickOutside;
	maBool								myDefaultPressed;
	maBool								myAutoResize;

	maBool								mUseHoverColor = BASE_ATTRIBUTE(UseHoverColor, false);
	maVect4DF							mHoverColor = BASE_ATTRIBUTE(HoverColor, 0.8f, 0.8f, 0.8f, 1.0f);
	maVect4DF							mIdleColor = BASE_ATTRIBUTE(IdleColor, 1, 1, 1, 1);
	maVect4DF							mClickedColor = BASE_ATTRIBUTE(ClickedColor, 0.5f, 0.5f, 0.5f, 1.0f);

	bool								mybInside;
	bool								myIsDown;
	bool								myIsMouseOver;
};

#endif