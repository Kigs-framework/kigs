#pragma once

#include "CoreSTL.h"
#include "TecLibs/Tec3D.h"
#include "TecLibs/3D/3DObject/BBox.h"
#include "OctreeBase.h"
#include "CoreModifiable.h"
#include "BinarySerializer.h"
#include <vector>
#include <map>

// prefix all struct / classes with MS (Mesh Simplification)
enum class MSCubeSide
{
	SIDE_MINUS_X = 1,
	SIDE_X = 2,
	SIDE_MINUS_Y = 4,
	SIDE_Y = 8,
	SIDE_MINUS_Z = 16,
	SIDE_Z = 32,
};

struct MSPlaneStruct
{
	v3f			mNormal;
	float		mDist;

	static v3f	mAxisNormal[3];
};

struct MSSurfaceStruct
{
	MSPlaneStruct	mPlane;
	float		mSurface;
	v3f			mInOctreePlaneP0;
};

struct MSTriangleVertex
{
	v3f		mVertex;
	u32		mSurfaceIndex;
};

class MSEdge
{
public:
	MSEdge(u32 v1, u32 v2)
	{
		v[0] = v1;
		v[1] = v2;
		t[0] = t[1] = -1;
	}

	u32 v[2]; // indexes on vertex
	u32 t[2]; // indexes on faces
};

class MSFace
{
public:
	MSFace()
	{
		edges[0] = edges[1] = edges[2] = edges[3] = -1;
	}

	// use highest bit to check "edge way"
	u32 edges[4]; // triangle or quad
};


inline bool operator ==(const MSTriangleVertex& first,const MSTriangleVertex& second)
{
	if (first.mSurfaceIndex != second.mSurfaceIndex)
		return false;

	return first.mVertex == second.mVertex;
}

class CollisionMeshSimplification;
class MeshSimplificationOctreeNode;
class BuildMeshFromEnveloppe;

class MSOctreeContent
{
public:

	MSOctreeContent() {}
	~MSOctreeContent() {
		if (mData)
			delete mData;
	}

	void	setContent(const v3f& p1,u32 surfaceIndex)
	{
		if (!mData)
		{
			mData = new ContentData;
		}
		mData->mVertices.push_back({ p1, surfaceIndex });
	}

	const std::vector<MSTriangleVertex>*	getContent() const
	{
		if (mData)
		{
			return &(mData->mVertices);
		}
		return nullptr;
	}

	static MSOctreeContent canCollapse(OctreeNodeBase** children, bool& doCollapse)
	{
		// collapse only if all nodes are empty
		MSOctreeContent returnval;
		doCollapse = false;

		return returnval;
	}

	bool	canOnlyBeSetOnLeaf() const
	{
		return true;
	}

	bool	isEmpty() const
	{
		if (mData)
		{
			return (mData->mVertices.size() == 0);
		}
		return true;
	}

	bool operator ==(const MSOctreeContent& other) const
	{
		if (other.isEmpty() && isEmpty())
			return true;

		if (other.isEmpty() || isEmpty())
			return false;

		if (other.getContent()->size() != getContent()->size())
			return false;

		if (other.getContent() == getContent())
			return true;


		return false;
	}

	friend class MeshSimplificationOctreeNode;
	friend class CollisionMeshSimplification;
	friend class BuildMeshFromEnveloppe;
protected:
	class ContentData
	{
	protected:
		
	public:

		std::vector<std::pair<u32, float>>	getPrincipalSurfacesForDirection(const nodeInfo& n, const v3f& dir,const std::vector<MSSurfaceStruct>& Surfaces);

		void	initEnvelopeData();

		~ContentData()
		{
			if (!mEnvelopeData)
				delete mEnvelopeData;

			mEnvelopeData = nullptr;
		}

		MeshSimplificationOctreeNode* getNeighbor(u32 pos)
		{
			if (!mEnvelopeData)
				return nullptr;
			for (auto& n : mEnvelopeData->mNeighbors)
			{
				if (n.first == pos)
				{
					return n.second;
				}
			}
			return nullptr;
		}

		std::pair<u32, u8>* getVertexForFreeFace(u8 freefaceMask)
		{
			if (!mEnvelopeData)
				return nullptr;
			for (auto& p : mEnvelopeData->mGoodIntersectionPoint)
			{
				if (freefaceMask & p.second)
				{
					return &p;
				}
			}
			return nullptr;
		}

