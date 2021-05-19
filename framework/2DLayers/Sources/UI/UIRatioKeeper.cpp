#include "UI/UIRatioKeeper.h"
#include "Base2DLayer.h"
#include "RenderingScreen.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIRatioKeeper, UIRatioKeeper, 2DLayers);
IMPLEMENT_CLASS_INFO(UIRatioKeeper)

IMPLEMENT_CONSTRUCTOR(UIRatioKeeper)
, mRatioIsOK(false)
{
	
}

void UIRatioKeeper::NotifyUpdate(const unsigned int labelid)
{
	if ((labelid == mSize.getLabelID()) ||
		(labelid == mAnchor.getLabelID()) ||
		(labelid == mPosition.getLabelID()) ||
		(labelid == mDock.getLabelID()) ||
		(labelid == mPreScale.getLabelID()) ||
		(labelid == mPostScale.getLabelID()))
		
	{
		// I don't want this values to be changed, so reset them
		mRatioIsOK = false;
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
		mSize = v2f(dsSize.x,dsSize.y);

		mRealSize = dsSize;

		mPosition = v2f{ 0,0 };
		mDock = v2f{ 0.5f, 0.5f };
		mAnchor = v2f{ 0.5f, 0.5f };
		
		mPostScale = v2f(1.0f,1.0f);
		mPreScale = v2f(1.0f, 1.0f);


		Point2D ratio(pxSize.x / dsSize.x, pxSize.y / dsSize.y);

		if (ratio.x < ratio.y)
		{
			mPreScale[1] = ratio.x / ratio.y;
		}
		else
		{
			mPreScale[0] = ratio.y / ratio.x;
		}

		SetNodeFlag(Node2D_NeedUpdatePosition);
		mRatioIsOK = true;
	}
}

void	UIRatioKeeper::Update(const Timer& a_Timer, void* addParam)
{
	if (!mRatioIsOK)
	{
		RecomputeRatio();
	}
}