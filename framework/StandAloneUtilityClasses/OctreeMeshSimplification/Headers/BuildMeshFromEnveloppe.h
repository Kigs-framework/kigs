#pragma once
#include "CoreSTL.h"
#include <vector>
#include "OctreeMeshSimplification.h"

class MSEdge
{
public:
	MSEdge(u32 v1, u32 v2)
	{
		v[0] = v1;
		v[1] = v2;
		t[0] = t[1] = -1;
		FlagAsOnCorner = false;
	}

	u32 v[2]; // indexes on vertex
	u32 t[2]; // indexes on faces
	u32	FlagAsOnCorner;
};
class MSFace
{
public:
	MSFace()
	{
		
	}

	// use highest bit to check "edge way"
	std::vector<u32> edges; // max should be 8 edges for really special cases ?
	v3f				 normal;
};

class BuildMeshFromEnveloppe
{
public:

	class MSVertice
	{
	public:
		MSVertice(const v3f& v, nodeInfo d) : mV(v), mOctreeNode(d)
		{
			mEdges.clear();
		}

		// return edge index in edge list going to the given vertice
		u32	getEdgeIndexInThisList(u32 v, const std::vector<MSEdge>& _edges) const
		{
			u32 vindex = 0;
			for (auto e : mEdges)
			{
				const MSEdge& currentE = _edges[e & 0x7fffffff];
				u32 ew = e >> 31;
				if (currentE.v[1- ew] == v)
				{
					return vindex;
				}
				vindex++;
			}
			return -1;
		}

		void	insertEdgeBefore(u32 insertAfter, u32 edgeToInsert)
		{
			std::vector<u32>::iterator it;
			for (it = mEdges.begin(); it != mEdges.end(); it++)
			{
				if (((*it) & 0x7fffffff) == (insertAfter & 0x7fffffff))
				{
					mEdges.insert(it, edgeToInsert);
					break;
				}
			}
		}

		void	removeEdge(u32 edgeToRemove)
		{
			std::vector<u32>::iterator it;
			for (it = mEdges.begin(); it != mEdges.end(); it++)
			{
				if (((*it) & 0x7fffffff) == (edgeToRemove& 0x7fffffff))
				{
					mEdges.erase(it);
					break;
				}
			}
		}

		std::vector<u32>	mEdges;
		v3f	mV;
		v3f mN;
		nodeInfo			mOctreeNode;
		// last step : flag point on plane (all surrounding triangles have the same normal) or point on 2 planes intersection
		u32 mFlag=0;
	};
protected:
	// add given vertice and return index
	unsigned int addVertice(const v3f& v, nodeInfo d)
	{
		mVertices.push_back({ v,d });
		return (unsigned int)(mVertices.size() - 1);
	}

	void	separateVertices(const MSOctreeContent& node, const nodeInfo& n);

