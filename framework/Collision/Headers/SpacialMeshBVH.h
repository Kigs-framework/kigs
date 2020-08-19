#ifndef _SPACIALMESHBVH_H_
#define _SPACIALMESHBVH_H_

#include "CoreModifiable.h"
#include "TecLibs/Tec3D.h"
#include "TecLibs/3D/3DObject/BBox.h"
#include "TecLibs/2D/BBox2D.h"
#include "Node3D.h"
#include "CollisionBaseObject.h"

class ModernMesh;

// ****************************************
// * SpacialMeshBVHNode class
// * --------------------------------------
/**
* \file	SpacialMeshBVH.h
* \class	SpacialMeshBVHNode
* \ingroup Collision
* \brief  Bounding volume hierarchy node structure.
*/
// ****************************************

class SpacialMeshBVHNode
{
public:
	static float HalfPlaneSign(const v2f& p1, const  v2f& p2, const  v2f& p3)
	{
		return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
	}

	static bool PointInTriangle(const v2f& pt, const v2f& v1, const v2f& v2, const v2f& v3)
	{
		float d1, d2, d3;
		bool has_neg, has_pos;

		d1 = HalfPlaneSign(pt, v1, v2);
		d2 = HalfPlaneSign(pt, v2, v3);
		d3 = HalfPlaneSign(pt, v3, v1);

		has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
		has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

		return !(has_neg && has_pos);
	}
	SpacialMeshBVHNode() : mBBox3D(BBox::PreInit())
	{
		
	}

	unsigned int	getStableTriangleListSize() const
	{
		return mStableTriangles.size();
	}

	v3f	getStableTriangleCenter(int index) const
	{
		unsigned int t=mStableTriangles[index];

		v2f tcenter = mTriangles2D[3*t] + mTriangles2D[3*t + 1] + mTriangles2D[3*t + 2];
		tcenter *= 1.0f / 3.0f;

		v3f center = mO + mU * tcenter.x + mV * tcenter.y;
		return center;
	}

	const v3f& getNormal() const
	{
		return mNormal;
	}

	const BBox& getBBox() const
	{
		return mBBox3D;
	}

	float getSurface() const { return mSurface; }

protected:

	friend class SpacialMeshBVH;

	void add2DVertice(const v3f& pt)
	{
		mBBox3D.Update(pt);
		// project pt on u and v
		v3f	toProject(pt);
		toProject -= mO;

		v2f projected;
		projected.x = Dot(toProject, mU);
		projected.y = Dot(toProject, mV);
		mTriangles2D.push_back(projected);
	}

	v3f	getProjected(const v3f& pt)
	{
		v3f	toProject(pt);
		toProject -= mO;

		float p = Dot(toProject, mNormal);
		
		v3f result = pt - mNormal * p;
		return result;
	}

	void	computeBBox()
	{
		if (mTriangles2D.size()>1)
		{
			mBBox.Init(mTriangles2D[0]);
			mBBox.Update(&mTriangles2D[1], mTriangles2D.size() - 1);
		}
	}

	v3f		mNormal;	// normal + d = plane equation

	// 2D planar coordinate system
	v3f		mO;	// origin 
	v3f		mU;	// u
	v3f		mV;	// v

	std::vector<v2f>			mTriangles2D; // triangles
	std::vector<unsigned int>	mStableTriangles;
	BBox2D				mBBox;
	BBox				mBBox3D;
	float mSurface = 0.0f;

	bool	intersect(Hit& hit, const v3f& start, const v3f& dir) const;

public:
#ifdef KIGS_TOOLS
	mutable bool mHit = false;
#endif
};

// ****************************************
// * SpacialMeshBVH class
// * --------------------------------------
/**
* \file	SpacialMeshBVH.h
* \class	SpacialMeshBVH
* \ingroup Collision
* \brief  Bounding volume hierarchy structure.
*/
// ****************************************

class SpacialMeshBVH : public CollisionBaseObject
{
private :
	SpacialMeshBVH()
	{}
protected : 
	SpacialMeshBVH(const Matrix3x4& LtoGMatrix);

	void BuildFromTriangleList(Point3D* vertexList, int vertexCount, void* indexList, int triangleCount, bool onlyHorizontalAndVertical);

	void ReorderVerticesOnAxis(std::vector<v3f>& vertices, std::vector<unsigned int>& indices);

	int	CreateVirtualIndicesList(std::vector<int>& virtualIndices,const std::vector<v3f>& vertices);

