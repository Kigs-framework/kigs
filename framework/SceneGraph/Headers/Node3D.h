#pragma once
#include "CoreModifiable.h"
#include "TecLibs/Tec3D.h"
#include "TecLibs/3D/3DObject/BBox.h"
#include "SceneNode.h"
#include "CoreDecorator.h"
#include "RendererMatrix.h"


class TravState;
class Drawable;


class Node3D : public SceneNode
{
public:
	DECLARE_CLASS_INFO(Node3D, SceneNode, SceneGraph)
	DECLARE_CONSTRUCTOR(Node3D)
	
	
	// return TRUE if this node is visible or we are in a "all visible" branch of the scenegraph
	virtual bool Draw(TravState* state);

	/**
	* set node up to date, and compute visibility. Return FALSE when node is totally outside of the culling volume.
	* TRUE if this node is visible or we are in a "all visible" branch of the scenegraph
	*/
	DECLARE_DECORABLE_2_PARAMS(bool, Cull, Node3D, TravState*, unsigned int);
	DECLARE_DECORABLE_DEFINITION(bool, Cull, TravState* state, unsigned int CullingMask);

	virtual void TravDraw(TravState* state);
	virtual void TravCull(TravState* state);

	const BBox& GetLocalBoundingBox() {
		if (HasFlag(BoundingBoxIsDirty))
			RecomputeBoundingBox();
		return myLocalBBox;
	}
	void GetLocalBoundingBox(Point3D& pmin, Point3D& pmax) 
	{ 
		if (HasFlag(BoundingBoxIsDirty)) 
			RecomputeBoundingBox();
		pmin = myLocalBBox.m_Min; pmax = myLocalBBox.m_Max; 
	}

	const BBox& GetBoundingBox() {
		if (HasFlag(BoundingBoxIsDirty))
			RecomputeBoundingBox();
		return myBBox;
	}
	void GetBoundingBox(Point3D& pmin, Point3D& pmax)
	{
		if (HasFlag(BoundingBoxIsDirty)) 
			RecomputeBoundingBox();
		pmin = myBBox.m_Min; pmax = myBBox.m_Max;
	}

	const BBox& GetGlobalBoundingBox() { if (HasFlag(GlobalBoundingBoxIsDirty))
										RecomputeGlobalBoundingBox();
								return myGlobalBBox; }
	void	GetGlobalBoundingBox(Point3D& pmin, Point3D& pmax) 
	{ 
		if (HasFlag(GlobalBoundingBoxIsDirty))
			RecomputeGlobalBoundingBox();
		pmin = myGlobalBBox.m_Min; pmax = myGlobalBBox.m_Max;
	}

	
	bool	addItem(const CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;
	bool	removeItem(const CMSP& item DECLARE_DEFAULT_LINK_NAME) override;


	void SetAllSonsVisible(bool t)
	{
		if (t) SceneNode::SetAllSonsVisible();
		else SceneNode::UnsetAllSonsVisible();
	}

	/**
	* \brief	set myIsVisible member, myIsVisible store current frame index so that it can be used to test if culling was already done
	* \fn 		void SetVisible(int f)
	* \param	f : set myIsVisible to t
	*/
	inline void SetVisibleFrame(unsigned int f) { myIsVisible = f; }

	/**
	* \brief	check if the last seen frame is the current one
	* \fn 		void GetVisible(unsigned int f)
	* \param	currentFrame : index of the current frame
	*/
	inline bool IsVisibleInFrame(unsigned int currentFrame) { return myIsVisible != currentFrame; }


	Node3D* getFather() { return myFatherNode; }

	const Matrix3x4& GetLocalToGlobal() { if (HasFlag(LocalToGlobalMatrixIsDirty)) RecomputeLocalToGlobal(); return myLocalToGlobal; }
	const Matrix3x4& GetGlobalToLocal() { if (HasFlag(GlobalToLocalMatrixIsDirty)) RecomputeGlobalToLocal(); return myGlobalToLocal; }
	const Matrix3x4& GetLocal() { return myTransform; }

	virtual void ChangeMatrix(const Matrix3x4& newmatrix);

	void ChangeMatrixGlobal(const Matrix3x4& new_global_matrix);

	void ApplyLocalTransform(const Matrix3x4& transform);
	void ApplyGlobalTransform(const Matrix3x4& transform);

	// add rendering matrix before export if needed
	void PrepareExport(ExportSettings* settings) override;
	void EndExport(ExportSettings* settings) override;

	// Useless legacy stuff
	void ComputeGlobalBBox() {}
	void SetUpNode() {}
	void SetupNodeIfNeeded() {}
	void NeedBoundingBoxUpdate(SceneNode* node = 0, bool force = false) {}
	void NeedLocalToGlobalMatrixUpdate(bool force = false) {}
	void NeedSceneGraphUpdate(SceneNode* node = 0) {}
	void ComputeLocalToGlobal() {}
	void RecursiveUpdateAndBoundingVolume(kdouble time) {}
	//

	bool IsScaled() const { return HasFlag(IsScaledFlag); }
	void PropagateDirtyFlags(SceneNode* source);

	// use positive numbers 0,10,20,50... for priority, the higher the priority is, the earliest the node should be draw in travdraw
	void	setPriority(int priority)
	{
		myDrawPriority = priority;
		PropagateNodePriorityDirtyToParents();
	}

	int getDrawPriority()
	{
		return myDrawPriority;
	}

protected:

	void removeUser(CoreModifiable* user) override;
	void addUser(CoreModifiable* user) override;
	void InitModifiable() override;
	
	Node3D * myFatherNode;

	//! valid after scene TravCull : myIsVisible store current frame index if object is visible
	unsigned int myIsVisible;


	//! call predraw for drawable sons of this node return true if at least one drawable son was found
	void	PreDrawDrawable(TravState* state);
	//! call draw for drawable sons of this node
	void	DrawDrawable(TravState* state);
	//! call postdraw for drawable sons of this node
	void	PostDrawDrawable(TravState* state);

	//!	first visible Node in ModuleSceneGraph::myVisiblesNodeList
	// values are update during TravCull and used during TravDraw
	unsigned short myFirstVisibleNodeIndex;
	//!	number of visible node
	unsigned short myVisibleNodeCount;
	unsigned int myCullingMask;

	//!	Local transformation matrix (father => this local coordinate system)
	Matrix3x4 myTransform;
	//!	Local to global transformation matrix (Local coordinate system => global "world" coordinate system)
	Matrix3x4 myLocalToGlobal;
	//!	Global to local transformation matrix (Global "world" coordinate system => local coordinate system)
	Matrix3x4 myGlobalToLocal;

	//! AABounding box in local coordinate system
	BBox myLocalBBox;
	//! AABounding box in father coordinate system
	BBox myBBox;
	//! AABounding box in global "world" coordinate system
	BBox myGlobalBBox;

	maBool mIgnoreBBox = BASE_ATTRIBUTE(IgnoreBBox, false);

	void RecomputeLocalToGlobal();
	void RecomputeGlobalToLocal();
	void RecomputeGlobalBoundingBox();

	virtual void RecomputeBoundingBox();
	
	virtual void PropagateDirtyFlagsToSons(SceneNode* source);
	virtual void PropagateDirtyFlagsToParents(SceneNode* source);

	// draw priority management (not strict, just hint)

	int			myDrawPriority;
	void		PropagateNodePriorityDirtyToParents();
	int			ComputeNodePriority();
};

mat3x4 GetLocalLookAtPoint(Node3D* node, v3f global_point, bool force_up = false, v3f up_axis = v3f(0, 1, 0));