	u32	addEdge(u32 v1, u32 v2)
	{
		const std::vector< u32>& searchExisting = mEdgeMap[v1 ^ v2];

		if (searchExisting.size())
		{
			for (const auto& i : searchExisting)
			{
				if ((mEdges[i].v[0] == v1) && (mEdges[i].v[1] == v2)) 
				{
					return i;
				}
				if ((mEdges[i].v[1] == v1) && (mEdges[i].v[0] == v2))
				{
					return i | (1 << 31); 
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
	std::vector<nodeInfo*>& mNodeList;
	const std::vector<MSSurfaceStruct>& mAllSurfaces;

	inline v3f		getEdgeVector(u32 e)
	{
		u32 ew = (e >> 31);
		u32 ei = (e & 0x7fffffff);
		MSEdge& current = mEdges[ei];

		v3f edgeDir = mVertices[current.v[1]].mV - mVertices[current.v[0]].mV;

		return ew?-edgeDir: edgeDir;
	}

	void	setUpVertices(nodeInfo& node);
	void	setUpEdges(nodeInfo node);
	void	setUpNormals();
	void	setUpFaces();
	void	splitFaces();
	// compute triangle normal and setup edge triangle references
	void	finishTriangleSetup();
	void	tagVerticesForSimplification();
	// return intern edge index (in vertice) & vertice index
	std::pair<u32, u32>		findBestPlanarMerge(u32 vindex);
	std::pair<u32, u32>		findBestLinearMerge(u32 vindex);

	bool	isMergeValid(MSVertice& v, u32 endV, u32 internEdgeIndex);
	bool	isLinearMergeValid(MSVertice& v, u32 endV, u32 internEdgeIndex);
	// return the list of edges just to be switch to new point
	std::vector<u32>		findInternEdgesToSwitch(MSVertice& v,std::pair<u32, u32> mergeWith);
	void					flattenTriangles(MSFace& t, u32 tIndex, u32 fromV, u32 toV,u32 flattenE);

	void	checkVOnCoplanarTriangles(MSVertice& v);
	void	splitQuadFace(u32 fi);
	void	setEdgesInteriorFace(MSFace& toSet,u32 fi, u32 oldfi);
	void	splitMoreThanQuadFace(u32 fi);
	void	mergeTriangles();

	void	checkCoherency();
	void	checkVerticeCoherency();

	void	checkVertice(nodeInfo& node, v3f& goodOne)
	{
		// first check trivial case (a point belonging to all present surfaces)
		if (checkVerticeTrivialCase(node, goodOne))
			return;

		// then check easy case (a point with a good score in principal present surfaces) 
		if (checkVerticeEasyCase(node, goodOne))
			return;

		// then compute a vertice according to cell topology
		computeVerticeFromCell(node, goodOne);
	}

	// different point in cell computation method
	bool	checkVerticeTrivialCase(nodeInfo node, v3f& goodOne);
	bool	checkVerticeEasyCase(nodeInfo node, v3f& goodOne);
	bool	computeVerticeFromCell(nodeInfo node, v3f& goodOne);


	// all cases
	void manageSeparateFreeFaces(const MSOctreeContent& node, const nodeInfo& n);
	void validateGoodIntersectionPoint(const MSOctreeContent& node, const nodeInfo& n);
	void setUpInternCellEdge(u8 mask,const MSOctreeContent& node, std::map<std::pair<u32, u8>, std::set<u32>>& foundEdges);

	u32 getFaceIndex(u32 facemask)
	{
		for (u32 i = 0; i < 6; i++)
		{
			if (facemask & (1 << i))
				return i;
		}
		return 0;
	}

	class searchGoodVerticeInSortedList
	{
	public:
		searchGoodVerticeInSortedList(const std::vector<std::pair<v3f,u8>>& vlist,const v3f& bboxCenter);

		bool getFoundVertice(v3f& v)
		{
			if (mFound)
			{
				v = mFoundV;
				
				return true;
			}
			return false;
		}
	private:

		bool					findSecondPoint();
		bool					findThirdPoint();

		v3f						mBBoxCenter;

		struct foundpt
		{
			size_t	i;
			float	bestSQRDist;
			float	bestSQRDelta;
		};

		foundpt					mPointList[3];
		size_t					mCurrentIndex;

		bool					mFound = false;
		v3f						mFoundV;

		const std::vector<std::pair<v3f, u8>>& mList;

	};

	v3f searchGoodVerticeInCellForDirection(nodeInfo node, const MSOctreeContent& cnode, const v3f& direction,BBox currentBBox);

public:

	BuildMeshFromEnveloppe(std::vector<nodeInfo*>& nodes, const std::vector<MSSurfaceStruct>& surfaces) : mNodeList(nodes), mAllSurfaces(surfaces)
	{}
	void	Build();
	void	addFinalizedMesh(std::vector<v3f>& vertices, std::vector<u32>& indices);

	std::vector<std::pair<v3f, v3f>>	getEdges() const;
	std::vector<MSVertice>					getEnveloppeVertices() const;

};