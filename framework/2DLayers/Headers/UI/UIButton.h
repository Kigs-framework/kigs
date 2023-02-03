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

			std::string							mClickDownAction = "";
			std::string							mClickUpAction = "";
			std::string							mMouseOverAction = "";
			std::string							mMouseOutAction = "";
			std::string							mUnSelectAction = "";
			usString							mParameter = std::string("");
			bool								mStayPressed = false;
			bool								mKeepClickOutside = false;
			bool								mDefaultPressed = false;
			bool								mAutoResize = true;
			bool								mSwallowEvents = false;
			bool								mUseHoverColor = false;
			v4f									mHoverColor = { 0.8f, 0.8f, 0.8f, 1.0f };
			v4f									mIdleColor = { 1, 1, 1, 1 };
			v4f									mClickedColor = { 0.5f, 0.5f, 0.5f, 1.0f };

			WRAP_ATTRIBUTES(mClickDownAction, mClickUpAction, mMouseOverAction, mMouseOutAction, mUnSelectAction, mParameter, mStayPressed, mKeepClickOutside,
				mDefaultPressed, mAutoResize, mSwallowEvents, mUseHoverColor, mHoverColor, mIdleColor, mClickedColor);

			bool								mInside;
			bool								mIsDown;
			bool								mIsMouseOver;
		};

	}
}