	Matrix3x4						mLtoGMatrix; // check for horizontal / vertical planar surface

	std::vector<SpacialMeshBVHNode>	mNodeList;

	virtual bool CallLocalRayIntersection(Hit &hit, const Point3D& start, const Vector3D& dir)  const override;
	virtual bool CallLocalRayIntersection(std::vector<Hit> &hit, const Point3D& start, const Vector3D& dir)  const override;

	struct PlanarGroup // struct used to build groups
	{
		std::vector<unsigned int>	mTriangles;
		std::vector<std::pair<int, int> > mBorderEdges;
		Vector3D		mNormal;
#ifdef USE_LEAST_SQUARES
		Vector3D		u;
		Vector3D		v;
#endif
		Vector3D		mOrigin;
		float			mSurface;
		bool			mIsHOrV;
		std::set<std::pair<float, int>>	mSortStableTriangles;
		std::vector<unsigned int>	mInvalidTriangles;
		int				mValidcount;
	};

	struct SegmentCheck // struct used to build connected segment lists
	{
		// minIndice is the index in the list
		int mMaxIndice;
		int mTriangleIndice;
		int mSegmentFlag;
		SegmentCheck*	mNextSegment;
		SegmentCheck*	mTwin;
	};

	struct ConnectedTriangle // struct used to build planar groups
	{
		Vector3D	N;		// triangle normal
		Vector3D	tmpN;	// triangle normal buffer
		float		s;		// triangle surface
		float       h;		// triangle homogeneity 
		Vector3D	wN;		// weighted normal
		float		w[3];	// neightbor confidence 
	};

	struct TriangleGrp
	{
		int				grpID;
		union
		{
			unsigned int	rootTriangleIndex;
			bool			isValidTriangle;
		};
	};

	void BuildHalfEdgeList(const std::vector<unsigned int>& indices,
		const std::vector<v3f>& vertices,
		const std::vector<int>&	virtualIndices,
		std::vector<SegmentCheck>& PreallocatedSegments,
		std::vector<ConnectedTriangle>&	PreallocatedTriangles,
		std::vector<SegmentCheck*>& linkedSegmentList);

	void	IterativeTriangleConnection(const std::vector<unsigned int>& indices,
		std::vector<SegmentCheck>& PreallocatedSegments,
		std::vector<ConnectedTriangle>&	PreallocatedTriangles);


	void	FloodFillTriangleGroups(int triangleCount,
		const std::vector<unsigned int>& indices,
		std::vector<SegmentCheck>& PreallocatedSegments,
		std::vector<ConnectedTriangle>&	PreallocatedTriangles,
		std::vector<TriangleGrp>& triangleFlag,
		std::vector< std::set<unsigned int> >&	mergedGroups);

	// return final group count
	int	MergeConnectedGroups(const std::vector< std::set<unsigned int> >&	mergedGroups,
		std::vector < std::pair<int, int> >& MergeGroupIndex);

	void	CreatePlanarGroups(const std::vector<unsigned int>& indices,
		const std::vector<v3f>& vertices,
		std::vector<TriangleGrp>& triangleFlag,
		std::vector < std::pair<int, int> >& MergeGroupIndex,
		std::vector<SegmentCheck>& PreallocatedSegments,
		std::vector<ConnectedTriangle>&	PreallocatedTriangles,
		std::vector<PlanarGroup >& planarGroupList,
		bool onlyHorizontalAndVertical);

	void	MergeTwoPlanarGroups(std::vector<TriangleGrp>& triangleFlag, std::vector<SegmentCheck>& PreallocatedSegments,PlanarGroup& grp1, const PlanarGroup& grp2, int grpID1, int grpID2);

	BBox				mBBox3D;

public:

	static SpacialMeshBVH* BuildFromMesh(ModernMesh* mesh,const Matrix3x4& LtoGMatrix,bool onlyHorizontalAndVertical);
	virtual ~SpacialMeshBVH();
#ifdef KIGS_TOOLS
	// draw debug mInfo using GLSLDrawDebug
	virtual void DrawDebug(const Point3D& pos, const  Matrix3x4* mat, Timer *timer);
#endif

	virtual SpacialMeshBVH* getSpacialMeshBVH() override
	{
		return this;
	}

	const std::vector<SpacialMeshBVHNode>& getNodes() const
	{
		return mNodeList;
	}
};

#endif //_SPACIALMESHBVH_H_