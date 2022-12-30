#pragma once

#include "UIItem.h"
#include "UIDrawableItem.h"
#include "AttributePacking.h"

namespace Kigs
{
	namespace Draw
	{
		class RenderingScreen;
	}
	
	namespace Draw2D
	{

		// ****************************************
		// * UIRatioKeeper class
		// * --------------------------------------
		/**
		* \file	UIRatioKeeper.h
		* \class	UIRatioKeeper
		* \ingroup 2DLayers
		* \brief	Create a zone in screen where design size ratio is kept.
		*
		* UIRatioKeeper instance must be added to root uiitem.
		*/
		// ****************************************

		class UIRatioKeeper : public UIDrawableItem
		{
		public:
			DECLARE_CLASS_INFO(UIRatioKeeper, UIDrawableItem, 2DLayers);
			DECLARE_CONSTRUCTOR(UIRatioKeeper);

			void ForceRecomputeRatio()
			{
				mRatioIsOK = false;
			}

		protected:

			void NotifyUpdate(const unsigned int /* labelid */) override;
			void Update(const Time::Timer& a_Timer, void* addParam) override;

			void RecomputeRatio();

			bool mRatioIsOK;

			WRAP_METHODS(ForceRecomputeRatio);

			Draw::RenderingScreen* mCurrentScreen = nullptr;
		};

	}
}