		std::vector<MSTriangleVertex>					mVertices;
		u8	mEmptyNeighborsFlag = 0;
		u8	mFreeFaceCount = 0;
		u8  mOtherFlags = 0;
		struct additionnalEnvelopeData
		{
			std::vector<std::pair<u32, MeshSimplificationOctreeNode*>>	mNeighbors;
			// pair.first = index in BuildMeshFromEnveloppe mVertices  and pair.second is freeface mask
			std::vector<std::pair<u32, u8>>								mGoodIntersectionPoint;
			std::map<v3f, std::set<u32>>								mPerVSurfaces;
			std::set<u32>												mAllSurfaces;
		};
		additionnalEnvelopeData* mEnvelopeData = nullptr;
	};

	ContentData* mData = nullptr;
private:

};

class MeshSimplificationOctree;

class MeshSimplificationOctreeNode : public OctreeNode<MSOctreeContent>
{
public:
	MeshSimplificationOctreeNode() : OctreeNode<MSOctreeContent>()
	{

	}

	MeshSimplificationOctreeNode(const v3f& p1, u32 surfaceIndex) : OctreeNode<MSOctreeContent>()
	{
		setContent(p1, surfaceIndex);
	}


	void	setContent(const v3f& p1, u32 surfaceIndex)
	{
		mContentType.setContent(p1, surfaceIndex);
	}



	const std::vector<MSTriangleVertex>* getVertices() const
	{
		return mContentType.getContent();
	}

	void checkEmptyNeighbors(MeshSimplificationOctree& octree, nodeInfo& node);

	u8	getFreeFaceCount()
	{
		if(mContentType.mData)
			return mContentType.mData->mFreeFaceCount;

		return 0;
	}

	u8	getFreeFaceFlag()
	{
		if (mContentType.mData)
			return mContentType.mData->mEmptyNeighborsFlag;

		return 0;
	}

	std::set<u32>	getSurfaceIndexes()
	{
		std::set<u32> result;
		const std::vector<MSTriangleVertex>* packed = mContentType.getContent();
		if (packed)
		{
			for (const auto& p : *packed)
			{
				result.insert(p.mSurfaceIndex);
			}
		}
		return result;
	}

	friend class MeshSimplificationOctree;

	// second parameter is precomputed sum(absf(surface normal)*0.5);   
	bool	intersectSurface(const v3f& minp, const float& r, float dist, const v3f& n)
	{
		v3f e(0.5f, 0.5f, 0.5f);
		v3f c = minp+e;

		float s = Dot(n, c) - dist;

		return fabsf(s) < r;
	}

protected:

	

private:

};


class MeshSimplificationOctree : public OctreeBase<CoreModifiable>
{
public:

	friend class CollisionMeshSimplification;

	DECLARE_CLASS_INFO(MeshSimplificationOctree, OctreeBase<CoreModifiable>, Core);
	DECLARE_CONSTRUCTOR(MeshSimplificationOctree);

	// set vertex list and transform it in octree coordinate
	void			setVertexList(const std::vector<v3f>& vlist);

	void			setVoxelContent(u32 P1, u32 P2, u32 P3, u32 surfaceIndex);

	inline void		TransformInOctreeCoord(v3f& p)
	{
		p -= mBBox.m_Min;
		p *= mBBoxCoef;
		p += mTranslate;
	}

	BBox			getBoundingBox()
	{
		BBox	result;
		result.m_Min = mTranslate;
		result.m_Max = mBBox.m_Max- mBBox.m_Min;
		result.m_Max *= mBBoxCoef;
		result.m_Max += mTranslate;
		return result;
	}

	void			setBBox(const BBox& bbox)
	{

		v3f objectShift = mTranslate;
		objectShift *= mPrecision;

		BBox bboxTakingAccountOfTranslation(bbox);
		bboxTakingAccountOfTranslation.m_Max += objectShift;

		// compute cubic bounding box
		// so take the bigger edge

		v3f edges(bboxTakingAccountOfTranslation.m_Max);
		edges -= bboxTakingAccountOfTranslation.m_Min;

		float maxedge = edges.x;
		if (maxedge < edges.y)
			maxedge = edges.y;
		if (maxedge < edges.z)
			maxedge = edges.z;
		
		// add 2*mPrecision to have an empty border around bbox (1 on each side)
		maxedge += 2.0f * mPrecision;

		// now search corresponding power of two

		float subdivision = maxedge / mPrecision;
		int poweroftwodecal = 0;
		while ((1 << poweroftwodecal) < subdivision)
		{
			poweroftwodecal++;
		}

		setValue("MaxDepth", poweroftwodecal);

		maxedge = (float)(1 << poweroftwodecal);
		maxedge *= mPrecision;
		maxedge *= 0.5f;

		mBBoxCoef = 1.0f / mPrecision;

		v3f halfdiag(maxedge, maxedge, maxedge);

		v3f bboxCenter(bbox.m_Max+ bbox.m_Min);
		bboxCenter *= 0.5f;

		mBBox.m_Min = mBBox.m_Max = bboxCenter;
		mBBox.m_Min -= halfdiag;
		mBBox.m_Max += halfdiag;

	}

