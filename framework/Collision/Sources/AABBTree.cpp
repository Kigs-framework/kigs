#include "PrecompiledHeaders.h"
#include "AABBTree.h"
#include "ModernMesh.h"
#include "TecLibs\3D\3DObject\BBox.h"

#include "ModuleFileManager.h"
#include "AttributePacking.h"
#include "Intersection.h"

#include <stdlib.h>

AABBTreeNode::AABBTreeNode() 
{
	//! call base class constructor (CollisionBaseObject) and init members
	Son1 = NULL;
	Son2 = NULL;
	TriangleCount = 0;
	
}

AABBTreeNode::~AABBTreeNode()
{
	//! delete son nodes 
	/*if (Son1)
	{
		delete Son1;
		delete Son2;
	}*/
}

#ifdef KIGS_TOOLS
#include <GLSLDebugDraw.h>
void AABBTree::DrawDebug(const Point3D& pos, const  Matrix3x4* mat, Timer *timer)
{
	if (!m_Hit)
		return;

	m_Hit = false;

	Point3D p[8];
	p[0].Set(m_BBox.m_Max.x, m_BBox.m_Min.y, m_BBox.m_Min.z);
	p[1].Set(m_BBox.m_Max.x, m_BBox.m_Max.y, m_BBox.m_Min.z);
	p[2].Set(m_BBox.m_Min.x, m_BBox.m_Max.y, m_BBox.m_Min.z);
	p[3].Set(m_BBox.m_Min.x, m_BBox.m_Min.y, m_BBox.m_Min.z);

	p[4].Set(m_BBox.m_Max.x, m_BBox.m_Min.y, m_BBox.m_Max.z);
	p[5].Set(m_BBox.m_Max.x, m_BBox.m_Max.y, m_BBox.m_Max.z);
	p[6].Set(m_BBox.m_Min.x, m_BBox.m_Max.y, m_BBox.m_Max.z);
	p[7].Set(m_BBox.m_Min.x, m_BBox.m_Min.y, m_BBox.m_Max.z);



	mat->TransformPoints(p, 8);
	if (TriangleCount)
		dd::box(p, Point3D(255, 0, 0));
	else
		dd::box(p, debugColor);


	/*if (Son1)
	{
		Son1->DrawDebug(pos, mat, timer);
		Son2->DrawDebug(pos, mat, timer);
	}*/
}
#endif

void AABBTree::BuildFromTriangleList(Point3D* vertexList, int vertexCount, void* indexList, int triangleCount, int leafSize, bool force_u32)
{
	myVertexList = vertexList;
	myVertexCount = vertexCount;
	TriangleCount = triangleCount;

	kstl::vector<unsigned int>	indexlist1;
	kstl::vector<unsigned int>	indexlist2;
	indexlist1.reserve(triangleCount);
	indexlist2.reserve(triangleCount);

	myIndexlist1=&indexlist1;
	myIndexlist2=&indexlist2;

	//Point3D median(0,0,0);
	//! then construct temporary triangle array
	if (vertexCount <= USHRT_MAX && !force_u32)
	{
		BuildTriangle<unsigned short> *TrArray = new BuildTriangle<unsigned short>[triangleCount];
		BuildTriangle<unsigned short> *tmpTrArray = new BuildTriangle<unsigned short>[triangleCount];
		for (int i = 0; i < triangleCount; i++)
		{
			TrArray[i].SetIndex(indexList, i*3);
			TrArray[i].ComputeMidAndBBox(vertexList);
			//median += TrArray[i].m_Mid;
		}

		static_assert(sizeof(NormalTriangle) == 3 * sizeof(unsigned int));

		m_BBox.Init(TrArray[0].m_BBox);
		for (int i = 1; i < triangleCount; i++)
			m_BBox.Update(TrArray[i].m_BBox);

		//median /= triangleCount;
		//kigsprintf("create AABBTree (%p) whith %d triangles\n",this, triangleCount);

		Build2<unsigned short>(this,TrArray, tmpTrArray, TriangleCount, myVertexList, leafSize,-1, m_BBox);

		delete[] tmpTrArray;

		delete[] TrArray;
	}
	else
	{
		BuildTriangle<unsigned int> *TrArray = new BuildTriangle<unsigned int>[triangleCount];
		BuildTriangle<unsigned int> *tmpTrArray = new BuildTriangle<unsigned int>[triangleCount];
		for (int i = 0; i < triangleCount; i++)
		{
			TrArray[i].SetIndex(indexList, i*3);
			TrArray[i].ComputeMidAndBBox(vertexList);
		}

		m_BBox.Init(TrArray[0].m_BBox);
		for (int i = 1; i < triangleCount; i++)
			m_BBox.Update(TrArray[i].m_BBox);

		Build2<unsigned int>(this, TrArray, tmpTrArray, TriangleCount, myVertexList, leafSize, -1, m_BBox);
		delete[] tmpTrArray;
		delete[] TrArray;
	}
}

