#pragma once
#include "Node3D.h"

// ****************************************
// * MultiMesh class
// * --------------------------------------
/**
* \file	MultiMesh.h
* \class	MultiMesh
* \ingroup Renderer
* \brief	
*
*/
// ****************************************
class MultiMesh : public Node3D
{
public:
	DECLARE_CLASS_INFO(MultiMesh, Node3D, Renderer);
	DECLARE_CONSTRUCTOR(MultiMesh);

	bool addItem(const CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;
	bool removeItem(const CMSP& item) override;

	void TravDraw(TravState* state) override;
	
protected:
	void RecomputeBoundingBox() override;
	void PrepareExport(ExportSettings* settings) override;
	void EndExport(ExportSettings* settings) override;

	std::vector<Node3D*> mSubNodes;
	SmartPointer<Node3D> mFullMeshNode;

	bool mNeedFullMeshRecompute = false;

	BBox mFullBBox;
};