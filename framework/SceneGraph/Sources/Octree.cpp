#include "PrecompiledHeaders.h"

#include "Octree.h"
#include "TravState.h"
#include "ModuleSceneGraph.h"
#include "ModuleRenderer.h"
#include "Core.h"
#include "RendererMatrix.h"
#include "Drawable.h"
#include "CullingObject.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(Octree, Octree, SceneGraph);
IMPLEMENT_CLASS_INFO(Octree)

IMPLEMENT_CONSTRUCTOR(Octree)
, myIsInRemove(false)
, myRootSubNode(nullptr)
, myBBoxMin(*this, true, "BoundingBoxMin")
, myBBoxMax(*this, true, "BoundingBoxMax")
, mySubdivideLevel(*this, true, "SubdivideLevel", 10)
, myMaxRecursiveLevel(*this, true, "MaxRecursiveLevel", 5)
, myQuadTreeAxis(*this, true, "QuadTreeAxis", "None", "X", "Y", "Z")
, myIsQuadtree(false)

{
	myPrecomputedCull.clear();
	myRootSubNode = KigsCore::GetInstanceOf("myRootSubNode", "OctreeSubNode");
	addItem((CMSP&)myRootSubNode);
	
	myRootSubNode->myFatherOctree = this;
	myRootSubNode->Init();
	myIsInReorganise = false;
	myFirstReorganiseWasDone = false;

	OVERLOAD_DECORABLE(Cull, Node3D, Octree);
}

void	Octree::InitModifiable()
{
	Node3D::InitModifiable();

	myLocalBBox.m_Min.Set(myBBoxMin[0], myBBoxMin[1], myBBoxMin[2]);
	myLocalBBox.m_Max.Set(myBBoxMax[0], myBBoxMax[1], myBBoxMax[2]);
	myBBox = myLocalBBox;
	ComputeGlobalBBox();

	myRootSubNode->myLocalBBox = myLocalBBox;
	myRootSubNode->myBBox = myBBox;
	myRootSubNode->myGlobalBBox = myGlobalBBox;

	myIsQuadtree = (((int)myQuadTreeAxis) != 0);
	myAxisMask = 0;
	if (myIsQuadtree)
	{
		myAxisMask = 1 << (((int)myQuadTreeAxis) - 1);
	}
}

void Octree::PropagateDirtyFlagsToParents(SceneNode* source)
{
	ParentClassType::PropagateDirtyFlagsToParents(source);
	if (myIsInReorganise) return;
	if (!myIsInRemove)
	{
		auto found = myObjectNodeAssociation.find(source);
		if (found != myObjectNodeAssociation.end())
		{
			OctreeSubNode* subnode = (*found).second;
			myObjectNodeAssociation.erase(found);  // avoid reentrance here
			if (subnode)
			{
				CMSP toAdd(source, StealRefTag{});
				addItem(toAdd);
				subnode->RemoveNode(source);
			}
		}
	}
}

void Octree::RecomputeBoundingBox()
{
	if (!myFirstReorganiseWasDone)
	{
		return;
	}

	myLocalBBox.m_Min.Set(myBBoxMin[0], myBBoxMin[1], myBBoxMin[2]);
	myLocalBBox.m_Max.Set(myBBoxMax[0], myBBoxMax[1], myBBoxMax[2]);

	Matrix3x4		BBoxTransformMatrix(myTransform);

	BBoxTransformMatrix.e[0][0] = fabsf(BBoxTransformMatrix.e[0][0]);
	BBoxTransformMatrix.e[1][0] = fabsf(BBoxTransformMatrix.e[1][0]);
	BBoxTransformMatrix.e[2][0] = fabsf(BBoxTransformMatrix.e[2][0]);

	BBoxTransformMatrix.e[0][1] = fabsf(BBoxTransformMatrix.e[0][1]);
	BBoxTransformMatrix.e[1][1] = fabsf(BBoxTransformMatrix.e[1][1]);
	BBoxTransformMatrix.e[2][1] = fabsf(BBoxTransformMatrix.e[2][1]);

	BBoxTransformMatrix.e[0][2] = fabsf(BBoxTransformMatrix.e[0][2]);
	BBoxTransformMatrix.e[1][2] = fabsf(BBoxTransformMatrix.e[1][2]);
	BBoxTransformMatrix.e[2][2] = fabsf(BBoxTransformMatrix.e[2][2]);

	// compute local translation of the bounding box

	Vector3D translation(myLocalBBox.m_Min);
	translation += myLocalBBox.m_Max;
	translation *= 0.5f;

	myBBox.m_Max = myLocalBBox.m_Max;
	myBBox.m_Max -= translation;

	BBoxTransformMatrix.TransformVector((Vector3D*)&myBBox.m_Max);
	myTransform.TransformVector(&translation);

	translation += myTransform.GetTranslation();

	myBBox.m_Min = -myBBox.m_Max;
	myBBox.m_Max += translation;
	myBBox.m_Min += translation;


	myRootSubNode->myBBox = myLocalBBox;


	UnsetFlag(BoundingBoxIsDirty);
}

