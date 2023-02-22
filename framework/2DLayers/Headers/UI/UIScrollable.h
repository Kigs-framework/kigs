#pragma once

#include "UI/UIDrawableItem.h"
#include "AttributePacking.h"
#include "TouchInputEventManager.h"

namespace Kigs
{

	namespace Draw2D
	{
		// ****************************************
		// * UIScrollable class
		// * --------------------------------------
		/**
		* \file	UIScrollable.h
		* \class	UIScrollable
		* \ingroup 2DLayers
		* \brief	Base class for scrollable UI.
		*/
		// ****************************************

		class UIScrollable : public UIDrawableItem
		{
		public:
			DECLARE_CLASS_INFO(UIScrollable, UIDrawableItem, 2DLayers);
			DECLARE_INLINE_CONSTRUCTOR(UIScrollable) {}

			bool addItem(const CMSP& item, ItemPosition pos) override;
			bool removeItem(const CMSP& item) override;

			void SetScroll(v2f to);
			v2f GetCurrentScroll() const { return mCurrentScroll; }

		private:

			void InitModifiable() final;
			void Update(const Time::Timer&, void*) override;

			bool ManageScrollEvent(Input::ScrollEvent& scroll_event);

			bool			mVerticalScroll = true;
			bool			mHorizontalScroll = false;
			bool			mAdjustOnAddItem = true;
			bool			mAdjustOnRemoveItem = true;

			float			mThrowSpeedMult = 1.0f;
			float			mThrowFriction = 0.01f;

			v2f				mMinScroll = { 0.0f, 0.0f };
			v2f				mMaxScroll = { 0.0f, 0.0f };

			WRAP_ATTRIBUTES(mVerticalScroll, mHorizontalScroll, mAdjustOnAddItem, mAdjustOnRemoveItem, mThrowSpeedMult, mThrowFriction, mMinScroll, mMaxScroll);

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
	}
}