#ifndef _AABBTREE_H_
#define _AABBTREE_H_

#include "CoreModifiable.h"
#include "TecLibs/Tec3D.h"
#include "TecLibs/3D/3DObject/BBox.h"
#include "Node3D.h"
#include "CollisionBaseObject.h"

#include "DynamicGrowingBuffer.h"

class ModernMesh;

// ****************************************
// * AABBTreeNode class
// * --------------------------------------
/*!  \class AABBTreeNode
     a node in a Axis Aligned Bounding box tree, used to optimise intersection calculation 
	 \ingroup Collision
*/
// ****************************************

class AABBTree;

class AABBTreeNode 
{
public:



	auto GetSon1() const { return Son1; }
	auto GetSon2() const { return Son2; }

	const BBox& GetBBox() const { return m_BBox; }

	auto GetTriangleCount() const { return TriangleCount; }
	auto GetTriangleArray() const { return TriangleArray2; }

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
		BBox m_BBox;
		Point3D m_Mid;

		virtual BuildTriangle_base* Next() = 0;
		virtual void SetIndex(void *iArray, int first) = 0;
		virtual void GetIndex(unsigned int &a, unsigned int &b, unsigned int &c) = 0;
		virtual void ComputeMidAndBBox(Point3D *vArray) = 0;
		virtual int GetStructSize() = 0;


		static int CompareX(const void *A, const void *B)
		{
			if (static_cast<const BuildTriangle_base*>(A)->m_Mid.x < static_cast<const BuildTriangle_base*>(B)->m_Mid.x)
				return -1;
			else	if (static_cast<const BuildTriangle_base*>(A)->m_Mid.x > static_cast<const BuildTriangle_base*>(B)->m_Mid.x)
				return 1;

			return 0;
		}

		static int CompareY(const void *A, const void *B)
		{
			if (static_cast<const BuildTriangle_base*>(A)->m_Mid.y < static_cast<const BuildTriangle_base*>(B)->m_Mid.y)
				return -1;
			else	if (static_cast<const BuildTriangle_base*>(A)->m_Mid.y > static_cast<const BuildTriangle_base*>(B)->m_Mid.y)
				return 1;

			return 0;
		}

		static int CompareZ(const void *A, const void *B)
		{
			if (static_cast<const BuildTriangle_base*>(A)->m_Mid.z < static_cast<const BuildTriangle_base*>(B)->m_Mid.z)
				return -1;
			else	if (static_cast<const BuildTriangle_base*>(A)->m_Mid.z > static_cast<const BuildTriangle_base*>(B)->m_Mid.z)
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
		
		t* index;
		u32 triangle_index=0;

		virtual BuildTriangle_base* Next() { return (this)+1; }

		virtual int GetStructSize() { return sizeof(BuildTriangle<t>); }

		virtual void SetIndex(void *iArray, int first)
		{
			index = &reinterpret_cast<t*>(iArray)[first];
			triangle_index = first/3;
		}

		virtual void GetIndex(unsigned int &a, unsigned int &b, unsigned int &c)
		{
			a = index[0];
			b = index[1];
			c = index[2];
		}

		/*! \brief construct BBOX and middle point of this triangle
		*/
		virtual void ComputeMidAndBBox(Point3D *vArray)
		{
			m_Mid = (vArray[index[0]] + vArray[index[1]] + vArray[index[2]]) / 3.0f;
			m_BBox.Init(vArray[index[0]]);
			m_BBox.Update(vArray[index[1]]);
			m_BBox.Update(vArray[index[2]]);
		}
	};



protected:
	template<typename t>
	void Build2(AABBTree* root,BuildTriangle<t>* TrArray, BuildTriangle<t>* TmpTrArray, unsigned int TrCount, Point3D* VertexArray,const int leafSize,int lastSortMode,BBox currentbbox);

	void RescursiveSearchMesh(Node3D *root, kstl::vector<ModernMesh*>	&list);
	
	/*! \brief triangle count in this BBox
	*/
	int TriangleCount = 0;

public:
	/*! \brief sons. AABBTree is a binary tree 
	*/
	AABBTreeNode *Son1 = nullptr;

	// if node is a leaf, then TriangleCount is > 0 and TriangleArray2 is defined, Son1 & Son2 are null
	// else TriangleCount = 0 and Son1 & Son2 are defined
	union
	{
		AABBTreeNode	* Son2 = nullptr;
		NormalTriangle * TriangleArray2;
	};

