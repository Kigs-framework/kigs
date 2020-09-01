#ifndef _UIRENDERINGSCREEN_H_
#define _UIRENDERINGSCREEN_H_

#include "UITexturedItem.h"
#include "AttributePacking.h"
#include "TouchInputEventManager.h"

// ****************************************
// * UIRenderingScreen class
// * --------------------------------------
/**
* \file	UIRenderingScreen.h
* \class	UIRenderingScreen
* \ingroup 2DLayers
* \brief	A Textured UI where the texture display a Rendering Screen 
*/
// ****************************************

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

	maReference							mRenderingScreen = BASE_ATTRIBUTE(RenderingScreen);
	maBool								mForceNearest = BASE_ATTRIBUTE(ForceNearest, false);
	
};
 
#endif //_UIRENDERINGSCREEN_H_