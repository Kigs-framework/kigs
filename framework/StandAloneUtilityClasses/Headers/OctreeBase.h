#pragma once

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "TecLibs/Tec3D.h"

template<typename BaseType>
class OctreeBase;

class OctreeNodeBase
{
public:
	OctreeNodeBase() : mBrowsingFlag(0)
	{
#ifdef _DEBUG
		mCurrentAllocatedNodeCount++;
#endif
	}

	virtual ~OctreeNodeBase()
	{
#ifdef _DEBUG
		mCurrentAllocatedNodeCount--;
#endif
		// destroy sons
		collapse();
	}

#ifdef _DEBUG
	static unsigned int getCurrentAllocatedNodeCount()
	{
		return mCurrentAllocatedNodeCount;
	}
#endif

	inline bool isLeaf() const
	{
		// children are all set or all null, so test only one
		if (mChildren)
		{
			return false;
		}
		return true;
	}

	virtual void	initChildrenArray() = 0;
	virtual void	destroyChildrenArray() = 0;
	virtual bool	isEmpty() const = 0;

	// try to split node ( create 8 children )
	// only leaf nodes can be splitted 
	bool split()
	{
		if (isLeaf())
		{

			mChildren = new OctreeNodeBase*[8];

			initChildrenArray();

			return true;
		}
		return false;
	}
	

	// return depth of the node
	unsigned int getDepth()
	{
		if (isLeaf())
			return 0;

		unsigned int maxD = 0;
		for (int i = 0; i < 8; i++)
		{
			unsigned int cd = mChildren[i]->getDepth();
			if (cd > maxD)
			{
				maxD = cd;
			}
		}
		return maxD + 1;
	}

	OctreeNodeBase* getChild(int index)
	{
		return mChildren[index];
	}

	// destroy sons
	// methods don't test if collapse is valid or not
	// collapse children with different content is a bad idea
	void	collapse()
	{
		// destroy sons
		if (mChildren)
		{
			destroyChildrenArray();
			delete[] mChildren;
		}
		mChildren = nullptr;
	}
	void	setBrowsingFlag(unsigned int f)
	{
		mBrowsingFlag = f;
	}
	unsigned int	getBrowsingFlag() const
	{
		return mBrowsingFlag;
	}

	void clearNFlag()
	{
		mDirNDoneFlag = 0;
	}
protected:
	template<typename BaseType>
	friend class OctreeBase;

#ifdef _DEBUG
	static unsigned int	mCurrentAllocatedNodeCount;
#endif

	OctreeNodeBase** mChildren = nullptr;
	unsigned int	mBrowsingFlag;
	// check if neighbor already visited
	u32				mDirNDoneFlag = 0;

};


template<typename ContentType>
class OctreeNode : public OctreeNodeBase
{
public:

	OctreeNode() : OctreeNodeBase()
	{

	}
	OctreeNode(const ContentType& content) : OctreeNodeBase()
		,mContentType(content)
	{
	}

	virtual ~OctreeNode()
	{
		
	}

	// return content type of the node
	ContentType& getContentType()
	{
		return mContentType;
	}

	// try to set content of the node
	// content tells itself if it can be affected to a leaf 
	// return true if content was set
	bool	setContentType(const ContentType& content)
	{
		if (content.canOnlyBeSetOnLeaf())
		{
			if (isLeaf())
			{
				mContentType = content;
				return true;
			}
		}
		else
		{
			mContentType = content;
			return true;
		}
		return false;
	}

	// try to split node ( create 8 children )
	// only leaf nodes can be splitted 
	bool split(const ContentType& content)
	{
		if(OctreeNodeBase::split())
		{
			for (int i = 0; i < 8; i++)
			{
				static_cast<OctreeNode<ContentType>*>(mChildren[i])->setContentType(content);
			}

			return true;
		}
		return false;
	}

	// recompute mContentType according to sons content type
	// if all childs have same contentType collapse node
	// return true if mContentType was changed 
	bool refresh()
	{
		if (isLeaf())
		{
			return false;
		}

		bool doCollapse = false;
		const ContentType& collapseType = ContentType::canCollapse(mChildren, doCollapse);
		
		if (doCollapse)
		{
			mContentType = collapseType;
			collapse();
			return true;
		}

		if (collapseType == mContentType)
		{
			return false;
		}

		mContentType = collapseType;
		return true;
	}

