#pragma once

#include "UIButton.h"
#include "maUSString.h"

namespace Kigs
{
	namespace Draw2D
	{
		// ****************************************
		// * UIButtonImage class
		// * --------------------------------------
		/**
		* \file	UIButtonImage.h
		* \class	UIButtonImage
		* \ingroup 2DLayers
		* \brief	Button with three textures ( Up, Down , Over )
		*
		*/
		// ****************************************

		class UIButtonImage : public UIButton
		{
		public:
			DECLARE_CLASS_INFO(UIButtonImage, UIButton, 2DLayers);

			/**
			 * \brief	constructor
			 * \param	name : instance name
			 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
			 */
			UIButtonImage(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
			virtual ~UIButtonImage();

			void ChangeTexture(std::string texturename, std::string overtexturename = "", std::string downtexturename = "");

		protected:

			void InitModifiable()override;
			void NotifyUpdate(const unsigned int /* labelid */)override;

			void	ChangeState()override;

			void	ReloadTexture();

			SmartPointer<Draw::TextureHandler>				mUpTexturePointer;
			SmartPointer<Draw::TextureHandler>				mDownTexturePointer;
			SmartPointer<Draw::TextureHandler>				mOverTexturePointer;

			std::string										mUpTexture="";
			std::string										mDownTexture="";
			std::string										mOverTexture="";

			WRAP_ATTRIBUTES(mUpTexture, mDownTexture, mOverTexture);
		};

	}
}