#include "UI/UIRatioKeeper.h"
#include "Base2DLayer.h"
#include "RenderingScreen.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIRatioKeeper, UIRatioKeeper, 2DLayers);
IMPLEMENT_CLASS_INFO(UIRatioKeeper)

IMPLEMENT_CONSTRUCTOR(UIRatioKeeper)
, myRatioIsOK(false)
{
	
}

void UIRatioKeeper::NotifyUpdate(const unsigned int labelid)
{
	if ((labelid == mySizeX.getLabelID()) ||
		(labelid == mySizeY.getLabelID()) ||
		(labelid == myAnchor.getLabelID()) ||
		(labelid == myPosition.getLabelID()) ||
		(labelid == myDock.getLabelID()) ||
		(labelid == myPreScaleX.getLabelID()) ||
		(labelid == myPreScaleY.getLabelID()) ||
		(labelid == myPostScaleX.getLabelID()) ||
		(labelid == myPostScaleY.getLabelID()))
	{
		// I don't want this values to be changed, so reset them
		myRatioIsOK = false;
	}
	ParentClassType::NotifyUpdate(labelid);

}


void	UIRatioKeeper::RecomputeRatio()
{
	Abstract2DLayer* parent = getRootLayerFather();
	if (!parent)
	{
		if(mCurrentScreen)
			KigsCore::Disconnect(mCurrentScreen, "Resized", this, "ForceRecomputeRatio");
		mCurrentScreen = nullptr;
	}
	else
	{
		RenderingScreen* currentScreen = parent->GetRenderingScreen();

		if (currentScreen != mCurrentScreen)
		{
			if(mCurrentScreen)
				KigsCore::Disconnect(mCurrentScreen, "Resized", this, "ForceRecomputeRatio");

			KigsCore::Connect(currentScreen, "Resized", this, "ForceRecomputeRatio");
		}
		mCurrentScreen = currentScreen;

		Point2D pxSize;
		Point2D dsSize;

		currentScreen->GetSize(pxSize.x, pxSize.y);
		currentScreen->GetDesignSize(dsSize.x, dsSize.y);

		// reset default
		mySizeX = dsSize.x;
		mySizeY = dsSize.y;

		myRealSize = dsSize;

		myPosition = v2f{ 0,0 };
		myDock = v2f{ 0.5f, 0.5f };
		myAnchor = v2f{ 0.5f, 0.5f };
		
		myPostScaleX = 1.0f;
		myPostScaleY = 1.0f;
		myPreScaleX = 1.0f;
		myPreScaleY = 1.0f;


		Point2D ratio(pxSize.x / dsSize.x, pxSize.y / dsSize.y);

		if (ratio.x < ratio.y)
		{
			myPreScaleY = ratio.x / ratio.y;
		}
		else
		{
			myPreScaleX = ratio.y / ratio.x;
		}

		myNeedUpdatePosition = true;
		myRatioIsOK = true;
	}
}

void	UIRatioKeeper::Update(const Timer& a_Timer, void* addParam)
{
	if (!myRatioIsOK)
	{
		RecomputeRatio();
	}
}