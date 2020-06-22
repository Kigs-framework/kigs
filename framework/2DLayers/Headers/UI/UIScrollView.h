#ifndef _FEUILLETONGROUP_H_
#define _FEUILLETONGROUP_H_

#include "UI/UIControlBoxForScrollViewUI.h"
#include "UIDrawableItem.h"
#include "RenderingScreen.h"
#include "RendererIncludes.h"
#include "RendererDefines.h"
#include "ModuleRenderer.h"
#include "RenderingScreen.h"
#include "ModuleInput.h"


class UIScrollView : public UIDrawableItem
{

public:
	DECLARE_CLASS_INFO(UIScrollView, UIDrawableItem, Renderer);

	UIScrollView(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	~UIScrollView();

	bool scrollTo(kfloat deltaPos);	
	bool Draw(TravState* state)override;

	// void NotifyUpdate(unsigned int);
	bool isVertical;
	void SendClickUpToChildren(int buttonState, int buttonEvent, int X, int Y, bool & catchClick);
	void SendClickDownToChildren(int buttonState, int buttonEvent, int X, int Y, bool & catchClick);
	void SendFalseClickUpToChildren();
	//void SendMouseMoveToChildren(bool over,float MouseDeltaX, float MouseDeltaY);

protected:
	void InitModifiable()override;
	void SetUpNodeIfNeeded()override;
	void UpdateContentSize();

private:

	kfloat	xScroll, yScroll;
	int maxY, maxX;
	SP<UIControlBoxForScrollViewUI> myVision;
	ModuleInput* myInput;

};

#endif // _FEUILLETONGROUP_H_