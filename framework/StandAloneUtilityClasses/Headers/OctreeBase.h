#pragma once

#include "TecLibs/Tec3D.h"

class OctreeNodeBase
{
public:
	OctreeNodeBase()
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
	}

#ifdef _DEBUG
	static unsigned int getCurrentAllocatedNodeCount()
	{
		return mCurrentAllocatedNodeCount;
	}
#endif

protected:

#ifdef _DEBUG
	static unsigned int	mCurrentAllocatedNodeCount;
#endif
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
		// destroy sons
		collapse();
	}

	inline bool isLeaf()
	{
		// children are all set or all null, so test only one
		if (mChildren)
		{
			return false;
		}
		return true;
	}

	OctreeNodeBase* getChild(int index)
	{
		return static_cast<OctreeNodeBase*>(&(mChildren[index]));
	}

	// return content type of the node
	ContentType& getContentType()
	{
		return mContentType;
	}

	// destroy sons
	// methods don't test if collapse is valid or not
	// collapse children with different content is a bad idea
	void	collapse()
	{
		// destroy sons
		if (mChildren)
		{
			delete[] mChildren;
		}
		mChildren = nullptr;
	}

	// return depth of the node
	unsigned int getDepth()
	{
		if (isLeaf())
			return 0;

		unsigned int maxD = 0;
		for (int i = 0; i < 8; i++)
		{
			unsigned int cd = mChildren[i].getDepth();
			if (cd > maxD)
			{
				maxD = cd;
			}
		}
		return maxD + 1;
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
		if (isLeaf())
		{

			mChildren = new OctreeNode<ContentType>[8];
			for (int i = 0; i < 8; i++)
			{
				mChildren[i].setContentType(content);
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

	// return child at given index
	// don't test if index is in [0-7] but we suppose function is only called by VOctree
	OctreeNodeBase* getChild(unsigned int index)
	{
		return &(mChildren[index]);
	}

protected:
	OctreeNode*		mChildren = nullptr;
	ContentType		mContentType;

};


struct nodeInfo
{
	int				level;
	v3i				coord;
	OctreeNodeBase*	node = nullptr;

	template<typename T>
	T* getNode() const { return static_cast<T*>(node); }
};