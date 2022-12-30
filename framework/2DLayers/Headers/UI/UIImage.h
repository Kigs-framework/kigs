#pragma once

#include "UITexturedItem.h"
#include "TextureFileManager.h"

namespace Kigs
{
	namespace Draw2D
	{
		// ****************************************
		// * UIImage class
		// * --------------------------------------
		/**
		* \file	UIImage.h
		* \class	UIImage
		* \ingroup 2DLayers
		* \brief	Just display an image ( texture ) with different modes.
		*/
		// ****************************************

		class UIImage : public UITexturedItem
		{
		public:

			DECLARE_CLASS_INFO(UIImage, UITexturedItem, 2DLayers);

			/**
			 * \brief	constructor
			 * \param	name : instance name
			 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
			 */
			UIImage(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			virtual void ChangeTexture();

		protected:



			void InitModifiable() override;
			//virtual void NotifyUpdate(const unsigned int /* labelid */)override;
			bool isAlpha(float X, float Y) override;

		};

	}
}