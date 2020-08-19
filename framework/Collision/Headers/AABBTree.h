#ifndef _AABBTREE_H_
#define _AABBTREE_H_

#include "CoreModifiable.h"
#include "TecLibs/Tec3D.h"
#include "TecLibs/3D/3DObject/BBox.h"
#include "Node3D.h"
#include "CollisionBaseObject.h"

#include "DynamicGrowingBuffer.h"

class ModernMesh;
class AABBTree;


// ****************************************
// * AABBTreeNode class
// * --------------------------------------
/**
* \file	AABBTree.h
* \class	AABBTreeNode
* \ingroup Collision
* \brief Node structure in a Axis Aligned Bounding box tree, used to optimise intersection calculation.
*/
// ****************************************

class AABBTreeNode 
{
public:



	auto GetSon1() const { return mSon1; }
	auto GetSon2() const { return mSon2; }

	const BBox& GetBBox() const { return mBBox; }

	auto GetTriangleCount() const { return mTriangleCount; }
	auto GetTriangleArray() const { return mTriangleArray2; }

	template<typename PacketStream>
	bool SerializeNode(PacketStream& stream, AABBTree* root_node);

protected:
	class NormalTriangle
	{
	public:
		u8 data[3*4];
		
		u32 A() const { u32 result=0; memcpy(&result, &data[0], 3); return result; }
		u32 B() const { u32 result=0; memcpy(&result, &data[3], 3); return result; }
		u32 C() const { u32 result=0; memcpy(&result, &data[6], 3); return result; }
		u32 TriangleIndex() const { u32 result = 0; memcpy(&result, &data[9], 3); return result; }

		void SetA(u32 value) { memcpy(&data[0], &value, 3); }
		void SetB(u32 value) { memcpy(&data[3], &value, 3); }
		void SetC(u32 value) { memcpy(&data[6], &value, 3); }
		void SetTriangleIndex(u32 value) {memcpy(&data[9], &value, 3); }
	};

	class BuildTriangle_base
	{
	public:
		/*! \brief BBox of this triangle
		*/
		BBox mBBox;
		Point3D mMid;

		virtual BuildTriangle_base* Next() = 0;
		virtual void SetIndex(void *iArray, int first) = 0;
		virtual void GetIndex(unsigned int &a, unsigned int &b, unsigned int &c) = 0;
		virtual void ComputeMidAndBBox(Point3D *vArray) = 0;
		virtual int GetStructSize() = 0;


		static int CompareX(const void *A, const void *B)
		{
			if (static_cast<const BuildTriangle_base*>(A)->mMid.x < static_cast<const BuildTriangle_base*>(B)->mMid.x)
				return -1;
			else	if (static_cast<const BuildTriangle_base*>(A)->mMid.x > static_cast<const BuildTriangle_base*>(B)->mMid.x)
				return 1;

			return 0;
		}

		static int CompareY(const void *A, const void *B)
		{
			if (static_cast<const BuildTriangle_base*>(A)->mMid.y < static_cast<const BuildTriangle_base*>(B)->mMid.y)
				return -1;
			else	if (static_cast<const BuildTriangle_base*>(A)->mMid.y > static_cast<const BuildTriangle_base*>(B)->mMid.y)
				return 1;

			return 0;
		}

		static int CompareZ(const void *A, const void *B)
		{
			if (static_cast<const BuildTriangle_base*>(A)->mMid.z < static_cast<const BuildTriangle_base*>(B)->mMid.z)
				return -1;
			else	if (static_cast<const BuildTriangle_base*>(A)->mMid.z > static_cast<const BuildTriangle_base*>(B)->mMid.z)
				return 1;

			return 0;
		}
	};

	template<typename t>
	class BuildTriangle : public BuildTriangle_base
	{
	public:

		/*! \brief pointer to triangle index in the indexBuffer
		*/
		
		t* mIndex;
		u32 mTriangleIndex=0;

		virtual BuildTriangle_base* Next() { return (this)+1; }

