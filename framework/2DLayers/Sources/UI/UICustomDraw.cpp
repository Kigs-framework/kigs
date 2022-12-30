#include "UI/UICustomDraw.h"
#include "NotificationCenter.h"

using namespace Kigs::Draw2D;

IMPLEMENT_CLASS_INFO(UICustomDraw)


bool UICustomDraw::isAlpha(float X, float Y)
{
	if (mDelegate)
		return mDelegate->IsAlphafunc(X, Y);

	return false;
}


void UICustomDraw::ProtectedDraw(Scene::TravState* state)
{
	if (mDelegate)
		mDelegate->Drawfunc(state, this);
}