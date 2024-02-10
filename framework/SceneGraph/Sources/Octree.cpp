#include "PrecompiledHeaders.h"

#include "Octree.h"
#include "TravState.h"
#include "ModuleSceneGraph.h"
#include "ModuleRenderer.h"
#include "Core.h"
#include "RendererMatrix.h"
#include "Drawable.h"
#include "CullingObject.h"

using namespace Kigs::Scene;

//IMPLEMENT_AND_REGISTER_CLASS_INFO(Octree, Octree, SceneGraph);
IMPLEMENT_CLASS_INFO(Octree)

IMPLEMENT_CONSTRUCTOR(Octree)
, mIsInRemove(false)
, mRootSubNode(nullptr)
, mQuadTreeAxis(*this,  "QuadTreeAxis", "None", "X", "Y", "Z")
, mIsQuadtree(false)
{
	setInitParameter("SubdivideLevel", true);
	setInitParameter("MaxRecursiveLevel", true);
	setInitParameter("BoundingBoxMin", true);
	setInitParameter("BoundingBoxMax", true);

	mPrecomputedCull.clear();
	mRootSubNode = KigsCore::GetInstanceOf("myRootSubNode", "OctreeSubNode");
	addItem(mRootSubNode);
	
	mRootSubNode->mFatherOctree = this;
	mRootSubNode->Init();
	mIsInReorganise = false;
	mFirstReorganiseWasDone = false;
}

void	Octree::InitModifiable()
{
	Node3D::InitModifiable();

	mLocalBBox.m_Min = mBoundingBoxMin;
	mLocalBBox.m_Max = mBoundingBoxMax;
	mBBox = mLocalBBox;
	ComputeGlobalBBox();

	mRootSubNode->mLocalBBox = mLocalBBox;
	mRootSubNode->mBBox = mBBox;
	mRootSubNode->mGlobalBBox = mGlobalBBox;

	mIsQuadtree = (((int)mQuadTreeAxis) != 0);
	mAxisMask = 0;
	if (mIsQuadtree)
	{
		mAxisMask = 1 << (((int)mQuadTreeAxis) - 1);
	}
}

void Octree::PropagateDirtyFlagsToParents(SceneNode* source)
{
	ParentClassType::PropagateDirtyFlagsToParents(source);
	if (mIsInReorganise) return;
	if (!mIsInRemove)
	{
		auto found = mObjectNodeAssociation.find(source);
		if (found != mObjectNodeAssociation.end())
		{
			OctreeSubNode* subnode = (*found).second;
			mObjectNodeAssociation.erase(found);  // avoid reentrance here
			if (subnode)
			{
				addItem(source->SharedFromThis());
				subnode->RemoveNode(source);
			}
		}
	}
}

void Octree::RecomputeBoundingBox()
{
	if (!mFirstReorganiseWasDone)
	{
		return;
	}

	mLocalBBox.m_Min = mBoundingBoxMin;
	mLocalBBox.m_Max = mBoundingBoxMax;

	mat4		BBoxTransformMatrix(mTransform);

	BBoxTransformMatrix[0][0] = fabsf(BBoxTransformMatrix[0][0]);
	BBoxTransformMatrix[1][0] = fabsf(BBoxTransformMatrix[1][0]);
	BBoxTransformMatrix[2][0] = fabsf(BBoxTransformMatrix[2][0]);

	BBoxTransformMatrix[0][1] = fabsf(BBoxTransformMatrix[0][1]);
	BBoxTransformMatrix[1][1] = fabsf(BBoxTransformMatrix[1][1]);
	BBoxTransformMatrix[2][1] = fabsf(BBoxTransformMatrix[2][1]);

	BBoxTransformMatrix[0][2] = fabsf(BBoxTransformMatrix[0][2]);
	BBoxTransformMatrix[1][2] = fabsf(BBoxTransformMatrix[1][2]);
	BBoxTransformMatrix[2][2] = fabsf(BBoxTransformMatrix[2][2]);

	// compute local translation of the bounding box

	v3f translation(mLocalBBox.m_Min);
	translation += mLocalBBox.m_Max;
	translation *= 0.5f;

	mBBox.m_Max = mLocalBBox.m_Max;
	mBBox.m_Max -= translation;

	transformVector(BBoxTransformMatrix,mBBox.m_Max);
	transformVector(mTransform ,translation);

	translation += v3f(column(mTransform,3));

	mBBox.m_Min = -mBBox.m_Max;
	mBBox.m_Max += translation;
	mBBox.m_Min += translation;


	mRootSubNode->mBBox = mLocalBBox;


	unsetUserFlag(BoundingBoxIsDirty);
}

