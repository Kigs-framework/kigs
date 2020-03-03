#pragma once
#include "UI/UIItem.h"

#include "Upgrador.h"


// ****************************************
// * UIPopUp Upgrador
// * --------------------------------------
/**
* \file	UIPopUp.h
* \class	UIPopUp
* \ingroup 2DLayers
* \brief	This is the upgrador class for a pop-up
*
* A pop up appear when there is a notification "ShowPopUp" with numSignalPopUp as parameter and disapear when there is a notification "HidePopUp",
* or when the time in TimeStayOpen is over.
*
*/
// ****************************************

class PopUpUpgrador : public Upgrador<UIItem>
{
public:

	// create and init Upgrador if needed and add dynamic attributes
	virtual void	Init(CoreModifiable* toUpgrade) override;

	// destroy UpgradorData and remove dynamic attributes 
	virtual void	Destroy(CoreModifiable* toDowngrade) override;

	START_UPGRADOR(PopUpUpgrador);
	UPGRADOR_METHODS(HidePopUp, ShowPopUp);

protected:	

	void Show(UIItem* localthis,CoreModifiable* aActivator);
	void Hide(UIItem* localthis);

	// upgrador member variable
	kdouble myTimeOpen = 0;
	bool myOpenPopup = false;
	CoreModifiable* myActivator = nullptr;
};

