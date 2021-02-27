#pragma once
#include "UI\UIButtonImage.h"
#include "UI\UIText.h"
#include <math.h>

// ****************************************
// * UIPointInteret class
// * --------------------------------------
/**
* \file	UIPointInteret.h
* \class	UIPointInteret
* \ingroup Geoloc
* \brief	 An UIPointInteret is placed on the map to mark an point of interest
* \author	König Jolan
* \version ukn
* \date	14/02/16
*
* The point is placed with GPS coordonates, it is represented by an image. You can choose the condition of activation (distance with user or click on the point),
* and the actions (stack the sequence, show pop up, or show vignette).
* 
*/
// ****************************************

class PointInteretVignette;

class UIPointInteret :
	public UIButtonImage
{
public:
	DECLARE_CLASS_INFO(UIPointInteret, UIButtonImage, 2DLayers);


	UIPointInteret(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	void ActivePoint(bool);
	void ActivePoint(kdouble, kdouble);
	bool Get_VignetteIsHidden();
	kdouble DistanceToUser(kdouble _lat, kdouble _long);
	void Get_TitleAndDescription(kstl::string &title, kstl::string &Desc) { title = mNameOfInterestPoint; Desc = mDescriptionOfInterestPoint; }


	bool ClickAction(CoreModifiable* sender, usString param);
	WRAP_METHODS(ClickAction);

protected:
	void    ProtectedDestroy()override;
	void NotifyUpdate(const unsigned int /* labelid */)override;
	void InitModifiable()override;
	//bool TriggerMouseClick(int buttonState, int buttonEvent, int X, int Y, bool & catchClick)override;
	DECLARE_VIRTUAL_METHOD(HideVignette);
	COREMODIFIABLE_METHODS(HideVignette);
	virtual  void SetParent(CoreModifiable* value);

	SP<UIText> mNameText;

	SP<PointInteretVignette> mVignette;
	maString mVignetteParameters;

	void RecalculatePosition();

	maDouble mCoordGPS_Lat;
	maDouble mCoordGPS_Long;

	maString mNameOfInterestPoint;
	maString mDescriptionOfInterestPoint;

	maString mConditionOfActivation;
	maInt mDistanceOfActivation;

	maString mActionOfActivation;
	maString mActionOfDeactivation;
	maUSString mParameterOfActivation;
	maUSString mParameterOfDesactivation;

	maBool mShowTitle;
	maVect4DF mTitleColor;
	maString mTitleFontName;
	maInt mTitleFontSize;

	maBool mFeedbackOnActivation;

	bool mCanBeActivatedByClick;
	bool mIsTitleShown;
	bool mInactivate;
};

