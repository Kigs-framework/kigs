#pragma once

#include "UI/UIItem.h"
#include "SmartPointer.h"
#include "UIVerticesInfo.h"
#include "UIShapeDelegate.h"

namespace Kigs
{
	namespace Draw
	{
		class Texture;
	}
	namespace Draw2D
	{
		class UIDrawableItem;
		class UIShapeDelegate;


		// ****************************************
		// * UIDrawableItem class
		// * --------------------------------------
		/**
		* \file	UIDrawableItem.h
		* \class	UIDrawableItem
		* \ingroup 2DLayers
		* \brief	UIItem to be drawn.
		*
		*/
		// ****************************************

		class UIDrawableItem : public UIItem
		{
		public:
			DECLARE_CLASS_INFO(UIDrawableItem, UIItem, 2DLayers);
			DECLARE_CONSTRUCTOR(UIDrawableItem);

			static constexpr unsigned int UserFlagUseColorArray = 1 << ParentClassType::usedUserFlags;
			static constexpr unsigned int usedUserFlags = ParentClassType::usedUserFlags + 1;


			void ProtectedDraw(Scene::TravState* state) override;
			void SetVertexArray(UIVerticesInfo* aQI) override;
			void SetColor(UIVerticesInfo* aQI) override;
			UIVerticesInfo* GetVerticesInfo() override { return &mVI; }

			virtual void SetWhiteColor(UIVerticesInfo* aQI);

			// manage Shape directly added 
			bool	addItem(const CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;
			bool	removeItem(const CMSP& item DECLARE_DEFAULT_LINK_NAME) override;

		protected:

			virtual v2f getDrawablePos(const v2f& pos)
			{
				return pos;
			}

			UIVerticesInfo mVI;

			v2i		mSliced = { 0, 0 };

			WRAP_ATTRIBUTES(mSliced);

			// override SetVertexArray SetColor and SetTexUV to draw UI as a Shape
			SP<UIShapeDelegate> mShape;
		};


	}
}
