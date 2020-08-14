#ifndef _UINode3DLayer_H_
#define _UINode3DLayer_H_

#include "Node3D.h"
#include "UI/UIItem.h"

#include "AttributePacking.h"

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

	void Update(const Timer& a_timer, void* addParam) override;

	//! Do drawing here if any
	void TravDraw(TravState* state) override;

	SP<UIItem>& GetRootItem() { return mRootItem; }

protected:

	virtual void	NotifyUpdate(const unsigned int labelid) override;

	void RecomputeBoundingBox() override;

	void InitModifiable() override;

	void SortItemsFrontToBack(SortItemsFrontToBackParam& param);
	bool GetDataInTouchSupport(const touchPosInfos& posin, touchPosInfos& pout);
	void GetDistanceForInputSort(GetDistanceForInputSortParam& params);

	bool IsUsedInRenderPass(u32 pass_mask) const { return (u32(mRenderPassMask) & pass_mask) != 0; }

	SP<UIItem> mRootItem;
	// if not interactive, don't do update (but still draw)
	maBool	mIsInteractive = BASE_ATTRIBUTE(IsInteractive, true);

	maVect2DF	mSize = BASE_ATTRIBUTE(Size, 0.8f,0.6f);
	maVect2DF	mDesignSize = BASE_ATTRIBUTE(DesignSize, 800.0f, 600.0f);

	// Camera used for touch inputs
	maReference mCamera = BASE_ATTRIBUTE(Camera, "Camera:camera");
	maUInt mRenderPassMask = BASE_ATTRIBUTE(RenderPassMask, 0xffffffff);
	maInt mInputSortingLayer = BASE_ATTRIBUTE(InputSortingLayer, 0);

	CMSP		mCollider;
};

#endif //_UINode3DLayer_H_