	void	initChildrenArray() override
	{
		OctreeNode<ContentType>*	children= new OctreeNode<ContentType>[8];
		for (int i = 0; i < 8; i++)
		{
			mChildren[i] = &(children[i]);
		}
	}
	void	destroyChildrenArray() override
	{
		delete[] mChildren[0];
	}

	virtual bool	isEmpty() const override
	{
		if (isLeaf()) // only leaf can be empty
			return mContentType.isEmpty();

		return false;
	}

protected:
	
	ContentType		mContentType;


};


struct nodeInfo
{
	int				level;
	v3i				coord;
	OctreeNodeBase*	node = nullptr;

	bool	operator==(const nodeInfo& other)
	{
		return (node == other.node);
	}

	template<typename T>
	T* getNode() const { return static_cast<T*>(node); }
};


template<typename BaseType>
class OctreeBase : public BaseType
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(OctreeBase<BaseType>, BaseType, Core);
	DECLARE_INLINE_CONSTRUCTOR(OctreeBase)
	{
	}

#ifdef _DEBUG
	void	printAllocatedNodeCount()
	{
		printf("allocated nodes : %d\n", OctreeNodeBase::getCurrentAllocatedNodeCount());
	}
#endif

	// return max depth in octree
	unsigned int getMaxDepth()
	{
		return mRootNode->getDepth();
	}

	nodeInfo getVoxelAt(const v3i& coordinate, unsigned int maxDepth = (unsigned int)-1);

	// set coord to real cube center
	// minimal cube is 2 units wide
	void	setValidCubeCenter(v3i& pos, unsigned int decal);

	// convert one of the 6 main direction to corresponding dir mask (2 bits per axis)
	u32 mainDirToDirMask(u32 dir)
	{
		u32 mask = 2; // 10 => negative way on axis
		if (dir & 1) // positive way on axis
		{
			mask = 1; // 01
		}

		dir = dir >> 1;

		mask = mask << (dir << 1);

		return mask;
	}

	

	// get neighbour in the given direction mask (add index in mNeightboursDecalVectors)
	// each axis is coded on two bits:
	// 00 =>  0
	// 10 => -1
	// 01 =>  1


	nodeInfo	getVoxelNeighbour(const nodeInfo& node, u32 dir);


	class	applyOnAllNodes
	{
	public:

		template<typename F>
		applyOnAllNodes(OctreeBase<BaseType>& octree, F&& toapply)
		{
			run(octree.mRootNode, toapply);
		}

		template<typename F>
		void run(OctreeNodeBase* currentNode, F&& toapply);

		~applyOnAllNodes() {}
	};

