#ifndef _UINode3DLayer_H_
#define _UINode3DLayer_H_

#include "Node3D.h"
#include "UI/UIItem.h"

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

	void Update(const Timer& a_timer, void* addParam) override;

	//! Do drawing here if any
	void TravDraw(TravState* state) override;

	SP<UIItem>& GetRootItem() { return myRootItem; }

protected:

	virtual void	NotifyUpdate(const unsigned int labelid) override;

	void RecomputeBoundingBox() override;

	void InitModifiable() override;

	void SortItemsFrontToBack(SortItemsFrontToBackParam& param);

	SP<UIItem> myRootItem;
	// if not interactive, don't do update (but still draw)
	maBool	myIsInteractive = BASE_ATTRIBUTE(IsInteractive, true);

	maVect2DF	my3DSize = BASE_ATTRIBUTE(3DSize, 0.8f,0.6f);
	maVect2DF	myDesignSize = BASE_ATTRIBUTE(DesignSize, 800.0f, 600.0f);

	CMSP		my3DAndDesignSizeUniform;
};

#endif //_UINode3DLayer_H_