AABBTree* AABBTree::BuildFromMesh(ModernMesh* mesh)
{
	int VCount;
	float* VArray = nullptr;
	int ICount;
	void* IArray = nullptr;

	mesh->GetTriangles(VCount, VArray, ICount, IArray);
	mesh->SetCanFree();

	if (VCount == 0)
	{
		delete VArray;
		free(IArray);
		return nullptr;
	}

	AABBTree * node = new AABBTree(ICount / 3);
	node->BuildFromTriangleList((Point3D*)VArray, VCount, IArray, ICount / 3);
	free(IArray);

	return node;
}

template<typename t>
void AABBTreeNode::Build2(AABBTree* root, AABBTreeNode::BuildTriangle<t> * TrArray, BuildTriangle<t>* TmpTrArray, unsigned int TrCount, Point3D* VertexArray,const int leafSize, int lastSortMode, BBox currentbbox)
{
	unsigned int i;
	Point3D BoxLen;
	int mode;

	const Point3D* vertex_list = root->GetVertexList();

	//! first compute triangle array BoundingBox
	/*m_BBox.Init(TrArray[0].m_BBox);
	for (i = 1; i < TrCount; i++)
		m_BBox.Update(TrArray[i].m_BBox);*/

	m_BBox = currentbbox;

	//! Compute bounding box diagonal
	BoxLen = m_BBox.m_Max - m_BBox.m_Min;

	//! if Triangle count is small then this is a leaf
	if (TrCount < leafSize)
	{
		TriangleArray2 = ((AABBTree*)root)->getFreeNormalTriangleBuffer(TrCount);
		for (i = 0; i < TrCount; i++)
		{
			u32 a, b, c;
			TrArray[i].GetIndex(a, b, c);
			TriangleArray2[i].SetA(a);
			TriangleArray2[i].SetB(b);
			TriangleArray2[i].SetC(c);
			TriangleArray2[i].SetTriangleIndex(TrArray[i].triangle_index);
			const Point3D * P1 = &vertex_list[TriangleArray2[i].A()];
			const Point3D * P2 = &vertex_list[TriangleArray2[i].B()];
			const Point3D * P3 = &vertex_list[TriangleArray2[i].C()];
			/*TriangleArray2[i].m_normal.CrossProduct(Vector3D(*P1, *P2, asVector{}), Vector3D(*P1, *P3, asVector{}));
			TriangleArray2[i].m_normal.Normalize();*/
		}
		TriangleCount = TrCount;
		return;
	}

	//! else find the longest bbox length on X, Y or Z
	if (BoxLen.x > BoxLen.y)
	{
		if (BoxLen.x > BoxLen.z)
			// X axis is longest
			mode = 0;
		else
			if (BoxLen.y > BoxLen.z)
				// Y axis is longest
				mode = 1;
			else
				// Z axis is longest
				mode = 2;
	}
	else if (BoxLen.y > BoxLen.z)
	{
		// Y axis is longest
		mode = 1;
	}
	else
	{
		// Z axis is longest
		mode = 2;
	}

	// don't need to sort if already sorted on the same axis
	if (lastSortMode != mode)
	{
		//! and sort array according to the longest axis
		switch (mode)
		{
		case 0:
			qsort(TrArray, TrCount, sizeof(BuildTriangle<t>), BuildTriangle_base::CompareX);
			break;
		case 1:
			qsort(TrArray, TrCount, sizeof(BuildTriangle<t>), BuildTriangle_base::CompareY);
			break;
		case 2:
			qsort(TrArray, TrCount, sizeof(BuildTriangle<t>), BuildTriangle_base::CompareZ);
			break;
		}
	}

	/*! now that we have a sorted array of triangles, we take the bboxes of first and last triangle
	in the array, and try to set each triangle in the bbox where it fits better.
	Triangle are taken in this order :
	- 1, n, 2, n-1, 3, n-2 ...
	Once each triangle has been affected to the first or last bbox, the bbox is updated
	*/
	BBox	bbox1;
	bbox1.Init(TrArray[0].m_BBox);
	BoxLen = bbox1.m_Max - bbox1.m_Min;
	kfloat size1 = NormSquare(BoxLen);
	BBox	bbox2;
	bbox2.Init(TrArray[TrCount - 1].m_BBox);
	BoxLen = bbox2.m_Max - bbox2.m_Min;
	kfloat size2 = NormSquare(BoxLen);

	kstl::vector<unsigned int>&	indexlist1 = root->GetIndexlist1();
	indexlist1.clear();
	kstl::vector<unsigned int>&	indexlist2 = root->GetIndexlist2();
	indexlist2.clear();
	unsigned int titerator;
	for (titerator = 1; titerator < (TrCount - 1); titerator++)
	{
		int tindex;
		if (titerator & 1)
		{
			tindex = TrCount - ((titerator / 2) + 2);
		}
		else
		{
			tindex = (titerator / 2);
		}


		BBox compare1(bbox1);
		compare1.Update(TrArray[tindex].m_BBox);
		BoxLen = compare1.m_Max - compare1.m_Min;
		kfloat newSize1 = NormSquare(BoxLen);
		kfloat ds1 = (newSize1 - size1);

		BBox compare2(bbox2);
		compare2.Update(TrArray[tindex].m_BBox);
		BoxLen = compare2.m_Max - compare2.m_Min;
		kfloat newSize2 = NormSquare(BoxLen);
		kfloat ds2 = (newSize2 - size2);

		if (ds1 < ds2)
		{
			bbox1=compare1;
			size1 = newSize1;
			indexlist1.push_back(tindex);
		}
		else if (ds1 > ds2)
		{
			bbox2=compare2;
			size2 = newSize2;
			indexlist2.push_back(tindex);
		}
		else
		{
			if (size1 < size2)
			{
				bbox1 = compare1;
				size1 = newSize1;
				indexlist1.push_back(tindex);
			}
			else
			{
				bbox2 = compare2;
				size2 = newSize2;
				indexlist2.push_back(tindex);
			}
		}
	}

	/*! at the end, if one of the list is empty, just separate
	the triangle list in two
	*/
	int threshold = TrCount / 20;
	if ((indexlist1.size() <= threshold) || (indexlist2.size() <= threshold))
	{
		int halfTrCount = TrCount / 2;
		Son1 = root->getFreeAABBTreeNode();

		// compute bbox1 & bbox2
		bbox1.Init(TrArray[0].m_BBox);
		for (i = 1; i < halfTrCount; i++)
			bbox1.Update(TrArray[i].m_BBox);
		bbox2.Init(TrArray[halfTrCount].m_BBox);
		for (i = halfTrCount+1; i < TrCount; i++)
			bbox2.Update(TrArray[i].m_BBox);
		
		Son1->Build2(root,TrArray, TmpTrArray,halfTrCount, VertexArray,leafSize, mode,bbox1);
		Son2 = root->getFreeAABBTreeNode();
		
		Son2->Build2(root,&TrArray[halfTrCount], TmpTrArray, TrCount - halfTrCount, VertexArray, leafSize, mode,bbox2);
	}
	else
	{
		//! else use the list to construct the two sons recursively
		memcpy(TmpTrArray, TrArray, TrCount * sizeof(AABBTreeNode::BuildTriangle<t>));

		for (titerator = 0; titerator < indexlist1.size(); titerator++)
		{
			TrArray[titerator + 1] = TmpTrArray[indexlist1[titerator]];
		}
		for (titerator = 0; titerator < indexlist2.size(); titerator++)
		{
			TrArray[titerator + indexlist1.size() + 1] = TmpTrArray[indexlist2[titerator]];
		}

		int fisrtarraysize = indexlist1.size();

		Son1 = root->getFreeAABBTreeNode();
		
		Son1->Build2(root,TrArray, TmpTrArray, fisrtarraysize + 1, VertexArray, leafSize,mode,bbox1);
		Son2 = root->getFreeAABBTreeNode();
		
		Son2->Build2(root,&TrArray[fisrtarraysize + 1], TmpTrArray, TrCount - (fisrtarraysize + 1), VertexArray, leafSize,mode,bbox2);

	}
}

