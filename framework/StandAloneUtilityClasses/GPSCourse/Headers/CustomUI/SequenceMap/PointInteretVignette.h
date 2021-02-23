#pragma once
#include "UI\UIImage.h"

/*#include <iostream>
#include <vector>
#include "UI\UIText.h"
#include "CustomUI\SequenceMap\UIPointInteret.h"
#include "UI\UIMultilineText.h"*/

// ****************************************
// * PointInteretVignette class
// * --------------------------------------
/**
* \file	PointInteretVignette.h
* \class	PointInteretVignette
* \ingroup Geoloc
* \brief	 An PointInteretVignette is a pop-up created from a model,  who give a description of a point of interest. 
* \author	König Jolan
* \version ukn
* \date	14/02/16
*
* The vignette can contain some UIs, as image or text, who are then configure in the point of interest
*
*/
// ****************************************

class UIPointInteret;
class UIText;
class UIPointInteret;
class UIButtonImage;


class PointInteretVignette :
	public UIImage
{
public:
	DECLARE_CLASS_INFO(PointInteretVignette, UIImage, 2DLayers);
	DECLARE_CONSTRUCTOR(PointInteretVignette);

	void SetVignetteTexts(kstl::string _Name, kstl::string _Description);
	void PositionVignette();
	void PositionTitle();
	void PositionDescription();
	void PositionExitButton();
	void SetPointInteret(UIPointInteret *_myPoint) { mPointOfInterest = _myPoint; }
	void SetParameters(kstl::string params);

protected:
	void NotifyUpdate(const unsigned int /* labelid */) override;
	void InitModifiable() override;
	
	UIPointInteret* mPointOfInterest;
	SP<UIText> mNamePointInteretTextShow;
	SP<UIText> mDescriptionPointInteretTextShow;
	SP<UIButtonImage> mExitButton;

	kstl::string mNameOfInterestPoint;
	kstl::string mDescriptionOfInterestPoint;

	maString mPositionOnWindow;

	maString mFontTitleName;
	maInt mTitleFontSize;
	maVect4DF mTitleColor;

	maString mFontDescriptionName;
	maInt mDescriptionFontSize;
	maInt mDescriptionAlignment;
	maInt mDescriptionPositionY;
	maInt mDescriptionMaxSizeX;
	maVect4DF mDescriptionColor;


	maString mExitButtonTextureUp;
	maString mExitButtonTextureOver;
	maString mExitButtonTextureDown;


};

