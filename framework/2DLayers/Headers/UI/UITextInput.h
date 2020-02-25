#ifndef _UITEXTINPUT_H_
#define _UITEXTINPUT_H_

#include "UIDynamicText.h"
#include "maUSString.h"

#include "Platform/Input/KeyDefine.h"

#include "AttributePacking.h"

class Texture;

class UITextInput : public UIDynamicText
{
public:
	DECLARE_CLASS_INFO(UITextInput, UIDynamicText, 2DLayers);
	DECLARE_INLINE_CONSTRUCTOR(UITextInput) {}

	SIGNALS(TextChanged);

	kstl::string			GetReleaseAction() const {return myReleaseAction.c_str();}
	
	bool			HasFocus() override {return myFocus;}
	void			LoseFocus() override;
	void			GetFocus() override;

protected:
	
	void InitModifiable() override;
	virtual ~UITextInput();

	DECLARE_METHOD(FocusChanged);
	COREMODIFIABLE_METHODS(FocusChanged);
	WRAP_METHODS(UpdateKeyboard)

	void NotifyUpdate(const unsigned int labelid) override;
	bool ManageClickTouchEvent(ClickEvent& click_event) final;
	void UpdateKeyboard(kstl::vector<KeyEvent>& keys);

	bool					myIsCueBanner = false;

	maUSString				myCueBanner = BASE_ATTRIBUTE(CueBanner, usString(""));
	maString				myReleaseAction = BASE_ATTRIBUTE(ReleaseAction, "");

	kstl::vector<int>		myKeycodeList;
};

#endif //_UITEXTINPUT_H_