void AABBTreeNode::RescursiveSearchMesh(Node3D *root, kstl::vector<ModernMesh*>	&list)
{
	for (auto item : root->getItems())
	{
		if (item.myItem->isSubType("ModernMesh"))
			list.push_back(item.myItem->as<ModernMesh>());
		else if(item.myItem->isSubType("Node3D"))
			RescursiveSearchMesh(item.myItem->as<Node3D>(), list);
	}
};


AABBTree::AABBTree(int trCount) : AABBTreeNode() , CollisionBaseObject()
{
	mNormalTriangleArray = new NormalTriangle[trCount];
	mCurrentFreeTriangleIndex = 0;

	if (trCount > 16)
	{
		myDynamicGrowingBuffer = new DynamicGrowingBuffer<AABBTreeNode>(trCount / 8, trCount / 16);
	}
	else
	{
		myDynamicGrowingBuffer = new DynamicGrowingBuffer<AABBTreeNode>(trCount);
	}
}

bool AABBTree::CallLocalRayIntersection(Hit &hit, const Point3D& start, const Vector3D& dir) const
{
	kfloat u, v;
	return Intersection::IntersectionRayAABBTree(GetVertexList(), start, dir, *this, hit.HitDistance, hit.HitNormal, u, v, hit.HitTriangleVertexIndices);
}
bool AABBTree::CallLocalRayIntersection(std::vector<Hit> &hit, const Point3D& start, const Vector3D& dir) const
{
	return Intersection::IntersectionRayAABBTree(GetVertexList(), start, dir, *this, hit);
}

AABBTree::~AABBTree()
{
	delete[] mNormalTriangleArray;

	// destroy vertexlist if root
	delete[] myVertexList;
	
	delete myDynamicGrowingBuffer;

}

void AABBTree::LoadFromFile(const std::string& filename)
{
	mFileName = filename;
	u64 len;

	auto crb = OwningRawPtrToSmartPtr(ModuleFileManager::Get()->LoadFile(filename.c_str(), len)); 
	auto result = crb;

	if (crb && filename.substr(filename.find_last_of('.')) == ".kaabb")
	{
		CMSP uncompressManager = KigsCore::GetSingleton("KXMLManager");
		if (uncompressManager)
		{
			result = OwningRawPtrToSmartPtr(new CoreRawBuffer);
			uncompressManager->SimpleCall("UncompressData", crb.get(), result.get());
		}
	}

	if (result)
	{
		PacketReadStream stream_reader{ (u32*)result->data(), result->size() };
		serialize_object(stream_reader, *this);
	}
}