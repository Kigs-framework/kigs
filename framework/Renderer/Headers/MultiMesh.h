#pragma once
#include "Node3D.h"

class MultiMesh : public Node3D
{
public:
	DECLARE_CLASS_INFO(MultiMesh, Node3D, Renderer);
	DECLARE_CONSTRUCTOR(MultiMesh);

	bool addItem(CoreModifiable* item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;
	bool removeItem(CoreModifiable* item) override;

	void TravDraw(TravState* state) override;
	
protected:
	void RecomputeBoundingBox() override;
	void PrepareExport(ExportSettings* settings) override;
	void EndExport(ExportSettings* settings) override;

	kstl::vector<Node3D*> _subnodes;
	SmartPointer<Node3D> _full_mesh_node;

	bool _need_full_mesh_recompute = false;

	BBox _full_bbox;
};