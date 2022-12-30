#pragma once

#include "UITexturedItem.h"
#include "maUSString.h"
#include "AttributePacking.h"
#include "TouchInputEventManager.h"

namespace Kigs
{
	namespace Draw2D
	{
		// ****************************************
		// * UIButton class
		// * --------------------------------------
		/**
		* \file	UIButton.h
		* \class	UIButton
		* \ingroup 2DLayers
		* \brief	Simple Button class.
		*
		*/
		// ****************************************

		class UIButton : public UITexturedItem
		{
		public:
			DECLARE_CLASS_INFO(UIButton, UITexturedItem, 2DLayers);
			DECLARE_CONSTRUCTOR(UIButton);
			WRAP_METHODS(ManageClickTouchEvent, ManageDirectTouchEvent);
			SIGNALS(TouchUp, ClickUp, ClickDown, MouseOver, MouseOut, UnSelect);

			virtual void						ChangeState() {};

			bool								ManageClickTouchEvent(Input::ClickEvent& click_event);
			bool								ManageDirectTouchEvent(Input::DirectTouchEvent& direct_touch);

			[[deprecated("just don't")]]
			void								ForcedChangeState(bool isdown) { mIsDown = isdown; ChangeState(); }
			[[deprecated("connect to signals instead")]]
			bool								isPressed() const { return mIsDown; }
		protected:
			void								InitModifiable() override;
			void								NotifyUpdate(const unsigned int /* labelid */) override;

			bool								isAlpha(float X, float Y) override;

			void								AutoSize();

			maString							mClickDownAction;
			maString							mClickUpAction;
			maString							mMouseOverAction;
			maString							mMouseOutAction;
			maString							mUnSelectAction;
			maUSString							mParameter;
			maBool								mStayPressed;
			maBool								mKeepClickOutside;
			maBool								mDefaultPressed;
			maBool								mAutoResize;

			maBool								mUseHoverColor = BASE_ATTRIBUTE(UseHoverColor, false);
			maVect4DF							mHoverColor = BASE_ATTRIBUTE(HoverColor, 0.8f, 0.8f, 0.8f, 1.0f);
			maVect4DF							mIdleColor = BASE_ATTRIBUTE(IdleColor, 1, 1, 1, 1);
			maVect4DF							mClickedColor = BASE_ATTRIBUTE(ClickedColor, 0.5f, 0.5f, 0.5f, 1.0f);

			maBool								mSwallowEvents = BASE_ATTRIBUTE(SwallowEvents, false);

			bool								mInside;
			bool								mIsDown;
			bool								mIsMouseOver;
		};

	}
}