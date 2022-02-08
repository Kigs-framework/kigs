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
	mSon1 = NULL;
	mSon2 = NULL;
	mTriangleCount = 0;
	
}

AABBTreeNode::~AABBTreeNode()
{
	//! delete son nodes 
	/*if (Son1)
	{
		delete Son1;
		delete mSon2;
	}*/
}

#ifdef KIGS_TOOLS
#include <GLSLDebugDraw.h>
void AABBTree::DrawDebug(const Hit& h, const Matrix3x4& mat)
{
	if (!mHit)
		return;
	mHit = false;
	Point3D p[8];
	mBBox.ConvertToPoint(p);
	mat.TransformPoints(p, 8);
	if (mTriangleCount)
		dd::box(p, Point3D(255, 0, 0));
	else
		dd::box(p, mDebugColor);
	/*if (Son1)
	{
		Son1->DrawDebug(pos, mat, timer);
		mSon2->DrawDebug(pos, mat, timer);
	}*/
}
#endif

void AABBTree::BuildFromTriangleList(Point3D* vertexList, int vertexCount, void* indexList, int triangleCount, int leafSize, bool force_u32)
{
	mVertexList = vertexList;
	mVertexCount = vertexCount;
	mTriangleCount = triangleCount;

	kstl::vector<unsigned int>	indexlist1;
	kstl::vector<unsigned int>	indexlist2;
	indexlist1.reserve(triangleCount);
	indexlist2.reserve(triangleCount);

	mIndexlist1=&indexlist1;
	mIndexlist2=&indexlist2;

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

		mBBox.Init(TrArray[0].mBBox);
		for (int i = 1; i < triangleCount; i++)
			mBBox.Update(TrArray[i].mBBox);

		//median /= triangleCount;
		//kigsprintf("create AABBTree (%p) whith %d triangles\n",this, triangleCount);

		Build2<unsigned short>(this,TrArray, tmpTrArray, mTriangleCount, mVertexList, leafSize,-1, mBBox);

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

		mBBox.Init(TrArray[0].mBBox);
		for (int i = 1; i < triangleCount; i++)
			mBBox.Update(TrArray[i].mBBox);

		Build2<unsigned int>(this, TrArray, tmpTrArray, mTriangleCount, mVertexList, leafSize, -1, mBBox);
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
	/*mBBox.Init(TrArray[0].mBBox);
	for (i = 1; i < TrCount; i++)
		mBBox.Update(TrArray[i].mBBox);*/

	mBBox = currentbbox;

	//! Compute bounding box diagonal
	BoxLen = mBBox.m_Max - mBBox.m_Min;

	//! if Triangle count is small then this is a leaf
	if (TrCount < leafSize)
	{
		mTriangleArray2 = ((AABBTree*)root)->getFreeNormalTriangleBuffer(TrCount);
		for (i = 0; i < TrCount; i++)
		{
			u32 a, b, c;
			TrArray[i].GetIndex(a, b, c);
			mTriangleArray2[i].SetA(a);
			mTriangleArray2[i].SetB(b);
			mTriangleArray2[i].SetC(c);
			mTriangleArray2[i].SetFaceIndex(TrArray[i].mFaceIndex);
			const Point3D * P1 = &vertex_list[mTriangleArray2[i].A()];
			const Point3D * P2 = &vertex_list[mTriangleArray2[i].B()];
			const Point3D * P3 = &vertex_list[mTriangleArray2[i].C()];
			/*mTriangleArray2[i].m_normal.CrossProduct(Vector3D(*P1, *P2, asVector{}), Vector3D(*P1, *P3, asVector{}));
			mTriangleArray2[i].m_normal.Normalize();*/
		}
		mTriangleCount = TrCount;
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
	bbox1.Init(TrArray[0].mBBox);
	BoxLen = bbox1.m_Max - bbox1.m_Min;
	kfloat size1 = NormSquare(BoxLen);
	BBox	bbox2;
	bbox2.Init(TrArray[TrCount - 1].mBBox);
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
		compare1.Update(TrArray[tindex].mBBox);
		BoxLen = compare1.m_Max - compare1.m_Min;
		kfloat newSize1 = NormSquare(BoxLen);
		kfloat ds1 = (newSize1 - size1);

		BBox compare2(bbox2);
		compare2.Update(TrArray[tindex].mBBox);
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
		mSon1 = root->getFreeAABBTreeNode();

		// compute bbox1 & bbox2
		bbox1.Init(TrArray[0].mBBox);
		for (i = 1; i < halfTrCount; i++)
			bbox1.Update(TrArray[i].mBBox);
		bbox2.Init(TrArray[halfTrCount].mBBox);
		for (i = halfTrCount+1; i < TrCount; i++)
			bbox2.Update(TrArray[i].mBBox);
		
		mSon1->Build2(root,TrArray, TmpTrArray,halfTrCount, VertexArray,leafSize, mode,bbox1);
		mSon2 = root->getFreeAABBTreeNode();
		
		mSon2->Build2(root,&TrArray[halfTrCount], TmpTrArray, TrCount - halfTrCount, VertexArray, leafSize, mode,bbox2);
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

		mSon1 = root->getFreeAABBTreeNode();
		
		mSon1->Build2(root,TrArray, TmpTrArray, fisrtarraysize + 1, VertexArray, leafSize,mode,bbox1);
		mSon2 = root->getFreeAABBTreeNode();
		
		mSon2->Build2(root,&TrArray[fisrtarraysize + 1], TmpTrArray, TrCount - (fisrtarraysize + 1), VertexArray, leafSize,mode,bbox2);

	}
}

