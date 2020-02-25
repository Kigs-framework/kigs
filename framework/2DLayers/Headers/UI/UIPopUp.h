#pragma once
#include "UI/UIItem.h"

// ****************************************
// * UIPopUp class
// * --------------------------------------
/**
* \file	UIPopUp.h
* \class	UIPopUp
* \ingroup 2DLayers
* \brief	This is the class for a pop-up
* \author	König Jolan
* \version ukn
* \date	04/07/16
*
* A pop up appear when there is a notification "ShowPopUp" with numSignalPopUp as parameter and disapear when there is a notification "HidePopUp",
* or when the time in TimeStayOpen is over. 
*
*/
// ****************************************

class UIPopUp : public UIItem
{
public:
	DECLARE_CLASS_INFO(UIPopUp, UIItem, 2DLayers);
	DECLARE_CONSTRUCTOR(UIPopUp);

	DECLARE_VIRTUAL_METHOD(HidePopUp);
	DECLARE_VIRTUAL_METHOD(ShowPopUp);
	COREMODIFIABLE_METHODS(HidePopUp, ShowPopUp);

protected:
	virtual ~UIPopUp();
	void Update(const Timer&  timer, void* addParam) override;

	void Show(CoreModifiable * aActivator);
	void Hide();

	maInt myNumSignalPopUp = BASE_ATTRIBUTE(NumSignal, 0);
	maVect3DF myActiveColor = BASE_ATTRIBUTE(ActiveColor, 0.2f, 1.0f, 0.2f);
	maVect3DF myUsedColor = BASE_ATTRIBUTE(UsedColor, 0.4f, 0.4f, 0.4f);
	maBool myCloseAll = BASE_ATTRIBUTE(CloseAll, true);
	maInt myTimeStayOpen = BASE_ATTRIBUTE(StayOpen, 0);

	kdouble myTimeOpen = 0;
	bool myOpenPopup = false;

	CoreModifiable * myActivator = nullptr;
};