bool Octree::Cull(TravState* state, unsigned int cullingMask)
{
	mCullingMask = cullingMask;
	if (!IsRenderable())
	{
		mIsVisible = 0;
		return false;
	}

	ReorganiseOctree(); // check if some reorganise is useful

	if (!state->IsAllVisible())
	{
		if (mIsVisible != state->GetVisibilityFrame())
		{
			UnsetAllSonsVisible();

			CullingObject::CULLING_RESULT result = CullSubNodes(state->GetCullingObject().get(), state);

			if (result == CullingObject::all_out)
			{
				mIsVisible = 0;
				return false;
			}

			if (result == CullingObject::all_in)
			{
				mIsVisible = state->GetVisibilityFrame();
				SceneNode::SetAllSonsVisible();
				return true;
			}
		}
	}

	mIsVisible = state->GetVisibilityFrame();
	return true;

}

CullingObject::CULLING_RESULT  Octree::CullSubNodes(CullingObject* cullobj, TravState* state)
{
	std::vector<OctreeSubNode::PrecomputedCullInfo>::iterator	itprecomp;

	if (mPrecomputedCull.size() == 0)
	{
		for (auto it = cullobj->GetCullPlaneList().begin(); it != cullobj->GetCullPlaneList().end(); ++it)
		{
			// create vector
			OctreeSubNode::PrecomputedCullInfo toAdd;
			mPrecomputedCull.push_back(toAdd);
		}
	}


	itprecomp = mPrecomputedCull.begin();
	// precompute
	for (auto it = cullobj->GetCullPlaneList().begin(); it != cullobj->GetCullPlaneList().end(); ++it)
	{
		OctreeSubNode::PrecomputedCullInfo& current = (*itprecomp);
		current.mOrigin = it->mOrigin;
		current.mNormal = it->mNormal;

		auto g2l = GetGlobalToLocal();
		transformVector(g2l ,current.mNormal);
		transformPoint(g2l, current.mOrigin);

		if (current.mNormal.x < 0.0f)
		{
			current.mPreTests[0] = 0;
		}
		else
		{
			current.mPreTests[0] = 3;
		}

		if (current.mNormal.y < 0.0f)
		{
			current.mPreTests[1] = 0;
		}
		else
		{
			current.mPreTests[1] = 3;
		}

		if (current.mNormal.z < 0.0f)
		{
			current.mPreTests[2] = 0;
		}
		else
		{
			current.mPreTests[2] = 3;
		}

		++itprecomp;
	}

	return mRootSubNode->RecurseCullSubNodes(mPrecomputedCull, state, mCullingMask);

	//mCullingMask=myRootSubNode->mCullingMask;
}