DECLARE_DECORABLE_IMPLEMENT(bool, Cull, Octree, TravState* state, unsigned int cullingMask)
{
	myCullingMask = cullingMask;
	if (!IsRenderable())
	{
		myIsVisible = 0;
		return false;
	}

	ReorganiseOctree(); // check if some reorganise is useful

	if (!state->IsAllVisible())
	{
		if (myIsVisible != state->GetVisibilityFrame())
		{
			UnsetAllSonsVisible();

			CullingObject::CULLING_RESULT result = CullSubNodes(state->GetCullingObject().get(), state);

			if (result == CullingObject::all_out)
			{
				myIsVisible = 0;
				return false;
			}

			if (result == CullingObject::all_in)
			{
				myIsVisible = state->GetVisibilityFrame();
				SceneNode::SetAllSonsVisible();
				return true;
			}
		}
	}

	myIsVisible = state->GetVisibilityFrame();
	return true;

}

CullingObject::CULLING_RESULT  Octree::CullSubNodes(CullingObject* cullobj, TravState* state)
{
	kstl::vector<OctreeSubNode::PrecomputedCullInfo>::iterator	itprecomp;

	if (myPrecomputedCull.size() == 0)
	{
		for (auto it = cullobj->GetCullPlaneList().begin(); it != cullobj->GetCullPlaneList().end(); ++it)
		{
			// create vector
			OctreeSubNode::PrecomputedCullInfo toAdd;
			myPrecomputedCull.push_back(toAdd);
		}
	}


	itprecomp = myPrecomputedCull.begin();
	// precompute
	for (auto it = cullobj->GetCullPlaneList().begin(); it != cullobj->GetCullPlaneList().end(); ++it)
	{
		OctreeSubNode::PrecomputedCullInfo& current = (*itprecomp);
		current.myOrigin = it->myOrigin;
		current.myNormal = it->myNormal;

		auto g2l = GetGlobalToLocal();
		g2l.TransformVector(&current.myNormal);
		g2l.TransformPoints(&current.myOrigin, 1);


		if (current.myNormal.x < 0.0f)
		{
			current.myPreTests[0] = 0;
		}
		else
		{
			current.myPreTests[0] = 3;
		}

		if (current.myNormal.y < 0.0f)
		{
			current.myPreTests[1] = 0;
		}
		else
		{
			current.myPreTests[1] = 3;
		}

		if (current.myNormal.z < 0.0f)
		{
			current.myPreTests[2] = 0;
		}
		else
		{
			current.myPreTests[2] = 3;
		}

		++itprecomp;
	}

	return myRootSubNode->RecurseCullSubNodes(myPrecomputedCull, state, myCullingMask);

	//myCullingMask=myRootSubNode->myCullingMask;
}


// manage octree remove
bool	Octree::removeItem(const CMSP& item DECLARE_LINK_NAME)
{
	bool foundsubnode = false;
	if (item->isSubType("SceneNode"))
	{
		if (!item->isSubType("OctreeSubNode"))
		{
			OctreeSubNode* subnode = myObjectNodeAssociation[(SceneNode*)item.get()];
			if (subnode)
			{
				foundsubnode = subnode->RemoveNode((SceneNode*)item.get());

				// check if we can move back all nodes3D to father subnode
				OctreeSubNode* father = subnode->GetFather();
				if (father)
				{
					if (father->SonsAreEmpty())
					{
						father->KillSons();
					}
				}
				myObjectNodeAssociation[(SceneNode*)item.get()] = 0;
			}
		}
	}
	if (!foundsubnode)
		return Node3D::removeItem(item PASS_LINK_NAME(linkName));
	else
	{
		return true;
	}
}