	void	setSurfaceList(const std::vector<MSSurfaceStruct>& surfs)
	{
		mAllSurfacesPtr = &surfs;
	}

	std::vector<nodeInfo>	floodFillEmpty(u32 setBrowsingFlag);

	template<typename F>
	void	floodFillWithCondition(const nodeInfo& startPos, F&& condition, u32 setBrowsingFlag);

	const std::vector<v3f>& getInOctreeCoordsVertices()
	{
		return mInOctreeCoordsVertices;
	}
protected:

	friend class MeshSimplificationOctreeNode;

	MeshSimplificationOctreeNode* getFinalNodeAtPos(MeshSimplificationOctreeNode* currentNode, const v3i& pos, int currentDepth);

	// precision in meter (so 0.01f = 1cm)
	maFloat		mPrecision = BASE_ATTRIBUTE(Precision,0.01f);
	maVect3DF	mTranslate = BASE_ATTRIBUTE(Translate, 0.0f,0.0f,0.0f);
	const std::vector<MSSurfaceStruct>*	mAllSurfacesPtr;
	BBox	mBBox;
	float	mBBoxCoef;


	std::vector<v3f>	mInOctreeCoordsVertices;
};

#define STEP_BY_STEP_PROCESS

class CollisionMeshSimplification
{
protected:
	
#ifdef STEP_BY_STEP_PROCESS
	u32					mCurrentStep=0;
#endif


	SP<MeshSimplificationOctree> mOctree;

	static void		segmentPlaneIntersection(const v3f& P1, const v3f& P2, const MSPlaneStruct& p, std::set<v3f>& outSegTips);
	static float	distFromPlane(const MSPlaneStruct& p, const v3f& P1);

	std::vector<MSSurfaceStruct>	mAllSurfaces;
	std::vector<u32>			mTriangleSurfaceIndex;
	float						mAverageSurface;
	float						mCellSurface;
	float						mMaxSurface;
	float						mNormalisedSurfaceCoef; // 1/max surface
	u32							mGroupCount;

	v3f							mOctreeShift;
	v3f							mObjectShift;

	// return surface index in list
	u32			insertSurface(const MSSurfaceStruct& toadd);

	void		computeSurfaceList(const std::vector<u32>& indices, const std::vector<v3f>& vertices);

	struct enveloppeMesh
	{
		std::vector<v3f>					vertices;
		std::vector<std::pair<v3f, v3f>>	edges;
		std::vector<u32>	indices;
	};

	std::vector< enveloppeMesh>	mMeshes;
	std::vector<nodeInfo>		mEnvelopenodelist;

	void	rebuildMesh(u32 groupIndex,std::vector<nodeInfo>& envelopenodes);

	void	initOctree(const std::vector<u32>& indices, const std::vector<v3f>& vertices, float precision);

	void	StepByStepProcess(bool stepbystep);


public:

#ifdef STEP_BY_STEP_PROCESS


	const std::vector<v3f>& getOctreeCoordVertices()
	{
		return mOctree->getInOctreeCoordsVertices();
	}

	size_t						getEnveloppeSize()
	{
		return mEnvelopenodelist.size();
	}

	v3i	getEnveloppePos(size_t index, u32& flag)
	{
		flag = mEnvelopenodelist[index].getNode<MeshSimplificationOctreeNode>()->getContentType().mData->mEmptyNeighborsFlag;
		return mEnvelopenodelist[index].coord;
	}

#ifdef _DEBUG
	u32 getNodeDebugFlag(size_t index)
	{
		return 	mEnvelopenodelist[index].mDebugFlag;
	}
#endif

	std::vector<std::pair<v3f, v3f>>	getEdges() const;
	std::vector<v3f>					getEnveloppeVertices() const;
#endif

