#pragma once

#include "UIText.h"

namespace Kigs
{

	namespace Draw2D
	{
		// ****************************************
		// * UIScrollingText class
		// * --------------------------------------
		/**
		* \file	UIScrollingText.h
		* \class	UIScrollingText
		* \ingroup 2DLayers
		* \brief	?? Obsolete ??
		*/
		// ****************************************

		class UIScrollingText : public UIText
		{
		public:
			DECLARE_CLASS_INFO(UIScrollingText, UIText, 2DLayers);

			/**
			 * \brief	constructor
			 * \param	name : instance name
			 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
			 */
			UIScrollingText(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);


		protected:
			void NotifyUpdate(const unsigned int labelid) override;
			void			Update(const Timer& a_timer, void* addParam) override;

			float					mCharacterPerSecond = 0.0f;
			unsigned int			mScrollIndex;
			double					mElapsedTime;
			WRAP_ATTRIBUTES(mCharacterPerSecond)
		};

	}
}