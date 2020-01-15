#ifndef _OCTREE_H_
#define _OCTREE_H_

#include "Node3D.h"
#include "CullingObject.h"

class Octree;
class OctreeSubNode : public Node3D
{
protected: 


	struct PrecomputedCullInfo
	{
		Vector3D	myNormal;
		Point3D		myOrigin;
		int			myPreTests[3]; // index
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
	SP<OctreeSubNode>  AddNode(SP<SceneNode>& node,int currentlevel,int maxLevel,int subdivelevel);

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
	OctreeSubNode*  GetFather(){return myFatherSubNode;}

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
	 * \fn 		CullingObject::CULLING_RESULT  RecurseCullSubNodes(const kstl::vector<PrecomputedCullInfo>& precull,TravState* state,unsigned int& cullSubMask);
	 * \param	precull : precomputed culling
	 * \param	state : current TravState
	 * \return	the culling result
	 */
	CullingObject::CULLING_RESULT  RecurseCullSubNodes(const kstl::vector<PrecomputedCullInfo>& precull,TravState* state,unsigned int cullSubMask);

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
	SP<OctreeSubNode>&				GetSubNode(int index){return mySonsSubNodes[index];}

	/**
	 * \brief	get object list
	 * \fn 		const kstl::vector<SceneNode*>&   GetObjectList()
	 * \return	the object list
	 */
	const kstl::vector<SceneNode*>&   GetObjectList(){return myObjectList;}

	/**
	 * \brief	check if the octree has sons
	 * \fn 		HasSons()
	 * \return	TRUS if the octree has sons
	 */
	bool HasSons(){return (mySonsSubNodes!=0);}

	friend class Octree;

protected:

	//! list of sons	
	SP<OctreeSubNode>*				mySonsSubNodes;
	//! list of father?
	OctreeSubNode*					myFatherSubNode;

	//! list of object 
	kstl::vector<SceneNode*>		myObjectList;
	//! numer of nodes
	int								myTotalNodes;
	//! result of the culling
	CullingObject::CULLING_RESULT   myCullingResult;

	Octree*							myFatherOctree;
};

// ****************************************
// * Octree class
// * --------------------------------------
/**
 * \file	Octree.h
 * \class	Octree
 * \ingroup SceneGraph
 * \brief	An Octree is a special Node3D class used to subdivide space in OctreeSubNode allowing fast culling / drawing process
 * \author	ukn
 * \version ukn
 * \date	ukn
 * when culling or drawing scenegraph
 * base members are the global LOD, and the global time
 *
 * Exported parameters :<br>
 * <ul>
 * <li>
 *		kfloat <strong>BoundingBoxMin[3]</strong> : min point of the bounding box
 * </li>
 * <li>
 *		kfloat <strong>BoundingBoxMax[3]</strong> : max point of the bounding box
 * </li>
 * <li>
 *		kfloat <strong>SubdivideLevel</strong> : subdivide level
 * </li>
 * <li>
 *		kfloat <strong>MaxRecursiveLevel</strong> : max recursive level
 * </li>
 * </ul>
 */
// ****************************************
class Octree : public Node3D
{
public:
	friend class OctreeSubNode;

	DECLARE_CLASS_INFO(Octree,Node3D,SceneGraph)
	DECLARE_CONSTRUCTOR(Octree);
	
	DECLARE_DECORABLE_DEFINITION(bool, Cull, TravState* state, unsigned int CullingMask);


	
	bool	removeItem(CMSP& item DECLARE_DEFAULT_LINK_NAME) override;

	

	/**
	 * \brief	add a node to the map
	 * \fn 		 void  AddNodeToMap(SceneNode*,OctreeSubNode*);
	 * \param	SceneNode : node to add
	 * \param	OctreeSubNode : node map
	 */
	void  AddNodeToMap(SceneNode*,OctreeSubNode*);


	bool isQuadtree()
	{
		return myIsQuadtree;
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
		return myAxisMask;
	}

	void ReorganiseOctree();

protected:
	bool myIsInReorganise;
	bool myIsInRemove;
	bool myFirstReorganiseWasDone;

	//! list of culling object
	CullingObject::CULLING_RESULT  CullSubNodes(CullingObject* obj,TravState* state);

	
	void InitModifiable() override;


	//! root of the octree
	SP<OctreeSubNode> myRootSubNode;

	//! min point of the bounding box
	maVect3DF myBBoxMin;
	//! max point of the bounding box
	maVect3DF myBBoxMax;
	//! subdivide level
	maInt mySubdivideLevel;
	//! max recursive level
	maInt myMaxRecursiveLevel;
	//! if we only want a quadtree
	maEnum<4> myQuadTreeAxis;

	//! map of scene node
	kstl::unordered_map<SceneNode*,OctreeSubNode*>  myObjectNodeAssociation;

	bool myIsQuadtree;
	unsigned int myAxisMask;

	kstl::vector<OctreeSubNode::PrecomputedCullInfo> myPrecomputedCull;
protected:

	void RecomputeBoundingBox() override;
	void PropagateDirtyFlagsToParents(SceneNode* source) override;
};

#endif //_OCTREE_H_
