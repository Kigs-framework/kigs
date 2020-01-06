#ifndef _UICUSTOMDRAW_H_
#define _UICUSTOMDRAW_H_

#include "UIItem.h"
#include "maUSString.h"

class UICustomDrawDelegate
{
public:
	 virtual void Drawfunc(TravState*, UIItem*) = 0;
	 virtual bool IsAlphafunc(float X, float Y){return false;}
};

class UICustomDraw : public UIItem
{
public:
	DECLARE_CLASS_INFO(UICustomDraw,UIItem,2DLayers);
	DECLARE_INLINE_CONSTRUCTOR(UICustomDraw) {}

	void								GetColor(kfloat &R,kfloat &G,kfloat &B, kfloat &A) { R=myColor[0]; G=myColor[1]; B=myColor[2]; A=GetOpacity(); }
	inline UICustomDrawDelegate*		GetDelegate(){return myDelegate;}
	inline void							SetDelegate(UICustomDrawDelegate* a_value){myDelegate = a_value;}

protected:
	bool								isAlpha(float X, float Y) override;
	bool								TriggerMouseMove(bool over, float MouseDeltaX, float MouseDeltaY) override;
	bool								TriggerMouseClick(int buttonState, int buttonEvent, int X, int Y, bool & catchClick) override;

	void								ProtectedDraw(TravState* state) override;

	UICustomDrawDelegate*				myDelegate = nullptr;
	maString							myClickDownAction = BASE_ATTRIBUTE(ClickDownAction, "");
	maString							myClickUpAction = BASE_ATTRIBUTE(ClickUpAction, "");
	maUSString							myParameter = BASE_ATTRIBUTE(Parameter, (kstl::string)"");
	maBool								myStayPressed = BASE_ATTRIBUTE(StayPressed, false);
	bool								myIsDown = false;
	bool								myIsMouseOver = false;
	bool								mybInside = false;
};

#endif //_UICUSTOMDRAW_H_