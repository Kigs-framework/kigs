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
* \brief	UI rendering support
*
* BaseUI2DLayer inherits Abstract2DLayer. It have to be added to SceneGraph and is sorted by Priority with other 2DLayers and Scene3D.
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

	SP<UIItem> GetRootItem() { return mRootItem; }

	static void AccumulateToDraw(TravState* state, kstl::vector<NodeToDraw>& todraw, CoreModifiable* current, int depth = 0, u32 clip_count = 0);
	static void UpdateChildrens(const Timer& a_timer, UIItem* current, void* addParam);

protected:
	
	void InitModifiable() override;

	void SortItemsFrontToBack(SortItemsFrontToBackParam& param) override;

	ModuleInput* mInput;
	// TODO : change root item management in BaseUI2DLayer (use classic additem / removeitem...)
	SP<UIItem> mRootItem;


};
#endif //BASEUI2DLAYER_H_
