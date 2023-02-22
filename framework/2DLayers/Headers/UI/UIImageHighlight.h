#pragma once

#include "UIImage.h"
#include "TextureFileManager.h"
#include "AttributePacking.h"

namespace Kigs
{
	namespace Draw2D
	{
		// ****************************************
		// * UIImageHighlight class
		// * --------------------------------------
		/**
		* \file	UIImageHighlight.h
		* \class	UIImageHighlight
		* \ingroup 2DLayers
		* \brief	???
		*/
		// ****************************************

		class UIImageHighlight : public UIImage
		{
		public:
			DECLARE_CLASS_INFO(UIImageHighlight, UIImage, 2DLayers);
			DECLARE_INLINE_CONSTRUCTOR(UIImageHighlight) {}

			void ChangeTexture() override;
			void Highlight(bool b);

			void InitModifiable() override;
			virtual void NotifyUpdate(const unsigned int /* labelid */)override;

		protected:
			WRAP_METHODS(Highlight);

			SmartPointer<Draw::TextureHandler>				mPLTexturePointer;
			SmartPointer<Draw::TextureHandler>				mHLTexturePointer;


			std::string	mHLTexture = "";
			WRAP_ATTRIBUTES(mHLTexture);
		};

	}
}