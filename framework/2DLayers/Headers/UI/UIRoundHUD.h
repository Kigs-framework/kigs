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


			bool							mIsClockwise = true;
			s32								mSlotCount = -1; // -1 for dynamic

			float							mRadius = 0.0f;
			float							mRadiusOffset = 0.0f;
			float							mActivationRadiusSqr = 100.0f;
			float							mAngleStart = 0.0f;
			float							mAngleWide = 360.0f;

			std::weak_ptr<CoreModifiable>	mLabel;

			WRAP_ATTRIBUTES(mIsClockwise, mSlotCount, mRadius, mRadiusOffset, mActivationRadiusSqr, mAngleStart, mAngleWide, mLabel);

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