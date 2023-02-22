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

			UICustomDrawDelegate*				mDelegate = nullptr;

			bool								mStayPressed = false;
			std::string							mClickDownAction = "";
			std::string							mClickUpAction = "";
			usString							mParameter = (std::string)"";

			WRAP_ATTRIBUTES(mStayPressed, mClickDownAction, mClickUpAction, mParameter);

			bool								mIsDown = false;
			bool								mIsMouseOver = false;
			bool								mInside = false;
		};

	}
}