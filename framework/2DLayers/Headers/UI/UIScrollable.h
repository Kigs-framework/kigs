#pragma once

#include "UI/UIPanel.h"
#include "AttributePacking.h"
#include "TouchInputEventManager.h"

class UIScrollable : public UIPanel
{
public:
	DECLARE_CLASS_INFO(UIScrollable, UIPanel, 2DLayers);
	DECLARE_INLINE_CONSTRUCTOR(UIScrollable) {}
	
	bool addItem(CoreModifiable* item, ItemPosition pos) override;
	bool removeItem(CoreModifiable* item) override;

	void SetScroll(v2f to);
	v2f GetCurrentScroll() const { return mCurrentScroll; }

private:
	
	void InitModifiable() final;
	void Update(const Timer&, void*) override;

	bool ManageScrollEvent(ScrollEvent& scroll_event);

	maBool mVerticalScroll = BASE_ATTRIBUTE(VerticalScroll, true);
	maBool mHorizontalScroll = BASE_ATTRIBUTE(HorizontalScroll, false);
	maBool mAdjustOnAddItem = BASE_ATTRIBUTE(AdjustOnAddItem, true);
	maBool mAdjustOnRemoveItem = BASE_ATTRIBUTE(AdjustOnRemoveItem, true);
	
	maFloat mThrowSpeedMult = BASE_ATTRIBUTE(ThrowSpeedMult, 1.0f);
	maFloat mThrowFriction = BASE_ATTRIBUTE(ThrowFriction, 0.01f);

	maVect2DF mMinScroll = BASE_ATTRIBUTE(MinScroll, 0, 0);
	maVect2DF mMaxScroll = BASE_ATTRIBUTE(MaxScroll, 0, 0);

	maEnum<4> mMinScrollModeX = BASE_ATTRIBUTE(MinScrollModeX, "NotUsed", "Default", "Multiply", "Add");
	maEnum<4> mMaxScrollModeX = BASE_ATTRIBUTE(MaxScrollModeX, "NotUsed", "Default", "Multiply", "Add");

	maEnum<4> mMinScrollModeY = BASE_ATTRIBUTE(MinScrollModeY, "NotUsed", "Default", "Multiply", "Add");
	maEnum<4> mMaxScrollModeY = BASE_ATTRIBUTE(MaxScrollModeY, "NotUsed", "Default", "Multiply", "Add");


	v2f mCurrentScroll = { 0,0 };
	v2f mLastOffset = { 0,0 };
	v2f mThrowSpeed = { 0, 0 };

	double mLastTime = 0.0;

	WRAP_METHODS(ManageScrollEvent, SetScroll);
};