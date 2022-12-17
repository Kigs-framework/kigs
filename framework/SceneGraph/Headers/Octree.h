#ifndef _OCTREE_H_
#define _OCTREE_H_

#include "Node3D.h"
#include "CullingObject.h"

class Octree;


// ****************************************
// * OctreeSubNode class
// * --------------------------------------
/**
 * \file	Octree.h
 * \class	OctreeSubNode
 * \ingroup SceneGraph
 * \brief   Node in an octree.
 *
 * Using a Node3D for this is probably overkilling.
 */
 // ****************************************

class OctreeSubNode : public Node3D
{
protected: 


	struct PrecomputedCullInfo
	{
		Vector3D	mNormal;
		Point3D		mOrigin;
		int			mPreTests[3]; // index
	};

public:
	DECLARE_CLASS_INFO(OctreeSubNode,Node3D,SceneGraph)
	DECLARE_CONSTRUCTOR(OctreeSubNode);


	virtual ~OctreeSubNode();

	/**
	 * \brief	add a node
	 * \fn 		OctreeSubNode*  AddNode(Octree* father,SceneNode* node,int currentlevel,int maxLevel,int subdivelevel);
	 * \param	node : node to add
	 * \param	currentlevel : current level
	 * \param	maxLevel : max level
	 * \param	subdivelevel : subdivide level
	 * \return	the OctreeSubNode
	 */
	SP<OctreeSubNode>  AddNode(SP<SceneNode> node,int currentlevel,int maxLevel,int subdivelevel);

	/**
	 * \brief	remove a node
	 * \fn 		bool  RemoveNode(SceneNode* node);
	 * \param	node : node to add
	 * \return	TRUE if the node has been removed
	 */
	bool  RemoveNode(SceneNode* node);

	//bool  Cull(TravState* state,unsigned int cullingMask);
	DECLARE_DECORABLE_DEFINITION(bool, Cull, TravState* state, unsigned int CullingMask);


	/**
	 * \brief	divide
	 * \fn 		void  Divide();
	 */
	void  Divide();

	/**
	 * \brief	find a sub node
	 * \fn 		int   FindSubNode(SceneNode* node);
	 * \param	node : start from this node
	 * \return	the index of the found node
	 */
	int   FindSubNode(SceneNode* node);

	/**
	 * \brief	find a sub node from a bounding box
	 * \fn 		 int   FindSubNode(BBox* bbox);
	 * \param	bbox : start from this bounding box
	 * \return	the index of the found node
	 */
	int   FindSubNode(BBox* bbox);

	/**
	 * \brief	get the father
	 * \fn 		OctreeSubNode*  GetFather()
	 * \return	the father
	 */
	OctreeSubNode*  GetFather(){return mFatherSubNode;}

	/**
	 * \brief	check if sons are empty
	 * \fn 		bool  SonsAreEmpty();
	 * \return	TRUE if sons are empty
	 */
	bool  SonsAreEmpty();

	/**
	 * \brief	kill sons
	 * \fn 		void  KillSons();
	 */
	void  KillSons();

	/**
	 * \brief	perform culling recursivelly on sub node
	 * \fn 		CullingObject::CULLING_RESULT  RecurseCullSubNodes(const std::vector<PrecomputedCullInfo>& precull,TravState* state,unsigned int& cullSubMask);
	 * \param	precull : precomputed culling
	 * \param	state : current TravState
	 * \return	the culling result
	 */
	CullingObject::CULLING_RESULT  RecurseCullSubNodes(const std::vector<PrecomputedCullInfo>& precull,TravState* state,unsigned int cullSubMask);

	/**
	 * \brief	perform TravCull recursivelly on sub node
	 * \fn 		void						   RecurseTravCullSubNodes(TravState* state);
	 * \param	state : current TravState
	 */
	//void						   RecurseTravCullSubNodes(TravState* state);

	/**
	 * \brief	get a sub node
	 * \fn 		OctreeSubNode*				GetSubNode(int index)
	 * \param	index : index of the root
	 * \return	a sub node
	 */
	SP<OctreeSubNode>				GetSubNode(int index){return mSonsSubNodes[index];}

	/**
	 * \brief	get object list
	 * \fn 		const std::vector<SceneNode*>&   GetObjectList()
	 * \return	the object list
	 */
	const std::vector<SceneNode*>&   GetObjectList(){return mObjectList;}

	/**
	 * \brief	check if the octree has sons
	 * \fn 		HasSons()
	 * \return	TRUS if the octree has sons
	 */
	bool HasSons(){return (mSonsSubNodes!=0);}

	friend class Octree;

protected:

	//! list of sons	
	SP<OctreeSubNode>*				mSonsSubNodes;
	//! list of father?
	OctreeSubNode*					mFatherSubNode;

	//! list of object 
	std::vector<SceneNode*>		mObjectList;
	//! numer of nodes
	int								mTotalNodes;
	//! result of the culling
	CullingObject::CULLING_RESULT   mCullingResult;

	Octree*							mFatherOctree;
};


// ****************************************
// * Octree class
// * --------------------------------------
/**
 * \file	Octree.h
 * \class	Octree
 * \ingroup SceneGraph
 * \brief   An octree subdivide it's bounding box in eight son to manage faster culling / sorting.
 *
 * Probably obsolete.
 */
 // ****************************************
class Octree : public Node3D
{
public:
	friend class OctreeSubNode;

	DECLARE_CLASS_INFO(Octree,Node3D,SceneGraph)
	DECLARE_CONSTRUCTOR(Octree);
	
	DECLARE_DECORABLE_DEFINITION(bool, Cull, TravState* state, unsigned int CullingMask);


	
	bool	removeItem(const CMSP& item DECLARE_DEFAULT_LINK_NAME) override;

	

	/**
	 * \brief	add a node to the map
	 * \fn 		 void  AddNodeToMap(SceneNode*,OctreeSubNode*);
	 * \param	SceneNode : node to add
	 * \param	OctreeSubNode : node map
	 */
	void  AddNodeToMap(SceneNode*,OctreeSubNode*);


	bool isQuadtree()
	{
		return mIsQuadtree;
	}

	int getSubdivisionCount()
	{
		if(isQuadtree())
		{
			return 4;
		}
		return 8;
	}

	unsigned int getAxisMask()
	{
		return mAxisMask;
	}

	void ReorganiseOctree();

protected:
	bool mIsInReorganise;
	bool mIsInRemove;
	bool mFirstReorganiseWasDone;

	//! list of culling object
	CullingObject::CULLING_RESULT  CullSubNodes(CullingObject* obj,TravState* state);

	
	void InitModifiable() override;


	//! root of the octree
	SP<OctreeSubNode> mRootSubNode;

	//! min point of the bounding box
	maVect3DF mBoundingBoxMin;
	//! max point of the bounding box
	maVect3DF mBoundingBoxMax;
	//! subdivide level
	maInt mSubdivideLevel;
	//! max recursive level
	maInt mMaxRecursiveLevel;
	//! if we only want a quadtree
	maEnum<4> mQuadTreeAxis;

	//! map of scene node
	kigs::unordered_map<SceneNode*,OctreeSubNode*>  mObjectNodeAssociation;

	bool mIsQuadtree;
	unsigned int mAxisMask;

	std::vector<OctreeSubNode::PrecomputedCullInfo> mPrecomputedCull;
protected:

	void RecomputeBoundingBox() override;
	void PropagateDirtyFlagsToParents(SceneNode* source) override;
};

#endif //_OCTREE_H_
