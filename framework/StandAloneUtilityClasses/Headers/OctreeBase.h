#pragma once

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "TecLibs/Tec3D.h"


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

	inline bool isLeaf()
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
protected:

#ifdef _DEBUG
	static unsigned int	mCurrentAllocatedNodeCount;
#endif

	OctreeNodeBase** mChildren = nullptr;
	unsigned int	mBrowsingFlag;

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
		return mContentType.isEmpty();
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

	// get neighbour in the given direction ( as an index in mNeightboursDecalVectors)   
	nodeInfo	getVoxelNeighbour(const nodeInfo& node, int dir);


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

		
		recursiveFloodFill(OctreeBase<BaseType>& octree, std::vector<nodeInfo>* fillborderList) : 
			mOctree(octree),
			mFillBorderList(fillborderList)
		{
			reset();
		}
		~recursiveFloodFill() {}

		template<typename F>
		void run(const nodeInfo& node, F&& condition);

		void	reset()
		{
			mOctree.mCurrentBrowsingFlag++;
			if(mFillBorderList)
				mFillBorderList->clear();
		}

	private:
		std::vector<nodeInfo>*	mFillBorderList=nullptr;
		OctreeBase<BaseType>&	mOctree;
	};

	template<typename F>
	void	callRecurseFloodFill(const nodeInfo& startPos, std::vector<nodeInfo>& notEmptyList,F&& condition)
	{
		recursiveFloodFill t(*this, &notEmptyList);
		t.run(startPos,condition);
	}

	void	callRecurseFloodFill(const nodeInfo& startPos, std::vector<nodeInfo>& notEmptyList)
	{
		recursiveFloodFill t(*this, &notEmptyList);

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
nodeInfo	OctreeBase<BaseType>::getVoxelNeighbour(const nodeInfo& node, int dir)
{
	nodeInfo result;
	result.node = nullptr;

	const unsigned int maxSize = 2 << mMaxDepth;

	int dpos = 2 << (mMaxDepth - node.level);

	v3i	dposv(node.coord);

	dposv += mNeightboursDecalVectors[dir] * dpos;

	if (dir < 2)
	{
		if ((dposv.x >= 0) && (dposv.x < maxSize))
		{
			return getVoxelAt(dposv, node.level);
		}
		return result;
	}
	else if (dir < 4)
	{
		if ((dposv.y >= 0) && (dposv.y < maxSize))
		{
			return getVoxelAt(dposv, node.level);
		}
		return result;
	}
	else
	{
		if ((dposv.z >= 0) && (dposv.z < maxSize))
		{
			return getVoxelAt(dposv, node.level);
		}

	}
	return result;

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
void	OctreeBase<BaseType>::recursiveFloodFill::run(const nodeInfo& startPos, F&& condition)
{
	// set current node as "treated"
	startPos.node->setBrowsingFlag(mOctree.mCurrentBrowsingFlag);

	// for each adjacent node
	for (int dir = 0; dir < 6; dir++)
	{
		// get adjacent node
		nodeInfo	n = mOctree.getVoxelNeighbour(startPos, dir);

		if (n.node == nullptr) // TODO => outside of this octree -> should check other octrees
		{
			continue;
		}
		if (n.node->getBrowsingFlag() == mOctree.mCurrentBrowsingFlag) // already treated continue
		{
			continue;
		}

		std::vector< nodeInfo> child;
		if (n.node->isLeaf()) // if this node is a leaf, then this is the only one to treat
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
			if (c.node->getBrowsingFlag() != mOctree.mCurrentBrowsingFlag)
			{
				if (condition(c)) // recurse flood fill
				{
					run(c,condition);
					continue;
				}
				else // add this node to limit of flood fill list
				{
					c.node->setBrowsingFlag(mOctree.mCurrentBrowsingFlag);
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