void  Octree::ReorganiseOctree()
{
	myIsInReorganise = true;

	if (!myFirstReorganiseWasDone)
	{
		myFirstReorganiseWasDone = true;
		SetFlag(BoundingBoxIsDirty);
		SetFlag(GlobalBoundingBoxIsDirty);
		PropagateDirtyFlags(this);
		RecomputeBoundingBox();
	}

	kstl::vector<CMSP> toRemove;
	// add all concerned object to the octree
	for (auto mis : getItems())
	{
		CMSP& item = mis.myItem;

		if (item->isUserFlagSet(UserFlagNode3D))
		{
			// make sure the bounding box, matrix... are ok before adding
			if (!item->isSubType("OctreeSubNode"))
			{
				SP<OctreeSubNode> subNode = myRootSubNode->AddNode((SP<SceneNode>&)item, 0, myMaxRecursiveLevel, mySubdivideLevel);

				AddNodeToMap((SceneNode*)item.get(), subNode.get());
				toRemove.push_back(item);
			}
		}
	}

	for (auto todel : toRemove)
	{
		Node3D::removeItem(todel);
	}
	myIsInReorganise = false;
}

void  Octree::AddNodeToMap(SceneNode* node, OctreeSubNode* subnode)
{
	if (subnode)
	{
		myObjectNodeAssociation[node] = subnode;
	}
}


//IMPLEMENT_AND_REGISTER_CLASS_INFO(OctreeSubNode, OctreeSubNode, SceneGraph);
IMPLEMENT_CLASS_INFO(OctreeSubNode)



OctreeSubNode::OctreeSubNode(const kstl::string& name, CLASS_NAME_TREE_ARG) :
	Node3D(name, PASS_CLASS_NAME_TREE_ARG)
	, mySonsSubNodes(0)
	, myFatherSubNode(0)
	, myTotalNodes(0)
	, myFatherOctree(0)
{
	setUserFlag(UserFlagCameraSort);
	myObjectList.clear();
	OVERLOAD_DECORABLE(Cull, Node3D, OctreeSubNode);
}

OctreeSubNode::~OctreeSubNode()
{
	if (mySonsSubNodes)
	{
		delete[] mySonsSubNodes;
	}
}

void  OctreeSubNode::KillSons()
{
	int soncount = myFatherOctree->getSubdivisionCount();
	int index;
	for (index = 0; index < soncount; index++)
	{
		removeItem((CMSP&)mySonsSubNodes[index]);
	}
	delete[] mySonsSubNodes;
	mySonsSubNodes = nullptr;
}

bool  OctreeSubNode::SonsAreEmpty()
{
	if (mySonsSubNodes)
	{
		int index;
		int count = 0;
		int soncount = myFatherOctree->getSubdivisionCount();

		for (index = 0; index < soncount; index++)
		{
			count += mySonsSubNodes[index]->myTotalNodes;
		}

		return (count == 0);

	}

	return true;
}

SP<OctreeSubNode>  OctreeSubNode::AddNode(SP<SceneNode>& node, int currentlevel, int maxLevel, int subdivelevel)
{
	// add it to me
	if ((myTotalNodes < subdivelevel) || (currentlevel >= maxLevel))
	{
		myObjectList.push_back(node.get());
		addItem((CMSP&)node);

		myTotalNodes++;
		return CMSP(this, GetRefTag{});
	}
	else // try to add it to sons
	{
		// check if we have to create sons
		if (!mySonsSubNodes)
		{
			Divide();
		}

		int index = FindSubNode(node.get());
		if (index != -1)
		{
			myTotalNodes++;
			return mySonsSubNodes[index]->AddNode(node, currentlevel + 1, maxLevel, subdivelevel);
		}
		else
		{
			myObjectList.push_back(node.get());
			addItem((CMSP&)node);
			myTotalNodes++;
			return CMSP(this, GetRefTag{});
		}
	}
}

