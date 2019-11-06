#ifndef _SPACIALMESHBVH_H_
#define _SPACIALMESHBVH_H_

#include "CoreModifiable.h"
#include "TecLibs/Tec3D.h"
#include "TecLibs/3D/3DObject/BBox.h"
#include "TecLibs/2D/BBox2D.h"
#include "Node3D.h"
#include "CollisionBaseObject.h"

class ModernMesh;

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
	SpacialMeshBVHNode() : m_3DBox(BBox::PreInit())
	{
		
	}

	unsigned int	getStableTriangleListSize() const
	{
		return m_StableTriangles.size();
	}

	v3f	getStableTriangleCenter(int index) const
	{
		unsigned int t=m_StableTriangles[index];

		v2f tcenter = m_2D_Triangles[3*t] + m_2D_Triangles[3*t + 1] + m_2D_Triangles[3*t + 2];
		tcenter *= 1.0f / 3.0f;

		v3f center = m_o + m_u * tcenter.x + m_v * tcenter.y;
		return center;
	}

	const v3f& getNormal() const
	{
		return m_Normal;
	}

	const BBox& getBBox() const
	{
		return m_3DBox;
	}

	float getSurface() const { return m_surface; }

protected:

	friend class SpacialMeshBVH;

	void add2DVertice(const v3f& pt)
	{
		m_3DBox.Update(pt);
		// project pt on u and v
		v3f	toProject(pt);
		toProject -= m_o;

		v2f projected;
		projected.x = Dot(toProject, m_u);
		projected.y = Dot(toProject, m_v);
		m_2D_Triangles.push_back(projected);
	}

	v3f	getProjected(const v3f& pt)
	{
		v3f	toProject(pt);
		toProject -= m_o;

		float p = Dot(toProject, m_Normal);
		
		v3f result = pt - m_Normal * p;
		return result;
	}

	void	computeBBox()
	{
		if (m_2D_Triangles.size()>1)
		{
			m_BBox.Init(m_2D_Triangles[0]);
			m_BBox.Update(&m_2D_Triangles[1], m_2D_Triangles.size() - 1);
		}
	}

	v3f		m_Normal;	// normal + d = plane equation

	// 2D planar coordinate system
	v3f		m_o;	// origin 
	v3f		m_u;	// u
	v3f		m_v;	// v

	std::vector<v2f>			m_2D_Triangles; // triangles
	std::vector<unsigned int>	m_StableTriangles;
	BBox2D				m_BBox;
	BBox				m_3DBox;
	float m_surface = 0.0f;

	bool	intersect(Hit& hit, const v3f& start, const v3f& dir) const;

public:
#ifdef KIGS_TOOLS
	mutable bool m_Hit = false;
#endif
};


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

	Matrix3x4						m_LtoGMatrix; // check for horizontal / vertical planar surface

	std::vector<SpacialMeshBVHNode>	m_NodeList;

	virtual bool CallLocalRayIntersection(Hit &hit, const Point3D& start, const Vector3D& dir)  const override;
	virtual bool CallLocalRayIntersection(std::vector<Hit> &hit, const Point3D& start, const Vector3D& dir)  const override;

	struct PlanarGroup // struct used to build groups
	{
		std::vector<unsigned int>	triangles;
		std::vector<std::pair<int, int> > borderEdges;
		Vector3D		normal;
#ifdef USE_LEAST_SQUARES
		Vector3D		u;
		Vector3D		v;
#endif
		Vector3D		origin;
		float			surface;
		bool			is_H_Or_V;
		std::set<std::pair<float, int>>	sortStableTriangles;
		std::vector<unsigned int>	invalid_triangles;
		int				validcount;
	};

	struct SegmentCheck // struct used to build connected segment lists
	{
		// minIndice is the index in the list
		int maxIndice;
		int triangleIndice;
		int segmentFlag;
		SegmentCheck*	nextSegment;
		SegmentCheck*	Twin;
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

	BBox				m_3DBox;

public:

	static SpacialMeshBVH* BuildFromMesh(ModernMesh* mesh,const Matrix3x4& LtoGMatrix,bool onlyHorizontalAndVertical);
	virtual ~SpacialMeshBVH();
#ifdef KIGS_TOOLS
	// draw debug info using GLSLDrawDebug
	virtual void DrawDebug(const Point3D& pos, const  Matrix3x4* mat, Timer *timer);
#endif

	virtual SpacialMeshBVH* getSpacialMeshBVH() override
	{
		return this;
	}

	const std::vector<SpacialMeshBVHNode>& getNodes() const
	{
		return m_NodeList;
	}
};

#endif //_SPACIALMESHBVH_H_