void AABBTreeNode::RescursiveSearchMesh(Node3D *root, kstl::vector<ModernMesh*>	&list)
{
	for (auto item : root->getItems())
	{
		if (item.mItem->isSubType("ModernMesh"))
			list.push_back(item.mItem->as<ModernMesh>());
		else if(item.mItem->isSubType("Node3D"))
			RescursiveSearchMesh(item.mItem->as<Node3D>(), list);
	}
};


AABBTree::AABBTree(int trCount) : SimpleShapeBase(), AABBTreeNode()
{
	InitDynamicBuffer(trCount);
}

void AABBTree::InitDynamicBuffer(int trCount)
{
	if (mDynamicGrowingBuffer) return;

	mNormalTriangleArray = new NormalTriangle[trCount];
	mCurrentFreeTriangleIndex = 0;

	if (trCount > 16)
	{
		mDynamicGrowingBuffer = new DynamicGrowingBuffer<AABBTreeNode>(trCount / 8, trCount / 16);
	}
	else
	{
		mDynamicGrowingBuffer = new DynamicGrowingBuffer<AABBTreeNode>(trCount);
	}
}

bool AABBTree::CallLocalRayIntersection(Hit &hit, const Point3D& start, const Vector3D& dir) const
{
	kfloat u, v;
	bool found = Intersection::IntersectionRayAABBTree(GetVertexList(), start, dir, *this, hit.HitDistance, hit.HitNormal, u, v, hit.HitTriangleVertexIndices);
	if (found && mFaceCount != 0)
		hit.HitFaceIndex = mFaceIndex;
	return found;
}

bool AABBTree::CallLocalRayIntersection(std::vector<Hit> &hit, const Point3D& start, const Vector3D& dir) const
{
	int count_before = hit.size();
	if (Intersection::IntersectionRayAABBTree(GetVertexList(), start, dir, *this, hit))
	{
		if (mFaceCount != 0)
		{
			for (int i = count_before; i < hit.size(); ++i)
			{
				hit[i].HitFaceIndex = mFaceIndex;
			}
		}
		return true;
	}
	return false;
}

AABBTree::~AABBTree()
{
	delete[] mNormalTriangleArray;

	// destroy vertexlist if root
	delete[] mVertexList;
	
	delete mDynamicGrowingBuffer;

}

void AABBTree::LoadFromFile(const std::string& filename)
{
	mFileName = filename;
	u64 len;

	auto crb = ModuleFileManager::Get()->LoadFile(filename.c_str(), len);
	auto result = crb;

	if (crb && filename.substr(filename.find_last_of('.')) == ".kaabb")
	{
		CMSP uncompressManager = KigsCore::GetSingleton("KXMLManager");
		if (uncompressManager)
		{
			result = MakeRefCounted<CoreRawBuffer>();
			uncompressManager->SimpleCall("UncompressData", crb.get(), result.get());
		}
	}

	if (result)
	{
		PacketReadStream stream_reader{ (u32*)result->data(), result->size() };
		serialize_object(stream_reader, *this);
	}
}


CoreItemSP AABBTree::getCoreItemDesc() const
{
	CoreItemSP map = getBaseCoreItemDesc();
	map->set("FaceCount", mFaceCount);
	if (mTriangleCount)
	{
		if(mFileName.size())
			map->set("Filename", mFileName);
		else
			map->set("Buffer", SaveToString(*const_cast<AABBTree*>(this)));
	}
	return map;
}

void AABBTree::initFromCoreItemDesc(CoreItemSP init)
{
	mFaceCount = init["FaceCount"];

	auto filename = init["Filename"];
	if (filename)
	{
		LoadFromFile(filename);
		return;
	}

	auto buffer = init["Buffer"];
	if (buffer)
	{
		LoadFromString(*this, buffer);
		return;
	}
}