void  OctreeSubNode::Divide()
{
	int soncount = myFatherOctree->getSubdivisionCount();

	mySonsSubNodes = new SP<OctreeSubNode>[soncount];

	// compute son bounding box
	int i, j, k, index;

	for (i = 0; i < soncount; i++)
	{
		mySonsSubNodes[i] = KigsCore::GetInstanceOf("mySonsSubNodes", "OctreeSubNode");
		addItem((CMSP&)mySonsSubNodes[i]);
		mySonsSubNodes[i]->myFatherOctree = myFatherOctree;
		mySonsSubNodes[i]->Init();
	}

	index = 0;

	Vector3D size_on_two = myBBox.m_Max;
	size_on_two -= myBBox.m_Min;
	size_on_two *= 0.5;

	int maxx, maxy, maxz;
	maxx = maxy = maxz = 2;

	unsigned int axismask = myFatherOctree->getAxisMask();

	switch (axismask)
	{
	case 0:
		break;
	case 1:
		maxx = 1;
		size_on_two.x = myBBox.m_Max.x - myBBox.m_Min.x;
		break;
	case 2:
		maxy = 1;
		size_on_two.y = myBBox.m_Max.y - myBBox.m_Min.y;
		break;
	case 4:
		maxz = 1;
		size_on_two.z = myBBox.m_Max.z - myBBox.m_Min.z;
		break;
	}

	for (i = 0; i < maxx; i++)
	{
		for (j = 0; j < maxy; j++)
		{
			for (k = 0; k < maxz; k++)
			{
				mySonsSubNodes[index]->myFatherSubNode = this;

				mySonsSubNodes[index]->myBBox.m_Min.x = myBBox.m_Min.x + (kfloat)i*size_on_two.x;
				mySonsSubNodes[index]->myBBox.m_Max.x = myBBox.m_Min.x + (kfloat)(i + 1)*size_on_two.x;

				mySonsSubNodes[index]->myBBox.m_Min.y = myBBox.m_Min.y + (kfloat)j*size_on_two.y;
				mySonsSubNodes[index]->myBBox.m_Max.y = myBBox.m_Min.y + (kfloat)(j + 1)*size_on_two.y;

				mySonsSubNodes[index]->myBBox.m_Min.z = myBBox.m_Min.z + (kfloat)k*size_on_two.z;
				mySonsSubNodes[index]->myBBox.m_Max.z = myBBox.m_Min.z + (kfloat)(k + 1)*size_on_two.z;

				mySonsSubNodes[index]->myLocalBBox = mySonsSubNodes[index]->myBBox;

				index++;
			}
		}
	}

	// then move node in this to nodes in sons

	// create temporary list of nodes not to move
	j = (int)myObjectList.size();
	SceneNode** tmpnodes = new SceneNode*[j];

	kstl::vector<SceneNode*>::iterator it;
	i = 0;

	for (it = myObjectList.begin(); it != myObjectList.end(); ++it)
	{
		index = FindSubNode(*it);
		if (index != -1)
		{
			tmpnodes[i] = 0;
			mySonsSubNodes[index]->myTotalNodes++;

			myFatherOctree->AddNodeToMap(*it, mySonsSubNodes[index].get());

			mySonsSubNodes[index]->myObjectList.push_back(*it);

			CMSP totreat(*it, StealRefTag{});

			mySonsSubNodes[index]->addItem(totreat);
			removeItem(totreat);
		}
		else
		{
			tmpnodes[i] = (*it);
		}
		i++;
	}

	myObjectList.clear();

	for (i = 0; i < j; i++)
	{
		if (tmpnodes[i])
		{
			myObjectList.push_back(tmpnodes[i]);
		}
	}

	delete[] tmpnodes;
}

int   OctreeSubNode::FindSubNode(BBox* bbox)
{
	int index;

	if (!mySonsSubNodes)
	{
		return -1;
	}
	int soncount = myFatherOctree->getSubdivisionCount();
	for (index = 0; index < soncount; index++)
	{
		if (mySonsSubNodes[index]->myBBox.IsInWithAxisMask(*bbox, myFatherOctree->getAxisMask()))
		{
			return index;
		}
	}

	return -1;

}

