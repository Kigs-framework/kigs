#pragma once

#include "Node3D.h"
#include "UI/UIItem.h"

#include "AttributePacking.h"

namespace Kigs
{
	namespace Draw2D
	{
		using namespace Scene;
		// ****************************************
		// * UINode3DLayer class
		// * --------------------------------------
		/**
		* \file	UINode3DLayer.h
		* \class	UINode3DLayer
		* \ingroup 2DLayers
		*/
		// ****************************************
		class UINode3DLayer : public Node3D
		{
		public:
			DECLARE_CLASS_INFO(UINode3DLayer, Node3D, 2DLayers);
			DECLARE_CONSTRUCTOR(UINode3DLayer);
			WRAP_METHODS(SortItemsFrontToBack, GetDataInTouchSupport, GetDistanceForInputSort);

			void Update(const Time::Timer& a_timer, void* addParam) override;

			//! Do drawing here if any
			void TravDraw(TravState* state) override;

			SP<UIItem> GetRootItem() { return mRootItem; }

		protected:

			virtual void NotifyUpdate(const unsigned int labelid) override;

			void RecomputeBoundingBox() override;

			void InitModifiable() override;

			void SortItemsFrontToBack(Input::SortItemsFrontToBackParam& param);
			bool GetDataInTouchSupport(const Input::touchPosInfos& posin, Input::touchPosInfos& pout);
			void GetDistanceForInputSort(Input::GetDistanceForInputSortParam& params);

			bool IsUsedInRenderPass(u32 pass_mask) const { return (u32(mRenderPassMask) & pass_mask) != 0; }

			SP<UIItem> mRootItem;
			// if not interactive, don't do update (but still draw)
			bool	mIsInteractive = true;
			u32		mRenderPassMask = 0xffffffff;
			s32		mInputSortingLayer = 0;

			v2f		mSize = { 0.8f, 0.6f };
			v2f		mDesignSize = { 800.0f, 600.0f };

			// Camera used for touch inputs
			std::weak_ptr<CoreModifiable> mCamera;

			WRAP_ATTRIBUTES(mIsInteractive, mRenderPassMask, mInputSortingLayer, mSize, mDesignSize, mCamera);

			CMSP		mCollider;
		};

	}
}