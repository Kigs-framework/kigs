#pragma once

#include "CoreSTL.h"
#include "OctreeMeshSimplification.h"
#include "BuildMeshFromEnveloppe.h"
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

class MeshSimplification
{
protected:
	
	SP<MeshSimplificationOctree> mOctree;

	std::vector<MSSurfaceStruct>	mAllSurfaces;
	std::vector<u32>			mTriangleSurfaceIndex;
	
	u32							mGroupCount;

	v3f							mOctreeShift;
	v3f							mObjectShift;

	class surfaceListBuilder
	{
	protected:
		const std::vector<u32>& mIndices;
		const std::vector<v3f>& mVertices;
		float					mOneOnPrecision;
		MeshSimplification&		mParent;

		std::map<int, std::vector<u32>>	mConstructionMap;

		// return surface index in list
		u32			insertSurface(const MSSurfaceStruct& toadd);

		

	public:
		surfaceListBuilder(const std::vector<u32>& indices, const std::vector<v3f>& vertices, float	precision, MeshSimplification& parent) :
			mIndices(indices)
			, mVertices(vertices)
			, mOneOnPrecision(8.0f/precision)
			, mParent(parent)
		{

		}

		void	build();
	}; 

	
	void		computeSurfaceList(const std::vector<u32>& indices, const std::vector<v3f>& vertices, float precision);

#ifdef _DEBUG
	struct vAndN
	{
		v3f					V;
		std::vector<v3f>	N;
	};

	struct enveloppeMesh
	{
		std::vector<vAndN>					vertices;
		std::vector<std::pair<v3f, v3f>>	edges;
		std::vector<u32>	indices;
	};

	std::vector< enveloppeMesh>	mMeshes;
#endif
	std::vector<nodeInfo>		mEnvelopenodelist;


	void	rebuildMesh(u32 groupIndex,std::vector<nodeInfo>& envelopenodes);

	void	initOctree(const std::vector<u32>& indices, const std::vector<v3f>& vertices, float precision);

	// if precision gives a too deep octree, then change precision
	void	adjustPrecision(const BBox& bbox, float& precision);

	std::vector<u32>	mFinalIndices;
	std::vector<v3f>	mFinalVertices;

public:

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

#ifdef _DEBUG
	std::vector<std::pair<v3f, v3f>>	getEdges() const;
	std::vector<vAndN>					getEnveloppeVertices() const;
#endif

	u32									getTriangleCount()const
	{
		return mFinalIndices.size() / 3;
	}

	u32									getVerticeCount()const
	{
		return mFinalVertices.size();
	}

	const std::vector<u32>& getFinalTriangles()
	{
		return mFinalIndices;
	}
	const std::vector<v3f>& getFinalVertices()
	{
		return mFinalVertices;
	}


	BBox	getOctreeBoundingBox()
	{
		return mOctree->getBoundingBox();
	}


	MeshSimplification(const std::vector<u32>&	indices,const std::vector<v3f>& vertices, float precision);
	~MeshSimplification();
};

