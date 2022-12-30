#pragma once

#include "UIItem.h"
#include "maUSString.h"

namespace Kigs
{
	namespace Draw2D
	{
		class UICustomDrawDelegate
		{
		public:
			virtual void Drawfunc(Scene::TravState*, UIItem*) = 0;
			virtual bool IsAlphafunc(float X, float Y) { return false; }
		};

		// ****************************************
		// * UICustomDraw class
		// * --------------------------------------
		/**
		* \file	UICustomDraw.h
		* \class	UICustomDraw
		* \ingroup 2DLayers
		* \brief	Draw non UI objects.
		*
		*/
		// ****************************************

		class UICustomDraw : public UIItem
		{
		public:
			DECLARE_CLASS_INFO(UICustomDraw, UIItem, 2DLayers);
			DECLARE_INLINE_CONSTRUCTOR(UICustomDraw) {}

			inline UICustomDrawDelegate* GetDelegate() { return mDelegate; }
			inline void							SetDelegate(UICustomDrawDelegate* a_value) { mDelegate = a_value; }

		protected:
			bool								isAlpha(float X, float Y) override;

			void								ProtectedDraw(Scene::TravState* state) override;

			UICustomDrawDelegate* mDelegate = nullptr;
			maString							mClickDownAction = BASE_ATTRIBUTE(ClickDownAction, "");
			maString							mClickUpAction = BASE_ATTRIBUTE(ClickUpAction, "");
			maUSString							mParameter = BASE_ATTRIBUTE(Parameter, (std::string)"");
			maBool								mStayPressed = BASE_ATTRIBUTE(StayPressed, false);
			bool								mIsDown = false;
			bool								mIsMouseOver = false;
			bool								mInside = false;
		};

	}
}