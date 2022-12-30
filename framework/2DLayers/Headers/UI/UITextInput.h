#pragma once

#include "UIDynamicText.h"
#include "maUSString.h"

#include "Platform/Input/KeyDefine.h"

#include "AttributePacking.h"

namespace Kigs
{
	namespace Draw
	{
		class Texture;
	}
	namespace Draw2D
	{

		// ****************************************
		// * UITextInput class
		// * --------------------------------------
		/**
		* \file	UITextInput.h
		* \class	UITextInput
		* \ingroup 2DLayers
		* \brief	Text input UI item.
		*
		*  Use UIDynamicText where UITextArea uses UIDrawableItem directly
		*
		*/
		// ****************************************

		class UITextInput : public UIDynamicText
		{
		public:
			DECLARE_CLASS_INFO(UITextInput, UIDynamicText, 2DLayers);
			DECLARE_INLINE_CONSTRUCTOR(UITextInput) {}
			virtual ~UITextInput();
			SIGNALS(TextChanged);

			std::string			GetReleaseAction() const { return mReleaseAction.c_str(); }

			bool			HasFocus() override { return GetNodeFlag(UIItem_HasFocus); }
			void			LoseFocus() override;
			void			GetFocus() override;

		protected:

			void InitModifiable() override;


			DECLARE_METHOD(FocusChanged);
			COREMODIFIABLE_METHODS(FocusChanged);
			WRAP_METHODS(UpdateKeyboard)

				void NotifyUpdate(const unsigned int labelid) override;
			bool ManageClickTouchEvent(Input::ClickEvent& click_event) final;
			void UpdateKeyboard(std::vector<KeyEvent>& keys);

			bool					mIsCueBanner = false;

			maUSString				mCueBanner = BASE_ATTRIBUTE(CueBanner, usString(""));
			maString				mReleaseAction = BASE_ATTRIBUTE(ReleaseAction, "");

			std::vector<int>		mKeycodeList;
		};

	}
}