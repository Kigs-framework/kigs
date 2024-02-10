#pragma once

#include "UIButton.h"
#include "maUSString.h"

namespace Kigs
{
	namespace Draw2D
	{
		// ****************************************
		// * UIButtonText class
		// * --------------------------------------
		/**
		* \file	UIButtonText.h
		* \class	UIButtonText
		* \ingroup 2DLayers
		* \brief	Three state button with different texts.
		*
		* Probably obsolete. Perhaps too keep has an upgrador
		*/
		// ****************************************

		class UIButtonText : public UIButton
		{
		public:
			DECLARE_CLASS_INFO(UIButtonText, UIButton, 2DLayers);

			/**
			 * \brief	constructor
			 * \param	name : instance name
			 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
			 */
			UIButtonText(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			void				ChangeTexture(std::string& texturename, std::string& overtexturename, std::string& downtexturename);
			void				ChangeTextureColor(v4f& UpColor, v4f& OverColor, v4f& DownColor);
			void				GetUpColor(float& R, float& G, float& B);
			void				GetOverColor(float& R, float& G, float& B);
			void				GetDownColor(float& R, float& G, float& B);

		protected:

			void InitModifiable()override;
			void NotifyUpdate(const unsigned int labelid)override;
			void ChangeState()override;

			void				ChangeTextTexture(const std::string& text, unsigned int texture);
			void				ChangeTextTexture(const unsigned short* text, unsigned int texture);
			unsigned short* CutText(unsigned short* text, bool& flag);
			char* CutText(const char* text, bool& flag);

			DECLARE_METHOD(ReloadTexture);
			COREMODIFIABLE_METHODS(ReloadTexture);

			SmartPointer<Draw::Texture>			mUpTexturePointer;
			SmartPointer<Draw::Texture>			mDownTexturePointer;
			SmartPointer<Draw::Texture>			mOverTexturePointer;

			u32					mFontSize = 12;
			u32					mLength = 0;
			u32					mTextAlignment = 1;

			v4f					mUpColor = { 0.0f, 0.0f, 0.0f, 0.0f };
			v4f					mOverColor = { 0.0f, 0.0f, 0.0f, 0.0f };
			v4f					mDownColor = { 0.0f, 0.0f, 0.0f, 0.0f };

			std::string			mFont = "arial.ttf";
			std::string			mUpText = "";
			std::string			mDownText = "";
			std::string			mOverText = "";
		
			WRAP_ATTRIBUTES(mFontSize, mLength, mTextAlignment, mUpColor, mOverColor,mDownColor,mFont,mUpText,mDownText,mOverText);
		};

	}
}