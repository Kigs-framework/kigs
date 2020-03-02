#ifndef _UIRATIOKEEPER_H_
#define _UIRATIOKEEPER_H_

#include "UIItem.h"
#include "UIDrawableItem.h"
#include "AttributePacking.h"


class RenderingScreen;

// create a zone in screen where design size ratio is kept
// add this to root uiitem

class UIRatioKeeper : public UIDrawableItem
{
public:
	DECLARE_CLASS_INFO(UIRatioKeeper, UIDrawableItem, 2DLayers);
	DECLARE_CONSTRUCTOR(UIRatioKeeper);

	void ForceRecomputeRatio()
	{
		myRatioIsOK = false;
	}

protected:

	void NotifyUpdate(const unsigned int /* labelid */) override;
	void Update(const Timer& a_Timer, void* addParam) override;

	void RecomputeRatio();

	bool myRatioIsOK;

	WRAP_METHODS(ForceRecomputeRatio);

	RenderingScreen* mCurrentScreen = nullptr;
};

#endif //_UIRATIOKEEPER_H_