#pragma once
#include "UIItem.h"

#include "AttributePacking.h"
#include "TouchInputEventManager.h"

namespace Kigs
{
	namespace Draw2D
	{
		// ****************************************
		// * UICursor class
		// * --------------------------------------
		/**
		* \file	UICursor.h
		* \class	UICursor
		* \ingroup 2DLayers
		* \brief	???
		*
		*/
		// ****************************************

		class UICursor : public UIItem
		{
		public:
			DECLARE_CLASS_INFO(UICursor, UIItem, 2DLayers);
			DECLARE_CONSTRUCTOR(UICursor);

			bool ManageDirectTouchEvent(Input::DirectTouchEvent& direct_touch);
			WRAP_METHODS(ManageDirectTouchEvent);

		private:
			maReference mUIOwner = BASE_ATTRIBUTE(UIOwner, "");
		};


	}
}