		virtual int GetStructSize() { return sizeof(BuildTriangle<t>); }

		virtual void SetIndex(void *iArray, int first)
		{
			mIndex = &reinterpret_cast<t*>(iArray)[first];
			mTriangleIndex = first/3;
		}

		virtual void GetIndex(unsigned int &a, unsigned int &b, unsigned int &c)
		{
			a = mIndex[0];
			b = mIndex[1];
			c = mIndex[2];
		}

		/*! \brief construct BBOX and middle point of this triangle
		*/
		virtual void ComputeMidAndBBox(Point3D *vArray)
		{
			mMid = (vArray[mIndex[0]] + vArray[mIndex[1]] + vArray[mIndex[2]]) / 3.0f;
			mBBox.Init(vArray[mIndex[0]]);
			mBBox.Update(vArray[mIndex[1]]);
			mBBox.Update(vArray[mIndex[2]]);
		}
	};



protected:
	template<typename t>
	void Build2(AABBTree* root,BuildTriangle<t>* TrArray, BuildTriangle<t>* TmpTrArray, unsigned int TrCount, Point3D* VertexArray,const int leafSize,int lastSortMode,BBox currentbbox);

	void RescursiveSearchMesh(Node3D *root, kstl::vector<ModernMesh*>	&list);
	
	/*! \brief triangle count in this BBox
	*/
	int mTriangleCount = 0;

public:
	/*! \brief sons. AABBTree is a binary tree 
	*/
	AABBTreeNode *mSon1 = nullptr;

	// if node is a leaf, then TriangleCount is > 0 and mTriangleArray2 is defined, Son1 & mSon2 are null
	// else TriangleCount = 0 and Son1 & mSon2 are defined
	union
	{
		AABBTreeNode	* mSon2 = nullptr;
		NormalTriangle * mTriangleArray2;
	};

	/*! \brief BBox for this node 
	*/
	BBox mBBox;

public:	
	/*! \brief constructor 
	*/
	AABBTreeNode(); 

	/*! \brief destructor 
	*/
	virtual ~AABBTreeNode();
	



#ifdef KIGS_TOOLS
	// used for BBox drawing
	bool mHit = false;
#endif
	friend class AABBTreeNode;
	friend class Collision;
};

// ****************************************
// * AABBTree class
// * --------------------------------------
/**
* \file	AABBTree.h
* \class	AABBTree
* \ingroup Collision
* \brief Root ABBTreeNode + all buffers to manage collisions.
*/
// ****************************************

class AABBTree : public CollisionBaseObject, public AABBTreeNode
{
protected:
	NormalTriangle * mNormalTriangleArray = nullptr;
	unsigned int	 mCurrentFreeTriangleIndex = 0;
	unsigned int	 mCurrentFreeAABBTreeNode = 0;
	NormalTriangle * getFreeNormalTriangleBuffer(int trcount)
	{
		NormalTriangle * result = &mNormalTriangleArray[mCurrentFreeTriangleIndex];
		mCurrentFreeTriangleIndex += trcount;
		return result;
	}

	std::string mFileName;

	Point3D *		mVertexList = nullptr;
	unsigned int	mVertexCount = 0;

	DynamicGrowingBuffer<AABBTreeNode>*	mDynamicGrowingBuffer = nullptr;

	AABBTreeNode * getFreeAABBTreeNode()
	{
		AABBTreeNode * result = mDynamicGrowingBuffer->at(mCurrentFreeAABBTreeNode);
		++mCurrentFreeAABBTreeNode;
		return result;
	}

	// temporary construction buffer
	kstl::vector<unsigned int>*	mIndexlist1 = nullptr;
	kstl::vector<unsigned int>*	mIndexlist2 = nullptr;

	virtual bool CallLocalRayIntersection(Hit &hit, const Point3D& start, const Vector3D& dir)  const override;
	virtual bool CallLocalRayIntersection(std::vector<Hit> &hit, const Point3D& start, const Vector3D& dir)  const override;
	virtual AABBTree* getAABBTree() override
	{
		return this;
	}
public:


