#pragma once

#include "UITexturedItem.h"
#include "AttributePacking.h"
#include "TouchInputEventManager.h"

namespace Kigs
{

	namespace Draw2D
	{
		// ****************************************
		// * UIRenderingScreen class
		// * --------------------------------------
		/**
		* \file	UIRenderingScreen.h
		* \class	UIRenderingScreen
		* \ingroup 2DLayers
		* \brief	A Textured UI where the texture display a Rendering Screen
		*/
		// ****************************************

		class UIRenderingScreen : public UITexturedItem
		{
		public:
			DECLARE_CLASS_INFO(UIRenderingScreen, UITexturedItem, 2DLayers);
			DECLARE_CONSTRUCTOR(UIRenderingScreen);

			virtual bool GetDataInTouchSupport(const Input::touchPosInfos& posin, Input::touchPosInfos& pout);
			WRAP_METHODS(GetDataInTouchSupport);

		protected:
			void								InitModifiable() override;
			void								NotifyUpdate(unsigned int labelid) override;
			void								SetTexUV(UIVerticesInfo* aQI) override;

			bool								mForceNearest = false;
			std::weak_ptr<CoreModifiable>		mRenderingScreen;
	
			WRAP_ATTRIBUTES(mForceNearest, mRenderingScreen);
		};

	}
}