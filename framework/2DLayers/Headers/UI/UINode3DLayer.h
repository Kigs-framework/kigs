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
	WRAP_METHODS(SortItemsFrontToBack, GetDataInTouchSupport);

	void Update(const Timer& a_timer, void* addParam) override;

	//! Do drawing here if any
	void TravDraw(TravState* state) override;

	SP<UIItem>& GetRootItem() { return myRootItem; }

protected:

	virtual void	NotifyUpdate(const unsigned int labelid) override;

	void RecomputeBoundingBox() override;

	void InitModifiable() override;

	void SortItemsFrontToBack(SortItemsFrontToBackParam& param);
	bool GetDataInTouchSupport(const touchPosInfos& posin, touchPosInfos& pout);

	SP<UIItem> myRootItem;
	// if not interactive, don't do update (but still draw)
	maBool	myIsInteractive = BASE_ATTRIBUTE(IsInteractive, true);

	maVect2DF	mySize = BASE_ATTRIBUTE(Size, 0.8f,0.6f);
	maVect2DF	myDesignSize = BASE_ATTRIBUTE(DesignSize, 800.0f, 600.0f);

	// Camera used for touch inputs
	maReference myCamera = BASE_ATTRIBUTE(Camera, "Camera:camera");

	CMSP		my3DAndDesignSizeUniform;
	CMSP		myCollider;
};

#endif //_UINode3DLayer_H_