	static AABBTree* BuildFromMesh(ModernMesh* mesh);

#ifdef KIGS_TOOLS
	// draw debug mInfo using GLSLDrawDebug
	virtual void DrawDebug(const Point3D& pos, const  Matrix3x4* mat, Timer *timer);
#endif
	friend class AABBTreeNode;

	void LoadFromFile(const std::string& filename);

	template<typename PacketStream>
	bool Serialize(PacketStream& stream);

	void BuildFromTriangleList(Point3D* vertexList, int vertexCount, void* indexList, int indexCount, int leafSize = 16, bool force_u32 = false);

	AABBTree() {};
	AABBTree(int trCount);
	virtual ~AABBTree();

	const Point3D* GetVertexList() const { return mVertexList; }

	unsigned int GetVertexCount() const { return mVertexCount; }

	kstl::vector<unsigned int>&	GetIndexlist1()
	{
		return *mIndexlist1;
	}
	kstl::vector<unsigned int>&	GetIndexlist2()
	{
		return *mIndexlist2;
	}
};


#include "BinarySerializer.h"

template<typename PacketStream>
bool AABBTreeNode::SerializeNode(PacketStream& stream, AABBTree* root_node)
{
	CHECK_SERIALIZE(serialize_object(stream, mBBox.m_Min));
	CHECK_SERIALIZE(serialize_object(stream, mBBox.m_Max));
	CHECK_SERIALIZE(serialize_object(stream, mTriangleCount));
	//printf("%d", TriangleCount);
	bool has_sons = mSon1;
	CHECK_SERIALIZE(serialize_object(stream, has_sons));

	if (!PacketStream::IsWriting)
	{
		if (has_sons)
		{
			mSon1 = root_node->getFreeAABBTreeNode();
			mSon2 = root_node->getFreeAABBTreeNode();
		}
		else
		{
			mTriangleArray2 = root_node->getFreeNormalTriangleBuffer(mTriangleCount);
		}
	}

	//printf("%d", Son1 ? 0 : 1);
	if (has_sons)
	{
		CHECK_SERIALIZE(mSon1->SerializeNode(stream, root_node));
		CHECK_SERIALIZE(mSon2->SerializeNode(stream, root_node));
	}
	else
	{
		//printf("-%d|", int(mTriangleArray2 - root_node->mNormalTriangleArray));
	}
	return true;
}

template<typename PacketStream>
bool AABBTree::Serialize(PacketStream& stream)
{
	CHECK_SERIALIZE(serialize_object(stream, mVertexCount));
	if (!PacketStream::IsWriting)
	{
		mVertexList = new v3f[mVertexCount];
	}
	for (int i = 0; i < mVertexCount; ++i)
	{
		CHECK_SERIALIZE(serialize_object(stream, mVertexList[i]));
	}

	CHECK_SERIALIZE(serialize_object(stream, mCurrentFreeTriangleIndex));
	auto trcount = mCurrentFreeTriangleIndex;

	CHECK_SERIALIZE(serialize_object(stream, mCurrentFreeAABBTreeNode));
	auto aabbtreenodecount = mCurrentFreeAABBTreeNode;

	if (!PacketStream::IsWriting)
	{
		mNormalTriangleArray = new NormalTriangle[mCurrentFreeTriangleIndex];
		mDynamicGrowingBuffer = new DynamicGrowingBuffer<AABBTreeNode>(aabbtreenodecount);
		mCurrentFreeTriangleIndex = 0;
		mCurrentFreeAABBTreeNode = 0;
	}

	for (int i = 0; i < trcount; ++i)
	{
		auto& tri = mNormalTriangleArray[i];
		CHECK_SERIALIZE(serialize_bytes(stream, tri.data, 3*4));
	}

	CHECK_SERIALIZE(SerializeNode(stream, this));
	return true;
}



#endif