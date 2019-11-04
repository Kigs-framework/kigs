#ifndef _UIROUNDHUD_H_
#define _UIROUNDHUD_H_

#include "UIPanel.h"

class UIRoundHUD : public UIPanel
{
public:
	DECLARE_CLASS_INFO(UIRoundHUD, UIPanel, 2DLayers);
	DECLARE_CONSTRUCTOR(UIRoundHUD);

	SIGNALS(TouchUp, SlotChanged);

protected:
	virtual ~UIRoundHUD();
	void InitModifiable() override;
	void ProtectedDraw(TravState* state)override;
	//void Update(const Timer& timer, void* addParam) override;
	void NotifyUpdate(const unsigned int /* labelid */)override;
	bool addItem(CoreModifiable* item, ItemPosition pos = Last)override;
	bool removeItem(CoreModifiable* item)override;

	void SetTexUV(UIVerticesInfo * aQI) override{}
	void SetVertexArray(UIVerticesInfo * aQI) override {}
	void SetColor(UIVerticesInfo * aQI) override {}

	void UpdateSlots();

	bool ManageDirectTouchEvent(DirectTouchEvent& direct_touch);

	WRAP_METHODS(ManageDirectTouchEvent);
	

	maReference myLabel = BASE_ATTRIBUTE(Label);

	maFloat myRadius = BASE_ATTRIBUTE(Radius, 0.0f);
	maFloat myRadiusOffset = BASE_ATTRIBUTE(RadiusOffset, 0.0f);
	maFloat myActivationRadiusSqr = BASE_ATTRIBUTE(ActivationRadiusSqr, 100.0f);
	maFloat myAngleStart = BASE_ATTRIBUTE(AngleStart, 0.0f);
	maFloat myAngleWide = BASE_ATTRIBUTE(AngleWide, 360.0f);
	maInt mySlotCount = BASE_ATTRIBUTE(SlotCount, -1); // -1 for dynamic

	maBool myIsClockwise = BASE_ATTRIBUTE(IsClockwise, true);

	kstl::vector<CoreModifiable*> myItemList;
	
	int myRealSlotCount = 0;
	std::vector<v2f> mySlot;
	v2f myMidPoint = v2f(0.0f, 0.0f);
	bool myNeedUpdateSlots=true;
	bool isDown = false;

	int mySelectedSlot = -1;
	float voidZone = 0.0f;
};

#endif