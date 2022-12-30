#pragma once

#include "UIDrawableItem.h"

namespace Kigs
{

	namespace Draw2D
	{
		// ****************************************
		// * UIRoundHUD class
		// * --------------------------------------
		/**
		* \file	UIRoundHUD.h
		* \class	UIRoundHUD
		* \ingroup 2DLayers
		* \brief	Display a circular menu.
		*/
		// ****************************************

		class UIRoundHUD : public UIDrawableItem
		{
		public:
			DECLARE_CLASS_INFO(UIRoundHUD, UIDrawableItem, 2DLayers);
			DECLARE_CONSTRUCTOR(UIRoundHUD);
			virtual ~UIRoundHUD();
			SIGNALS(TouchUp, SlotChanged);

		protected:

			void InitModifiable() override;
			void ProtectedDraw(Scene::TravState* state)override;
			//void Update(const Timer& timer, void* addParam) override;
			void NotifyUpdate(const unsigned int /* labelid */)override;
			bool addItem(const CMSP& item, ItemPosition pos = Last)override;
			bool removeItem(const CMSP& item)override;

			void SetTexUV(UIVerticesInfo* aQI) override {}
			void SetVertexArray(UIVerticesInfo* aQI) override {}
			void SetColor(UIVerticesInfo* aQI) override {}

			void UpdateSlots();

			bool ManageDirectTouchEvent(Input::DirectTouchEvent& direct_touch);

			WRAP_METHODS(ManageDirectTouchEvent);


			maReference mLabel = BASE_ATTRIBUTE(Label);

			maFloat mRadius = BASE_ATTRIBUTE(Radius, 0.0f);
			maFloat mRadiusOffset = BASE_ATTRIBUTE(RadiusOffset, 0.0f);
			maFloat mActivationRadiusSqr = BASE_ATTRIBUTE(ActivationRadiusSqr, 100.0f);
			maFloat mAngleStart = BASE_ATTRIBUTE(AngleStart, 0.0f);
			maFloat mAngleWide = BASE_ATTRIBUTE(AngleWide, 360.0f);
			maInt mSlotCount = BASE_ATTRIBUTE(SlotCount, -1); // -1 for dynamic

			maBool mIsClockwise = BASE_ATTRIBUTE(IsClockwise, true);

			std::vector<CoreModifiable*> mItemList;

			int mRealSlotCount = 0;
			std::vector<v2f> mSlot;
			v2f mMidPoint = v2f(0.0f, 0.0f);
			bool mNeedUpdateSlots = true;
			bool mIsDown = false;

			int mSelectedSlot = -1;
			float mVoidZone = 0.0f;
		};

	}
}