protected:



	// utility class to avoid passing the same parameters to the recursive method
	// and mutualise some computation 
	class recurseVoxelSideChildren
	{
	public:

		recurseVoxelSideChildren()
		{

		}

		recurseVoxelSideChildren(int dir, OctreeBase<BaseType>& octree, std::vector<nodeInfo>* child)
		{
			reset(dir, octree, child);
		}
		~recurseVoxelSideChildren() {}

		void run(const nodeInfo& node);

		void	reset(int dir, OctreeBase<BaseType>& octree, std::vector<nodeInfo>* child)
		{
			mMaxDepth = octree.mMaxDepth;
			mChildList = child;
			mDir = dir;
			mMaskTest = 1 << (mDir / 2);
			mMaskResult = (dir & 1) * mMaskTest;
		}

	private:
		unsigned int mMaskTest;
		unsigned int mMaskResult;
		int			 mMaxDepth;

		int							mDir;
		std::vector<nodeInfo>* mChildList;
	};


	class recursiveFloodFill
	{
	public:

		
		recursiveFloodFill(OctreeBase<BaseType>& octree, std::vector<nodeInfo>* fillborderList,s32 setBrowsingFlag=-1) : 
			mOctree(octree),
			mFillBorderList(fillborderList)
		{
			reset(setBrowsingFlag);
		}
		~recursiveFloodFill() {}

		template<typename F>
		void run(const nodeInfo& node, F&& condition)
		{
			mToParseList.push_back(node);

			while (mToParseList.size())
			{
				nodeInfo toparse = mToParseList.back();
				mToParseList.pop_back();

				if (toparse.node->getBrowsingFlag() != mOctree.mCurrentBrowsingFlag)
				{
					subrun(toparse, condition);
				}
			}
		}

		void	reset(s32 setBrowsingFlag)
		{
			if (setBrowsingFlag == -1)
			{
				mOctree.mCurrentBrowsingFlag++;
			}
			else
			{
				mOctree.mCurrentBrowsingFlag = setBrowsingFlag;
			}
			if(mFillBorderList)
				mFillBorderList->clear();

			mToParseList.clear();
		}

	protected:

		template<typename F>
		void subrun(const nodeInfo& node, F&& condition);

	private:

		std::vector<nodeInfo>*	mFillBorderList=nullptr;
		OctreeBase<BaseType>&	mOctree;
		std::vector<nodeInfo>	mToParseList;

	};

	template<typename F>
	void	callRecurseFloodFill(const nodeInfo& startPos, std::vector<nodeInfo>& notEmptyList,F&& condition, u32 setBrowsingFlag)
	{
		recursiveFloodFill t(*this, &notEmptyList, setBrowsingFlag);
		t.run(startPos,condition);
	}

	void	callRecurseFloodFill(const nodeInfo& startPos, std::vector<nodeInfo>& notEmptyList,u32 setBrowsingFlag)
	{
		recursiveFloodFill t(*this, &notEmptyList, setBrowsingFlag);

		auto condition = [](const nodeInfo& c)->bool
		{
			return c.node->isEmpty();
		};

		t.run(startPos, condition);
	}


	// default depth max is 8 => 256x256x256 cubes = 512 x 512 x 512 units
	maInt		mMaxDepth = INIT_ATTRIBUTE(MaxDepth, 8);

	OctreeNodeBase* mRootNode = nullptr;

	unsigned int	mCurrentBrowsingFlag = 0;


	static inline const v3i				mNeightboursDecalVectors[6] = { {-1,0,0},{1,0,0},{0,-1,0},{0,1,0},{0,0,-1},{0,0,1} };
	static inline const int				mInvDir[6] = { 1,0,3,2,5,4 };
};

IMPLEMENT_TEMPLATE_CLASS_INFO(BaseType, OctreeBase)

template<typename BaseType>
nodeInfo OctreeBase<BaseType>::getVoxelAt(const v3i& coordinate, unsigned int maxDepth)
{
	nodeInfo result;
	result.coord = coordinate;
	result.level = 0;
	result.node = mRootNode;
	int currentDecal = mMaxDepth;
	do
	{
		if (result.node->isLeaf())
		{
			break;
		}
#ifdef _DEBUG
		if (result.level >= mMaxDepth)
		{
			result.node = nullptr;
			printf("should not occur\n");
			break;
		}
#endif
		if (result.level >= maxDepth)
		{
			break;
		}

		unsigned int index = ((coordinate.x >> currentDecal) & 1) | (((coordinate.y >> currentDecal) & 1) << 1) | (((coordinate.z >> currentDecal) & 1) << 2);
		result.node = result.node->getChild(index);
		result.level++;
		currentDecal--;

	} while (1);

	setValidCubeCenter(result.coord, currentDecal);
	return result;
}

template<typename BaseType>
void	OctreeBase<BaseType>::setValidCubeCenter(v3i& pos, unsigned int decal)
{
	// set coord center according to level
	unsigned int mask = 1 << (decal + 1);
	mask--;
	mask ^= 0xFFFFFFFF;
	unsigned int half = 1 << decal;

	pos.x = (pos.x & mask) + half;
	pos.y = (pos.y & mask) + half;
	pos.z = (pos.z & mask) + half;
}

