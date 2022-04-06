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
		flags = 0;
	}

	u32 v[2]; // indexes on vertex
	u32 t[2]; // indexes on faces

	// flag "corner edge" (1) and "inner corner edge" (2)
	u32	flags=0;
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
		MSVertice(const v3f& v, const nodeInfo* d, u32 grpflag) : mV(v), mOctreeNode(d), mGroupFlag(grpflag)
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

		u32	getLocalEdgeIndex(u32 ei) const
		{
			u32 result = 0;
			for (auto e : mEdges)
			{
				if((e & 0x7fffffff)==ei)
				{
					return result;
				}
				result++;
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

		std::vector<u32>		mEdges;
		v3f						mV;
		v3f						mN;
		const nodeInfo*			mOctreeNode;
		// last step : flag point on plane (all surrounding triangles have the same normal) or point on 2 planes intersection
		u32						mFlag=0;
		u32						mGroupFlag = 0;
	};
protected:
	// add given vertice and return index
	unsigned int addVertice(const v3f& v, const nodeInfo* d, u32 grpflag)
	{
		mVertices.push_back({ v,d ,grpflag});
		return (unsigned int)(mVertices.size() - 1);
	}

	// add edge without cache
	u32	addEdge(u32 v1, u32 v2)
	{
		mEdges.push_back({ v1,v2 });
		return (u32)(mEdges.size() - 1);
	}

	// add edge searching for duplicates in given cache
	u32	addEdge(u32 v1, u32 v2, std::map < u32, std::vector<u32>>& edgeMap)
	{
		u32 key = (v1 < v2) ? (v1 ^ (v2 >> 3)) : (v2 ^ (v1 >> 3));
		const std::vector<u32>& searchExisting = edgeMap[key];
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
		edgeMap[key].push_back((u32)(mEdges.size()));
		mEdges.push_back({ v1,v2 });
		return (u32)(mEdges.size() - 1);
	}

	std::vector<MSVertice>		mVertices;
	std::vector<MSFace>			mFaces;
	std::vector<MSEdge>			mEdges;
	std::vector<nodeInfo*>&		mNodeList;

	class CurrentCellData
	{
	public:
		std::vector<MSSurfaceStruct>			mCellSurfaces;
		std::map<v3f, std::set<u32>>			mPerVSurfaces;
		void	clear()
		{
			mCellSurfaces.clear();
			mPerVSurfaces.clear();
		}
	};

	CurrentCellData	mCellData;

	const std::vector<MSTriangleInfo>&	mTriangleInfos;

	bool mWTF = false;

	inline v3f		getEdgeVector(u32 e)
	{
		u32 ew = (e >> 31);
		u32 ei = (e & 0x7fffffff);
		MSEdge& current = mEdges[ei];

		v3f edgeDir = mVertices[current.v[1]].mV - mVertices[current.v[0]].mV;

		return ew?-edgeDir: edgeDir;
	}


	void	initCellSurfaceList(nodeInfo& node);
	void	addMultipleVertices(nodeInfo& node, const v3f& goodpoint,u32 grpflag);
	void	setUpVertices(nodeInfo& node);
	void	setUpEdges(nodeInfo node, std::map < u32, std::vector<u32>>& edgeMap);
	void	setUpNormals();
	void	setUpFaces();

	// remove empty vertice with no more edges
	void removeEmptyVertice(u32 verticeindex, std::vector<u32>& verticelist);
	// remove face with no more edges
	void removeEmptyFace(u32 faceindex, std::vector<u32>& facelist);
	// remove given edge from given vertice
	void	removeEdgeFromVertice(u32 edgeindex, u32 verticeindex);
	// remove given edge from given face
	void	removeEdgeFromFace(u32 edgeindex, u32 faceindex);
	// remove edge from list after removing it from faces
	void	removeEdge(u32 edgeindex,std::vector<u32>& edgelist);
	// reorder edges in vertices
	void	reorderEdgesInVertices();

	void	removeFlatFaces();
	void	DetectFlatTriangles(const std::vector<u32>& verticesIndex);

	void	setupInnerCorners();

	class innerCornerEdge
	{
	public:
		v3f					mVertices[2];
		const nodeInfo*		mOctreeNode[2];
		v3f					mFinalPos;
	};
	std::vector<innerCornerEdge>	mInnerCornersList;

	void	moveInnerCorners();

	void	firstClean();

	std::vector<u32>	mFinalMergedVIndex;
	void	finalClean();
	void	splitFaces();
	// compute triangle normal
	void	computeTriangleNormals();
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
	void	doShringEdge(MSVertice& v,const std::pair<u32, u32>& mergeWith, u32 vindex);
	void	mergeTriangles();

	void	checkCoherency();
	void	checkVerticeCoherency();

	void	checkVertice(nodeInfo& node, v3f& goodOne,u32& grpflag)
	{
		grpflag = 0;
		// first check trivial case (a point belonging to all present surfaces)
		if (checkVerticeTrivialCase(node, goodOne, grpflag))
			return;

		// then check easy case (a point with a good score in principal present surfaces) 
		if (checkVerticeEasyCase(node, goodOne, grpflag))
			return;

		// then compute a vertice according to cell topology
		computeVerticeFromCell(node, goodOne, grpflag);
	}

	// different point in cell computation method
	bool	checkVerticeTrivialCase(nodeInfo node, v3f& goodOne, u32& grpflag);
	bool	checkVerticeEasyCase(nodeInfo node, v3f& goodOne, u32& grpflag);
	bool	computeVerticeFromCell(nodeInfo node, v3f& goodOne, u32& grpflag);


	// all cases
	void manageSeparateFreeFaces(const MSOctreeContent& node, const nodeInfo& n);

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

	v3f searchGoodVerticeInCellForDirection(nodeInfo node, const MSOctreeContent& cnode, const v3f& direction,BBox currentBBox,u32& grpflag);

public:

	BuildMeshFromEnveloppe(std::vector<nodeInfo*>& nodes,const std::vector<MSTriangleInfo>& triinfos) : mNodeList(nodes), mTriangleInfos(triinfos)
	{}
	void	Build();
	void	addFinalizedMesh(std::vector<v3f>& vertices, std::vector<u32>& indices);

	std::vector<std::pair<std::pair<v3f, v3f>,u32>>	getEdges() const;
	std::vector<MSVertice>					getEnveloppeVertices() const;
	bool hasError() const
	{
		return mWTF;
	}

};