	/*! \brief BBox for this node 
	*/
	BBox m_BBox;

public:	
	/*! \brief constructor 
	*/
	AABBTreeNode(); 

	/*! \brief destructor 
	*/
	virtual ~AABBTreeNode();
	



#ifdef KIGS_TOOLS
	// used for BBox drawing
	bool m_Hit = false;
#endif
	friend class AABBTreeNode;
	friend class Collision;
};

// root abbtreenode contains buffers for all sons
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

	Point3D *		myVertexList = nullptr;
	unsigned int	myVertexCount = 0;

	DynamicGrowingBuffer<AABBTreeNode>*	myDynamicGrowingBuffer = nullptr;

	AABBTreeNode * getFreeAABBTreeNode()
	{
		AABBTreeNode * result = myDynamicGrowingBuffer->at(mCurrentFreeAABBTreeNode);
		++mCurrentFreeAABBTreeNode;
		return result;
	}

	// temporary construction buffer
	kstl::vector<unsigned int>*	myIndexlist1 = nullptr;
	kstl::vector<unsigned int>*	myIndexlist2 = nullptr;

	virtual bool CallLocalRayIntersection(Hit &hit, const Point3D& start, const Vector3D& dir)  const override;
	virtual bool CallLocalRayIntersection(std::vector<Hit> &hit, const Point3D& start, const Vector3D& dir)  const override;
	virtual AABBTree* getAABBTree() override
	{
		return this;
	}
public:


	static AABBTree* BuildFromMesh(ModernMesh* mesh);

#ifdef KIGS_TOOLS
	// draw debug info using GLSLDrawDebug
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

	const Point3D* GetVertexList() const { return myVertexList; }

	unsigned int GetVertexCount() const { return myVertexCount; }

	kstl::vector<unsigned int>&	GetIndexlist1()
	{
		return *myIndexlist1;
	}
	kstl::vector<unsigned int>&	GetIndexlist2()
	{
		return *myIndexlist2;
	}
};


#include "BinarySerializer.h"

template<typename PacketStream>
bool AABBTreeNode::SerializeNode(PacketStream& stream, AABBTree* root_node)
{
	CHECK_SERIALIZE(serialize_object(stream, m_BBox.m_Min));
	CHECK_SERIALIZE(serialize_object(stream, m_BBox.m_Max));
	CHECK_SERIALIZE(serialize_object(stream, TriangleCount));
	//printf("%d", TriangleCount);
	bool has_sons = Son1;
	CHECK_SERIALIZE(serialize_object(stream, has_sons));

	if (!PacketStream::IsWriting)
	{
		if (has_sons)
		{
			Son1 = root_node->getFreeAABBTreeNode();
			Son2 = root_node->getFreeAABBTreeNode();
		}
		else
		{
			TriangleArray2 = root_node->getFreeNormalTriangleBuffer(TriangleCount);
		}
	}

	//printf("%d", Son1 ? 0 : 1);
	if (has_sons)
	{
		CHECK_SERIALIZE(Son1->SerializeNode(stream, root_node));
		CHECK_SERIALIZE(Son2->SerializeNode(stream, root_node));
	}
	else
	{
		//printf("-%d|", int(TriangleArray2 - root_node->mNormalTriangleArray));
	}
	return true;
}

template<typename PacketStream>
bool AABBTree::Serialize(PacketStream& stream)
{
	CHECK_SERIALIZE(serialize_object(stream, myVertexCount));
	if (!PacketStream::IsWriting)
	{
		myVertexList = new v3f[myVertexCount];
	}
	for (int i = 0; i < myVertexCount; ++i)
	{
		CHECK_SERIALIZE(serialize_object(stream, myVertexList[i]));
	}

	CHECK_SERIALIZE(serialize_object(stream, mCurrentFreeTriangleIndex));
	auto trcount = mCurrentFreeTriangleIndex;

	CHECK_SERIALIZE(serialize_object(stream, mCurrentFreeAABBTreeNode));
	auto aabbtreenodecount = mCurrentFreeAABBTreeNode;

	if (!PacketStream::IsWriting)
	{
		mNormalTriangleArray = new NormalTriangle[mCurrentFreeTriangleIndex];
		myDynamicGrowingBuffer = new DynamicGrowingBuffer<AABBTreeNode>(aabbtreenodecount);
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