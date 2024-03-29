#pragma once

#include "UIDrawableItem.h"
#include "maUSString.h"

namespace Kigs
{
	namespace Draw
	{
		class Texture;
	}
	namespace Draw2D
	{

		class UIVerticesInfo;

		// ****************************************
		// * UITextArea class
		// * --------------------------------------
		/**
		* \file	UITextArea.h
		* \class	UITextArea
		* \ingroup 2DLayers
		* \brief	Text input UI item.
		*/
		// ****************************************

		class UITextArea : public UIDrawableItem
		{
		public:
			DECLARE_CLASS_INFO(UITextArea, UIDrawableItem, 2DLayers);

			/**
			 * \brief	constructor
			 * \param	name : instance name
			 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
			 */
			UITextArea(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
			virtual		~UITextArea();

			usString		GetText() const { return mText; }
			std::string	GetFontName() const { return mFont; }
			std::string	GetReleaseAction() const { return mReleaseAction; }
			int				GetFontSize() const { return mFontSize; }
			int				GetLength() const { return mLength; }
			void			SetText(const unsigned short* value) { this->ChangeText(value); mIsDefaultText = false; }
			bool			GetIsDefaultText() const { return mIsDefaultText; }
			unsigned int	GetCol() const { return mCol; }
			unsigned int	GetRow() const { return mRow; }


			void			CreateFirstText();

			//void			UpdateText(std::vector<int> keycodeList);

			//bool			TriggerMouseClick(int buttonState, int buttonEvent, int X, int Y, bool & catchClick)override;
			bool			HasFocus()override { return GetNodeFlag(UIItem_HasFocus); }
			void			LoseFocus()override;
			void			GetFocus()override;

			virtual void	ReloadTexture();

			DECLARE_METHOD(UpdateKeyBoard);
			COREMODIFIABLE_METHODS(UpdateKeyBoard);

		protected:
			void		InitModifiable() override;
			void		NotifyUpdate(const unsigned int /* labelid */)override;

			bool		isAlpha(float X, float Y)override;
			void		ProtectedDraw(Scene::TravState* state)override;

			void		ChangeText(const unsigned short* newText = NULL);
			void		ChangeText(const std::string& newText)
			{
				usString toChange(newText);
				ChangeText(toChange.us_str());
			}

			bool					mIsDown;
			bool					mStayPressed;
			bool					mHasDefaultText = true;
			bool					mIsDefaultText;
			u32						mFontSize = 12;
			u32						mLength = 0;
			u32						mRow = 1;
			u32						mCol = 1;
			u32						mTextAlignment = 1;

			usString				mText = (std::string)"DefaultText";
			std::string				mFont = "";
			std::string				mReleaseAction = "";

			WRAP_ATTRIBUTES(mHasDefaultText, mFontSize, mLength, mRow, mCol, mTextAlignment,mText, mFont, mReleaseAction);

			SmartPointer<Draw::Texture>	mTexturePointer;
			UIVerticesInfo* mTextureQI; // draw BG quad
		};
	}
}