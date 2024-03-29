#pragma once

#include "UIScrollingText.h"

namespace Kigs
{

	namespace Draw2D
	{
		// ****************************************
		// * UIScrollingMultilineText class
		// * --------------------------------------
		/**
		* \file	UIScrollingMultilineText.h
		* \class	UIScrollingMultilineText
		* \ingroup 2DLayers
		* \brief	?? Obsolete ??
		*/
		// ****************************************

		class UIScrollingMultilineText : public UIScrollingText
		{
		public:
			DECLARE_CLASS_INFO(UIScrollingMultilineText, UIScrollingText, 2DLayers);

			/**
			 * \brief	constructor
			 * \param	name : instance name
			 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
			 */
			UIScrollingMultilineText(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);


		protected:


		};

	}
}