// manage octree remove
bool	Octree::removeItem(const CMSP& item DECLARE_LINK_NAME)
{
	bool foundsubnode = false;
	if (item->isSubType("SceneNode"))
	{
		if (!item->isSubType("OctreeSubNode"))
		{
			OctreeSubNode* subnode = mObjectNodeAssociation[(SceneNode*)item.get()];
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
				mObjectNodeAssociation[(SceneNode*)item.get()] = 0;
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
	mIsInReorganise = true;

	if (!mFirstReorganiseWasDone)
	{
		mFirstReorganiseWasDone = true;
		setUserFlag(BoundingBoxIsDirty);
		setUserFlag(GlobalBoundingBoxIsDirty);
		PropagateDirtyFlags(this);
		RecomputeBoundingBox();
	}

	std::vector<CMSP> toRemove;
	// add all concerned object to the octree
	for (auto mis : getItems())
	{
		CMSP item = mis.mItem;

		if (item->isUserFlagSet(UserFlagNode3D))
		{
			// make sure the bounding box, matrix... are ok before adding
			if (!item->isSubType("OctreeSubNode"))
			{
				SP<OctreeSubNode> subNode = mRootSubNode->AddNode(item, 0, mMaxRecursiveLevel, mSubdivideLevel);

				AddNodeToMap((SceneNode*)item.get(), subNode.get());
				toRemove.push_back(item);
			}
		}
	}

	for (auto todel : toRemove)
	{
		Node3D::removeItem(todel);
	}
	mIsInReorganise = false;
}

void  Octree::AddNodeToMap(SceneNode* node, OctreeSubNode* subnode)
{
	if (subnode)
	{
		mObjectNodeAssociation[node] = subnode;
	}
}


//IMPLEMENT_AND_REGISTER_CLASS_INFO(OctreeSubNode, OctreeSubNode, SceneGraph);
IMPLEMENT_CLASS_INFO(OctreeSubNode)



OctreeSubNode::OctreeSubNode(const std::string& name, CLASS_NAME_TREE_ARG) :
	Node3D(name, PASS_CLASS_NAME_TREE_ARG)
	, mSonsSubNodes(0)
	, mFatherSubNode(0)
	, mTotalNodes(0)
	, mFatherOctree(0)
{
	setUserFlag(UserFlagCameraSort);
	mObjectList.clear();
}

OctreeSubNode::~OctreeSubNode()
{
	if (mSonsSubNodes)
	{
		delete[] mSonsSubNodes;
	}
}

void  OctreeSubNode::KillSons()
{
	int soncount = mFatherOctree->getSubdivisionCount();
	int index;
	for (index = 0; index < soncount; index++)
	{
		removeItem(mSonsSubNodes[index]);
	}
	delete[] mSonsSubNodes;
	mSonsSubNodes = nullptr;
}

bool  OctreeSubNode::SonsAreEmpty()
{
	if (mSonsSubNodes)
	{
		int index;
		int count = 0;
		int soncount = mFatherOctree->getSubdivisionCount();

		for (index = 0; index < soncount; index++)
		{
			count += mSonsSubNodes[index]->mTotalNodes;
		}

		return (count == 0);

	}

	return true;
}

SP<OctreeSubNode>  OctreeSubNode::AddNode(SP<SceneNode> node, int currentlevel, int maxLevel, int subdivelevel)
{
	// add it to me
	if ((mTotalNodes < subdivelevel) || (currentlevel >= maxLevel))
	{
		mObjectList.push_back(node.get());
		addItem(node);

		mTotalNodes++;
		return SharedFromThis();
	}
	else // try to add it to sons
	{
		// check if we have to create sons
		if (!mSonsSubNodes)
		{
			Divide();
		}

		int index = FindSubNode(node.get());
		if (index != -1)
		{
			mTotalNodes++;
			return mSonsSubNodes[index]->AddNode(node, currentlevel + 1, maxLevel, subdivelevel);
		}
		else
		{
			mObjectList.push_back(node.get());
			addItem(node);
			mTotalNodes++;
			return SharedFromThis();
		}
	}
}

void  OctreeSubNode::Divide()
{
	int soncount = mFatherOctree->getSubdivisionCount();

	mSonsSubNodes = new SP<OctreeSubNode>[soncount];

	// compute son bounding box
	int i, j, k, index;

	for (i = 0; i < soncount; i++)
	{
		mSonsSubNodes[i] = KigsCore::GetInstanceOf("mySonsSubNodes", "OctreeSubNode");
		addItem(mSonsSubNodes[i]);
		mSonsSubNodes[i]->mFatherOctree = mFatherOctree;
		mSonsSubNodes[i]->Init();
	}

	index = 0;

	v3f size_on_two = mBBox.m_Max;
	size_on_two -= mBBox.m_Min;
	size_on_two *= 0.5;

	int maxx, maxy, maxz;
	maxx = maxy = maxz = 2;

	unsigned int axismask = mFatherOctree->getAxisMask();

	switch (axismask)
	{
	case 0:
		break;
	case 1:
		maxx = 1;
		size_on_two.x = mBBox.m_Max.x - mBBox.m_Min.x;
		break;
	case 2:
		maxy = 1;
		size_on_two.y = mBBox.m_Max.y - mBBox.m_Min.y;
		break;
	case 4:
		maxz = 1;
		size_on_two.z = mBBox.m_Max.z - mBBox.m_Min.z;
		break;
	}

	for (i = 0; i < maxx; i++)
	{
		for (j = 0; j < maxy; j++)
		{
			for (k = 0; k < maxz; k++)
			{
				mSonsSubNodes[index]->mFatherSubNode = this;

				mSonsSubNodes[index]->mBBox.m_Min.x = mBBox.m_Min.x + (float)i*size_on_two.x;
				mSonsSubNodes[index]->mBBox.m_Max.x = mBBox.m_Min.x + (float)(i + 1)*size_on_two.x;

				mSonsSubNodes[index]->mBBox.m_Min.y = mBBox.m_Min.y + (float)j*size_on_two.y;
				mSonsSubNodes[index]->mBBox.m_Max.y = mBBox.m_Min.y + (float)(j + 1)*size_on_two.y;

				mSonsSubNodes[index]->mBBox.m_Min.z = mBBox.m_Min.z + (float)k*size_on_two.z;
				mSonsSubNodes[index]->mBBox.m_Max.z = mBBox.m_Min.z + (float)(k + 1)*size_on_two.z;

				mSonsSubNodes[index]->mLocalBBox = mSonsSubNodes[index]->mBBox;

				index++;
			}
		}
	}

	// then move node in this to nodes in sons

	// create temporary list of nodes not to move
	j = (int)mObjectList.size();
	SceneNode** tmpnodes = new SceneNode*[j];

	std::vector<SceneNode*>::iterator it;
	i = 0;

	for (it = mObjectList.begin(); it != mObjectList.end(); ++it)
	{
		index = FindSubNode(*it);
		if (index != -1)
		{
			tmpnodes[i] = 0;
			mSonsSubNodes[index]->mTotalNodes++;

			mFatherOctree->AddNodeToMap(*it, mSonsSubNodes[index].get());

			mSonsSubNodes[index]->mObjectList.push_back(*it);

			auto to_treat = (*it)->SharedFromThis();
			mSonsSubNodes[index]->addItem(to_treat);
			removeItem(to_treat);
		}
		else
		{
			tmpnodes[i] = (*it);
		}
		i++;
	}

	mObjectList.clear();

	for (i = 0; i < j; i++)
	{
		if (tmpnodes[i])
		{
			mObjectList.push_back(tmpnodes[i]);
		}
	}

	delete[] tmpnodes;
}

int   OctreeSubNode::FindSubNode(BBox* bbox)
{
	int index;

	if (!mSonsSubNodes)
	{
		return -1;
	}
	int soncount = mFatherOctree->getSubdivisionCount();
	for (index = 0; index < soncount; index++)
	{
		if (mSonsSubNodes[index]->mBBox.IsInWithAxisMask(*bbox, mFatherOctree->getAxisMask()))
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

	int soncount = mFatherOctree->getSubdivisionCount();
	for (index = 0; index < soncount; index++)
	{
		if (mSonsSubNodes[index]->mBBox.IsInWithAxisMask(box, mFatherOctree->getAxisMask()))
		{
			return index;
		}
	}

	return -1;
}

bool  OctreeSubNode::RemoveNode(SceneNode* node)
{
	std::vector<SceneNode*>::iterator it;
	bool found = false;

	mFatherOctree->mIsInRemove = true;
	for (it = mObjectList.begin(); it != mObjectList.end(); ++it)
	{
		if ((*it) == node)
		{
			found = true;
			removeItem(node->SharedFromThis());
			mObjectList.erase(it);
			break;
		}
	}

	if (found)
	{
		mTotalNodes--;
		OctreeSubNode* father = mFatherSubNode;
		while (father)
		{
			father->mTotalNodes--;
			father = father->mFatherSubNode;
		}
	}
	mFatherOctree->mIsInRemove = false;
	return found;
}

bool  OctreeSubNode::Cull(TravState* state, unsigned int cullingMask)
{
	// just return precomputed cull result
	if (mCullingResult == CullingObject::all_out)
	{
		return false;
	}

	return true;
}

CullingObject::CULLING_RESULT  OctreeSubNode::RecurseCullSubNodes(const std::vector<PrecomputedCullInfo>& precull, TravState* state, unsigned int cullSubMask)
{
	mCullingMask = cullSubMask;
	UnsetAllSonsVisible();
	if (!mTotalNodes)
	{
		mCullingResult = CullingObject::all_out;
		mIsVisible = 0;
		return CullingObject::all_out;
	}

	CullingObject::CULLING_RESULT result = (CullingObject::CULLING_RESULT)0;

	std::vector<PrecomputedCullInfo>::const_iterator it;

	v3f  toTest1;
	v3f  toTest2;

	int index = 1;

	for (it = precull.begin(); it != precull.end(); ++it)
	{
		CullingObject::CULLING_RESULT localresult = CullingObject::all_in;
		const OctreeSubNode::PrecomputedCullInfo& current = (*it);
		if ((cullSubMask&index) == 0)
		{

			toTest1.x = mBBox[current.mPreTests[0]];
			toTest2.x = mBBox[3 - current.mPreTests[0]];

			toTest1.y = mBBox[1 + current.mPreTests[1]];
			toTest2.y = mBBox[4 - current.mPreTests[1]];

			toTest1.z = mBBox[2 + current.mPreTests[2]];
			toTest2.z = mBBox[5 - current.mPreTests[2]];

			toTest1 -= (v3f&)current.mOrigin;

			if (dot(toTest1, current.mNormal) < (float)0)
			{
				mCullingResult = CullingObject::all_out;
				mIsVisible = 0;
				return CullingObject::all_out;
			}

			toTest2 -= (v3f&)current.mOrigin;
			if (dot(toTest2, current.mNormal) < (float)0)
			{
				localresult = CullingObject::partially_in;
			}
			else
			{
				mCullingMask |= index;
			}
		}

		result = (CullingObject::CULLING_RESULT)(((unsigned int)result) | ((unsigned int)localresult));

		index = index << 1;
	}

	mCullingResult = result;
	mIsVisible = state->GetVisibilityFrame();

	if (result == CullingObject::partially_in)
	{
		if (mSonsSubNodes)
		{
			int i;
			int soncount = mFatherOctree->getSubdivisionCount();
			for (i = 0; i < soncount; i++)
			{
				mSonsSubNodes[i]->RecurseCullSubNodes(precull, state, mCullingMask);
			}

		}
	}
	else
	{
		SceneNode::SetAllSonsVisible();
	}

	return result;

}