#ifndef _UIRENDERINGSCREEN_H_
#define _UIRENDERINGSCREEN_H_

#include "UITexturedItem.h"
#include "AttributePacking.h"
//#include "ModuleInput.h"
#include "TouchInputEventManager.h"

class UIRenderingScreen : public UITexturedItem
{
public:
	DECLARE_CLASS_INFO(UIRenderingScreen, UITexturedItem, 2DLayers);
	DECLARE_CONSTRUCTOR(UIRenderingScreen);

	virtual bool GetDataInTouchSupport(const touchPosInfos& posin, touchPosInfos& pout);
	WRAP_METHODS(GetDataInTouchSupport);

protected:
	void								InitModifiable() override;
	void								NotifyUpdate(unsigned int labelid) override;
	void								SetTexUV(UIVerticesInfo * aQI) override;

	maReference							myRenderingScreen = BASE_ATTRIBUTE(RenderingScreen);
	maBool								myForceNearest = BASE_ATTRIBUTE(ForceNearest, false);
	
};
 
#endif //_UIRENDERINGSCREEN_H_