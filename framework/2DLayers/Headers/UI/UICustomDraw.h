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

// ****************************************
// * UICustomDraw class
// * --------------------------------------
/**
* \file	UICustomDraw.h
* \class	UICustomDraw
* \ingroup 2DLayers
* \brief	Draw non UI objects.
*
*/
// ****************************************

class UICustomDraw : public UIItem
{
public:
	DECLARE_CLASS_INFO(UICustomDraw,UIItem,2DLayers);
	DECLARE_INLINE_CONSTRUCTOR(UICustomDraw) {}

	inline UICustomDrawDelegate*		GetDelegate(){return mDelegate;}
	inline void							SetDelegate(UICustomDrawDelegate* a_value){mDelegate = a_value;}

protected:
	bool								isAlpha(float X, float Y) override;
	
	void								ProtectedDraw(TravState* state) override;

	UICustomDrawDelegate*				mDelegate = nullptr;
	maString							mClickDownAction = BASE_ATTRIBUTE(ClickDownAction, "");
	maString							mClickUpAction = BASE_ATTRIBUTE(ClickUpAction, "");
	maUSString							mParameter = BASE_ATTRIBUTE(Parameter, (kstl::string)"");
	maBool								mStayPressed = BASE_ATTRIBUTE(StayPressed, false);
	bool								mIsDown = false;
	bool								mIsMouseOver = false;
	bool								mInside = false;
};

#endif //_UICUSTOMDRAW_H_