template<typename BaseType>
nodeInfo	OctreeBase<BaseType>::getVoxelNeighbour(const nodeInfo& node, u32 dirmask)
{
	nodeInfo result;
	result.node = nullptr;

	const unsigned int maxSize = 2 << mMaxDepth;

	int dpos = 2 << (mMaxDepth - node.level);

	v3i	dposv(node.coord);

	for (u32 axis = 0; axis < 3; axis++)
	{

		// 00 =>  0
		// 10 => -1
		// 01 =>  1

		u32 daxis = (axis << 1);
		u32 currentMask = (dirmask >> daxis) & 3;

		if (currentMask)
		{
			// should compute each axis separately
			dposv += mNeightboursDecalVectors[daxis+(currentMask&1)] * dpos;
		}
		if ((dposv[axis] < 0) || (dposv[axis] >= maxSize))
		{
			return result;
		}
	}

	return getVoxelAt(dposv, node.level);
}

template<typename BaseType>
void	OctreeBase<BaseType>::recurseVoxelSideChildren::run(const nodeInfo& node)
{

	int dpos = 1 << (mMaxDepth - (node.level + 1));

	// check all sons
	for (int c = 0; c < 8; c++)
	{
		if ((c & mMaskTest) == mMaskResult)
		{
			nodeInfo toAdd;
			toAdd.node = node.node->getChild(c);
			toAdd.level = node.level + 1;

			// add block so childCoord is not pushed for recursion
			{
				v3i childCoord = node.coord;
				childCoord.x += (((c & 1) << 1) - 1) * dpos;
				childCoord.y += ((c & 2) - 1) * dpos;
				childCoord.z += (((c & 4) >> 1) - 1) * dpos;

				toAdd.coord = childCoord;
			}

			if (toAdd.node->isLeaf())
			{
				(*mChildList).push_back(toAdd);
			}
			else
			{
				run(toAdd);
			}
		}
	}

}

template<typename BaseType>
template<typename F>
void OctreeBase<BaseType>::applyOnAllNodes::run(OctreeNodeBase* currentnode, F&& toapply)
{
	toapply(currentnode);
	if (currentnode->mChildren)
	{
		for (int i = 0; i < 8; i++)
		{
			auto sonNode = currentnode->mChildren[i];
			run(sonNode, toapply);
		}
	}
}


template<typename BaseType>
template<typename F>
void	OctreeBase<BaseType>::recursiveFloodFill::subrun(const nodeInfo& startPos, F&& condition)
{
	// set current node as "treated"
	startPos.node->setBrowsingFlag(mOctree.mCurrentBrowsingFlag);

	// for each adjacent node
	for (int dir = 0; dir < 6; dir++)
	{
		if (startPos.node->mDirNDoneFlag & (1 << dir))
			continue;
		// get adjacent node
		nodeInfo	n = mOctree.getVoxelNeighbour(startPos, mOctree.mainDirToDirMask(dir));

		if (n.node == nullptr) // TODO ? => outside of this octree -> should check other octrees
		{
			continue;
		}

		if (n.node->getBrowsingFlag() == mOctree.mCurrentBrowsingFlag) // already treated continue
		{
			continue;
		}

		std::vector< nodeInfo> child;
		if (n.node->isLeaf()) // if this node is a leaf then this is the only one to treat
		{
			child.push_back(n);
		}
		else // else get all sons on the correct side of n
		{
			recurseVoxelSideChildren r(mInvDir[dir], mOctree, &child);
			r.run(n);
		}

		// for all the found nodes, check if they need to be added to visible list or to be flood fill
		for (auto& c : child)
		{
			// mark the direction from where the node was added
			c.node->mDirNDoneFlag |= (1 << mInvDir[dir]);
			if (c.node->getBrowsingFlag() != mOctree.mCurrentBrowsingFlag)
			{
				if (condition(c)) // recurse flood fill
				{
					mToParseList.push_back(c);
					continue;
				}
				else // add this node to limit of flood fill list
				{
					//c.node->setBrowsingFlag(mOctree.mCurrentBrowsingFlag);
					if (mFillBorderList)
					{
						if (std::find((*mFillBorderList).begin(), (*mFillBorderList).end(), c) == (*mFillBorderList).end())
						{
							(*mFillBorderList).push_back(c);
						}
					}
				}
			}
		}
	}
}