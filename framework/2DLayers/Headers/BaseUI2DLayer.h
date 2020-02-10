#ifndef BASEUI2DLAYER_H_
#define BASEUI2DLAYER_H_

#include "Abstract2DLayer.h"

#include "GlobalEnum.h"
#include "maReference.h"
#include <deque>

class UIItem;
class ModuleInput;

class Node2D;

struct NodeToDraw
{
	Node2D* node = nullptr;
	int prio = 0;
	CoreModifiable* parent = nullptr;
	s32 depth = 0;
	u32 clip_count = 0;
	CoreModifiable* root = nullptr;

	struct Sorter
	{
		bool operator()(NodeToDraw& a, NodeToDraw& b) const;
	};
};


// ****************************************
// * BaseUI2DLayer class
// * --------------------------------------
/**
* \file	Base2DLayer.h
* \class	BaseUI2DLayer
* \ingroup 2DLayers
* \brief	base class for 2DLayerUI (2D Renderering)
*
* BaseUI2DLayer inherits Abstract2DLayer, it should be added to a scenegraph
*
* <dl class="dependency"><dt><b>Dependency:</b></dt><dd>Module2DLayer</dd></dl>
*
*/
// ****************************************
class BaseUI2DLayer : public Abstract2DLayer
{
public:
	DECLARE_CLASS_INFO(BaseUI2DLayer,Abstract2DLayer,2DLayers);
	DECLARE_CONSTRUCTOR(BaseUI2DLayer);
	
	void Update(const Timer& a_timer, void* addParam) override;

	//! Do drawing here if any
	void TravDraw(TravState* state) override;

	SP<UIItem>& GetRootItem() { return myRootItem; }

protected:
	
	void InitModifiable() override;

	void UpdateChildrens(const Timer& a_timer, UIItem* current, void* addParam);

	void SortItemsFrontToBack(SortItemsFrontToBackParam& param) override;

	ModuleInput* myInput;
	// TODO : change root item management in BaseUI2DLayer (use classic additem / removeitem...)
	SP<UIItem> myRootItem;

};
#endif //BASEUI2DLAYER_H_
