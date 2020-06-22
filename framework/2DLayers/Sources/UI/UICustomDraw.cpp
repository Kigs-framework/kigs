#include "UI/UICustomDraw.h"
#include "NotificationCenter.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UICustomDraw, UICustomDraw, 2DLayers);
IMPLEMENT_CLASS_INFO(UICustomDraw)


bool UICustomDraw::isAlpha(float X, float Y)
{
	if (myDelegate)
		return myDelegate->IsAlphafunc(X, Y);

	return false;
}


void UICustomDraw::ProtectedDraw(TravState* state)
{
	if (myDelegate)
		myDelegate->Drawfunc(state, this);
}