	BBox	getOctreeBoundingBox()
	{
		return mOctree->getBoundingBox();
	}

	static void		trianglePlaneIntersection(const v3f& triA, const v3f& triB, const v3f& triC, const MSPlaneStruct& p, std::set<v3f>& outSegTips);

	CollisionMeshSimplification(const std::vector<u32>&	indices,const std::vector<v3f>& vertices, float precision,bool stepbystep=false);
	~CollisionMeshSimplification();
};


// TODO : use more files to write the code
class BuildMeshFromEnveloppe
{
protected:

	class MSVertice
	{
	public:
		MSVertice(const v3f& v, nodeInfo d) : mV(v), mOctreeNode(d)
		{
			mEdges.clear();
		}

		// return edge index in edge list going to the given vector
		u32	getEdgeIndexInThisList(u32 v,const std::vector<MSEdge>& _edges) const
		{
			u32 vindex = 0;
			for (auto e : mEdges)
			{
				if ((_edges[e].v[0] == v) || (_edges[e].v[1] == v))
				{
					return vindex;
				}
				vindex++;
			}
			return -1;
		}

		v3f	mV;
		v3f mN;
		nodeInfo			mOctreeNode;
		std::vector<u32>	mEdges;
	};

	// add given vertice and return index
	unsigned int addVertice(const v3f& v, nodeInfo d)
	{
		mVertices.push_back({v,d});
		return (unsigned int)(mVertices.size() - 1);
	}

	u32	addEdge(u32 v1, u32 v2)
	{
		const std::vector< u32>& searchExisting = mEdgeMap[v1 ^ v2];

		if (searchExisting.size())
		{
			for (const auto& i : searchExisting)
			{
				if (((mEdges[i].v[0] == v1) && (mEdges[i].v[1] == v2)) || ((mEdges[i].v[1] == v1) && (mEdges[i].v[0] == v2)))
				{
					return i;
				}
			}
		}

		mEdges.push_back({ v1,v2 });
		mEdgeMap[v1 ^ v2].push_back((u32)(mEdges.size() - 1));
		return (u32)(mEdges.size() - 1);
	}

	// key = v1^v2 , value = list of index in  mEdges
	std::map < u32, std::vector< u32>>	mEdgeMap;

	std::vector<MSVertice>		mVertices;
	std::vector<MSFace>			mFaces;
	std::vector<MSEdge>			mEdges;
	std::vector<nodeInfo*>&		mNodeList;
	const std::vector<MSSurfaceStruct>&	mAllSurfaces;
	float mCellSurface;

	void	setUpVertices(nodeInfo& node);
	void	setUpEdges(nodeInfo node);
	void	setUpNormals();
	void	setUpFaces();
	void	splitFaces();
	void	mergeTriangles();

	bool	checkVertice(nodeInfo& node, v3f& goodOne)
	{
		if (checkVerticeTrivialCase(node, goodOne))
			return true;

		if (checkVerticeEasyCase(node, goodOne))
			return true;

#ifdef _DEBUG
		node.mDebugFlag = 1;
#endif

		return false;
	}

	bool	checkVerticeTrivialCase(nodeInfo node, v3f& goodOne);
	bool	checkVerticeEasyCase(nodeInfo node, v3f& goodOne);



	// all cases
	void manageDirectionalFreeFace(const MSOctreeContent& node,const nodeInfo& n);
	void manageSeparateFreeFaces(const MSOctreeContent& node, const nodeInfo& n);
	void setUpEdgesInFourFreeFaceNode(const MSOctreeContent& node, std::map<std::pair<u32, u8>, std::set<u32>>& foundEdges);

	u32 getFaceIndex(u32 facemask)
	{
		for (u32 i = 0; i < 6; i++)
		{
			if (facemask & (1 << i))
				return i;
		}
		return 0;
	}

	v3f searchGoodPointInCellForDirection(const MSOctreeContent& node, const v3f& direction);

	std::pair<v3f,v3f> searchGoodPlaneInCellForDirection(const MSOctreeContent& node, const v3f& direction);

public:

	BuildMeshFromEnveloppe(std::vector<nodeInfo*>& nodes, const std::vector<MSSurfaceStruct>& surfaces, float cellsurface) : mNodeList(nodes), mAllSurfaces(surfaces), mCellSurface(cellsurface)
	{}
	void Build();

	std::vector<std::pair<v3f, v3f>>	getEdges() const;
	std::vector<v3f>					getEnveloppeVertices() const;
};