int   OctreeSubNode::FindSubNode(SceneNode* node)
{
	int index;

	BBox box;

	if (node->isUserFlagSet(UserFlagNode3D))
	{
		((Node3D*)node)->GetBoundingBox(box.m_Min, box.m_Max);
	}

	int soncount = myFatherOctree->getSubdivisionCount();
	for (index = 0; index < soncount; index++)
	{
		if (mySonsSubNodes[index]->myBBox.IsInWithAxisMask(box, myFatherOctree->getAxisMask()))
		{
			return index;
		}
	}

	return -1;
}

bool  OctreeSubNode::RemoveNode(SceneNode* node)
{
	kstl::vector<SceneNode*>::iterator it;
	bool found = false;

	myFatherOctree->myIsInRemove = true;
	for (it = myObjectList.begin(); it != myObjectList.end(); ++it)
	{
		if ((*it) == node)
		{
			found = true;
			CMSP todel(*it, StealRefTag{});
			removeItem(todel);
			myObjectList.erase(it);

			break;
		}
	}

	if (found)
	{
		myTotalNodes--;
		OctreeSubNode* father = myFatherSubNode;
		while (father)
		{
			father->myTotalNodes--;
			father = father->myFatherSubNode;
		}
	}
	myFatherOctree->myIsInRemove = false;
	return found;
}

DECLARE_DECORABLE_IMPLEMENT(bool, Cull, OctreeSubNode, TravState* state, unsigned int cullingMask)
{
	// just return precomputed cull result
	if (myCullingResult == CullingObject::all_out)
	{
		return false;
	}

	return true;
}

CullingObject::CULLING_RESULT  OctreeSubNode::RecurseCullSubNodes(const kstl::vector<PrecomputedCullInfo>& precull, TravState* state, unsigned int cullSubMask)
{
	myCullingMask = cullSubMask;
	UnsetAllSonsVisible();
	if (!myTotalNodes)
	{
		myCullingResult = CullingObject::all_out;
		myIsVisible = 0;
		return CullingObject::all_out;
	}

	CullingObject::CULLING_RESULT result = (CullingObject::CULLING_RESULT)0;

	kstl::vector<PrecomputedCullInfo>::const_iterator it;

	Vector3D  toTest1;
	Vector3D  toTest2;

	int index = 1;

	for (it = precull.begin(); it != precull.end(); ++it)
	{
		CullingObject::CULLING_RESULT localresult = CullingObject::all_in;
		const OctreeSubNode::PrecomputedCullInfo& current = (*it);
		if ((cullSubMask&index) == 0)
		{

			toTest1.x = myBBox[current.myPreTests[0]];
			toTest2.x = myBBox[3 - current.myPreTests[0]];

			toTest1.y = myBBox[1 + current.myPreTests[1]];
			toTest2.y = myBBox[4 - current.myPreTests[1]];

			toTest1.z = myBBox[2 + current.myPreTests[2]];
			toTest2.z = myBBox[5 - current.myPreTests[2]];

			toTest1 -= current.myOrigin;

			if (Dot(toTest1, current.myNormal) < (kfloat)0)
			{
				myCullingResult = CullingObject::all_out;
				myIsVisible = 0;
				return CullingObject::all_out;
			}

			toTest2 -= current.myOrigin;
			if (Dot(toTest2, current.myNormal) < (kfloat)0)
			{
				localresult = CullingObject::partially_in;
			}
			else
			{
				myCullingMask |= index;
			}
		}

		result = (CullingObject::CULLING_RESULT)(((unsigned int)result) | ((unsigned int)localresult));

		index = index << 1;
	}

	myCullingResult = result;
	myIsVisible = state->GetVisibilityFrame();

	if (result == CullingObject::partially_in)
	{
		if (mySonsSubNodes)
		{
			int i;
			int soncount = myFatherOctree->getSubdivisionCount();
			for (i = 0; i < soncount; i++)
			{
				mySonsSubNodes[i]->RecurseCullSubNodes(precull, state, myCullingMask);
			}

		}
	}
	else
	{
		SceneNode::SetAllSonsVisible();
	}

	return result;

}