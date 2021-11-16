#include "CollisionMeshSimplification.h"
//#include "SimpleShapeMeshCollider.h"
#include <math.h>
#include <map>
#include "TecLibs/2D/BBox2D.h"


// local operator for std::set
bool operator<(const v3f& U, const v3f& V)
{
	if (U.x == V.x)
	{
		if (U.y == V.y)
		{
			if (U.z == V.z)
			{
				return false;
			}
			return U.z < V.z;
		}
		return U.y < V.y;
	}
	return U.x < V.x;
}

v3f	MSPlaneStruct::mAxisNormal[3] = { {1.0f,0.0f,0.0f},{0.0f,1.0f,0.0f},{0.0f,0.0f,1.0f} };

unsigned int adjacent_faces[3][4] = { {4,8,16,32},{1,2,16,32},{1,2,4,8} };


IMPLEMENT_CLASS_INFO(MeshSimplificationOctree);

IMPLEMENT_CONSTRUCTOR(MeshSimplificationOctree)
{
	mRootNode = new MeshSimplificationOctreeNode();
}

void			MeshSimplificationOctree::setVoxelContent(const v3f& P1, const v3f& P2, const v3f& P3, u32 surfaceIndex)
{

	v3f	octreeCoords[3];

	octreeCoords[0] = P1;
	octreeCoords[1] = P2;
	octreeCoords[2] = P3;

	for (int pts = 0; pts < 3; pts++)
	{
		octreeCoords[pts] -= mBBox.m_Min;
		octreeCoords[pts] *= mBBoxCoef;
		octreeCoords[pts] += mTranslate;
	}


	BBox	triangleBBox(octreeCoords[0]);
	triangleBBox.Update(octreeCoords[1]);
	triangleBBox.Update(octreeCoords[2]);

	const MSSurfaceStruct& surf = (*mAllSurfacesPtr)[surfaceIndex];
	// get triangle normal
	v3f triangleNormal = surf.mPlane.mNormal;

	int projplaneindex = 0;
	float bestComposant = 0.0f;
	// choose best 2d projection plane
	for (u32 i = 0; i < 3; i++)
	{
		if (fabsf(triangleNormal[i]) > bestComposant)
		{
			projplaneindex = i;
			bestComposant = fabsf(triangleNormal[i]);
		}
	}

	int coordsIndex[3];
	coordsIndex[0] = projplaneindex;

	int otherPlaneIndex = 0;
	for (int otherPlanes = 0; otherPlanes < 3; otherPlanes++)
	{
		if (otherPlanes == projplaneindex)
			continue;

		otherPlaneIndex++;
		coordsIndex[otherPlaneIndex] = otherPlanes;
	}

	std::map<float,std::map<float,std::set<v3f>>> intersectionStruct;

	int proj2DAxis1 = coordsIndex[1];
	int proj2DAxis2 = coordsIndex[2];

	// first, add triangle vertices
	for (int pts = 0; pts < 3; pts++)
	{
		intersectionStruct[octreeCoords[pts][proj2DAxis1]][octreeCoords[pts][proj2DAxis2]].insert(octreeCoords[pts]);
	}

	for (int planeIndex = 0; planeIndex < 3; planeIndex++)
	{
		otherPlaneIndex = 0;
		for (int otherPlanes = 0; otherPlanes < 3; otherPlanes++)
		{
			if (otherPlanes == planeIndex)
				continue;

			otherPlaneIndex++;
			coordsIndex[otherPlaneIndex] = otherPlanes;
		}

		int planestart = floor(triangleBBox.m_Min[planeIndex]);
		int planeend = ceil(triangleBBox.m_Max[planeIndex]);

		for (int pos1 = planestart+1; pos1 < planeend; pos1++)
		{

			MSPlaneStruct p;
			p.mNormal = MSPlaneStruct::mAxisNormal[planeIndex];
			p.mDist = -pos1;
			std::set<v3f>	segments;

			CollisionMeshSimplification::trianglePlaneIntersection(octreeCoords[0], octreeCoords[1], octreeCoords[2], p, segments);

			for (const auto& p : segments)
			{
				intersectionStruct[p[proj2DAxis1]][p[proj2DAxis2]].insert(p);
			}

			// then cut segment
			if (segments.size() == 2)
			{
				v3f p3d[2];
				int resultIndex = 0;
				for (auto result : segments)
				{
					p3d[resultIndex] = result;
					p3d[resultIndex][planeIndex] = (float)pos1; // avoid rounding problemns
					resultIndex++;
				}

				// cut segment along other axis
				for (int otherPlanes = 0; otherPlanes < 2; otherPlanes++)
				{
					int secondplane = coordsIndex[otherPlanes + 1];
					int thirdplane = coordsIndex[2 - otherPlanes];

					int segmentstart = floor(triangleBBox.m_Min[secondplane]);
					int segmentend = ceil(triangleBBox.m_Max[secondplane]);

					for (int pos2 = segmentstart + 1; pos2 < segmentend; pos2++)
					{
						v3f newintersection;
						newintersection[planeIndex] = p3d[0][planeIndex];
						newintersection[secondplane] = (float)pos2;
						newintersection[thirdplane] = (newintersection[secondplane] - p3d[0][secondplane]) / (p3d[1][secondplane] - p3d[0][secondplane]);

						if ((newintersection[thirdplane] >= 0.0f) && (newintersection[thirdplane] <= 1.0f))
						{
							newintersection[thirdplane] *= (p3d[1][thirdplane] - p3d[0][thirdplane]);
							newintersection[thirdplane] += p3d[0][thirdplane];

							intersectionStruct[newintersection[proj2DAxis1]][newintersection[proj2DAxis2]].insert(newintersection);
						}
					}
				}
			}
		}
	}


	class doubleMapIter
	{
	protected: 

		std::map<float, std::map<float, std::set<v3f>>>::const_iterator	mFirstAxisIterators[4];
		std::map<float, std::set<v3f>>									mSecondAxisMap;
		std::map<float, std::set<v3f>>::const_iterator					mSecondAxisIterators[4];

		const std::map<float, std::map<float, std::set<v3f>>>&			mDataStruct;

		bool	mInnerAxisEnd;

		void	initSecondAxisOuterIterators()
		{
			mSecondAxisIterators[0] = mSecondAxisMap.begin(); // outer start
			mSecondAxisIterators[3] = mSecondAxisMap.end(); // outer end
			initSecondAxisInnerIterators();
		}

		std::map<float, std::map<float, std::set<v3f>>>::const_iterator getNextFAOuterIterator(std::map<float, std::map<float, std::set<v3f>>>::const_iterator itcurrent, std::map<float, std::map<float, std::set<v3f>>>::const_iterator itend)
		{
			std::map<float, std::map<float, std::set<v3f>>>::const_iterator it = itcurrent;
			u32 StartPos = (u32)((*it).first);
			u32 EndPos = StartPos;
			do
			{
				it++;
				if (it == itend)
					break;
				EndPos = (u32)((*it).first);
			} while (StartPos == EndPos);

			return it;
		}

		std::map<float, std::set<v3f>>::const_iterator getNextSAOuterIterator(std::map<float, std::set<v3f>>::const_iterator itcurrent, std::map<float, std::set<v3f>>::const_iterator itend)
		{
			std::map<float, std::set<v3f>>::const_iterator it = itcurrent;
			u32 StartPos = (u32)((*it).first);
			u32 EndPos = StartPos;
			do
			{
				it++;
				if (it == itend)
					break;
				EndPos = (u32)((*it).first);
			} while (StartPos == EndPos);

			return it;
		}

		void initFirstAxisInnerMap()
		{
			mFirstAxisIterators[1] = mFirstAxisIterators[0];
			mFirstAxisIterators[2] = getNextFAOuterIterator(mFirstAxisIterators[1], mFirstAxisIterators[3]);

			mSecondAxisMap.clear();
			bool done = false;
			do
			{
				if (mFirstAxisIterators[1] == mFirstAxisIterators[2])
					done = true;
				if (mFirstAxisIterators[1] != mFirstAxisIterators[3])
				{
					const auto& v= (*mFirstAxisIterators[1]).second;

					for (const auto& m : v)
					{
						for (const auto& pv : m.second)
						{
							mSecondAxisMap[m.first].insert(pv);
						}
					}
					mFirstAxisIterators[1]++;
				}

				
			} while (!done);
			
		}

		void initSecondAxisInnerIterators()
		{
			mSecondAxisIterators[1] = mSecondAxisIterators[0];
			mSecondAxisIterators[2] = getNextSAOuterIterator(mSecondAxisIterators[1], mSecondAxisIterators[3]);
			mInnerAxisEnd = false;
		}

	public:
		doubleMapIter(const std::map<float, std::map<float, std::set<v3f>>>& datastruct) : mDataStruct(datastruct)
		{
			mFirstAxisIterators[0] = mDataStruct.begin(); // outer start
			mFirstAxisIterators[3] = mDataStruct.end(); // outer end
			initFirstAxisInnerMap();

			initSecondAxisOuterIterators();
		}

		void	firstAxisOuterInc()
		{
			if (mFirstAxisIterators[0] != mFirstAxisIterators[3])
			{
				mFirstAxisIterators[0] = getNextFAOuterIterator(mFirstAxisIterators[0], mFirstAxisIterators[3]);

				// check that this is not the last border (<=> empty node)
				auto checklast = mFirstAxisIterators[0];
				if (checklast != mFirstAxisIterators[3])
				{
					checklast++;
					if (checklast == mFirstAxisIterators[3])
						mFirstAxisIterators[0] = checklast;

					if (mFirstAxisIterators[0] != mFirstAxisIterators[3])
					{
						initFirstAxisInnerMap();
						initSecondAxisOuterIterators();
					}
				}
			}
		}

		void	secondAxisOuterInc()
		{
			if (mSecondAxisIterators[0] != mSecondAxisIterators[3])
			{
				mSecondAxisIterators[0] = getNextSAOuterIterator(mSecondAxisIterators[0], mSecondAxisIterators[3]);

				// check that this is not the last border (<=> empty node)
				auto checklast = mSecondAxisIterators[0];
				if (checklast != mSecondAxisIterators[3])
				{
					checklast++;
					if (checklast == mSecondAxisIterators[3])
						mSecondAxisIterators[0] = checklast;

					if (mSecondAxisIterators[0] != mSecondAxisIterators[3])
						initSecondAxisInnerIterators();
				}

				
			}

		}
		void	secondAxisInnerInc()
		{
			if (mSecondAxisIterators[1] != mSecondAxisIterators[2])
			{
				mSecondAxisIterators[1]++;
			}
			else
			{
				mInnerAxisEnd = true;
			}
		}

		bool FirstAxisOuterNotEnd()
		{
			return (mFirstAxisIterators[0] != mFirstAxisIterators[3]);
		}

		bool SecondAxisOuterNotEnd()
		{
			return (mSecondAxisIterators[0] != mSecondAxisIterators[3]);
		}

		bool SecondAxisInnerNotEnd()
		{
			return !mInnerAxisEnd;
		}
		bool SecondAxisInnerValid()
		{
			return mSecondAxisIterators[1] != mSecondAxisIterators[3];
		}

		const std::set<v3f>& getData()
		{
			return (*mSecondAxisIterators[1]).second;
		}

		float getFirstAxisPos()
		{
			return (*mFirstAxisIterators[0]).first;
		}

		float getSecondAxisPos()
		{
			return (*mSecondAxisIterators[0]).first;
		}
	};

	// now for each cell containing data, update octree

	doubleMapIter	dmi(intersectionStruct);

	while (dmi.FirstAxisOuterNotEnd())
	{

		while (dmi.SecondAxisOuterNotEnd())
		{
			std::set <v3f> allptsInSquare;
			
			do
			{

				if (dmi.SecondAxisInnerValid())
				{
					const std::set<v3f>& pts = dmi.getData();

					for (const auto& p : pts)
					{
						allptsInSquare.insert(p);
					}
				}

				dmi.secondAxisInnerInc();
			} while (dmi.SecondAxisInnerNotEnd());

			if (allptsInSquare.size())
			{

				float thirdAxisMin, thirdAxisMax;
				thirdAxisMin = thirdAxisMax = (*(allptsInSquare.begin()))[projplaneindex];

				for (const auto& p : allptsInSquare)
				{
					if (p[projplaneindex] < thirdAxisMin)
					{
						thirdAxisMin = p[projplaneindex];
					}
					else if (p[projplaneindex] > thirdAxisMax)
					{
						thirdAxisMax = p[projplaneindex];
					}
				}

				for (u32 thirdAxis = thirdAxisMin; (float)thirdAxis < thirdAxisMax; thirdAxis++)
				{
					float thirdAxisFloat = thirdAxis;

					v3i octreepos;
					octreepos[projplaneindex] = thirdAxis;
					octreepos[proj2DAxis1] = dmi.getFirstAxisPos();
					octreepos[proj2DAxis2] = dmi.getSecondAxisPos();

					v3f octreeposfloat(octreepos.x, octreepos.y, octreepos.z);

					MeshSimplificationOctreeNode* nodeToFill = getFinalNodeAtPos(static_cast<MeshSimplificationOctreeNode*>(mRootNode), octreepos, 1);
					for (const auto& p : allptsInSquare)
					{
						if ((p[projplaneindex] >= thirdAxisFloat) && (p[projplaneindex] <= (thirdAxisFloat + 1.0f)))
						{
							v3f posInLeaf(p - octreeposfloat);

							u8 sidemask = 0;
							for (u32 ci = 0; ci < 3; ci++)
							{
								if (posInLeaf[ci] == 0.0f)
								{
									sidemask |= 1 << (ci * 2);
								}
								else if (posInLeaf[ci] == 1.0f)
								{
									sidemask |= 2 << (ci * 2);
								}
							}

							nodeToFill->setContent(p, surfaceIndex, sidemask);
						}
					}

				}
			}

			dmi.secondAxisOuterInc();
		}

		dmi.firstAxisOuterInc();
	}
	
}

std::vector<nodeInfo>	MeshSimplificationOctree::floodFillEmpty(u32 setBrowsingFlag)
{
	std::vector<nodeInfo> envelopenodelist;
	v3i startingPos(0, 0, 0);
	nodeInfo startingNode = getVoxelAt(startingPos);

	callRecurseFloodFill(startingNode, envelopenodelist, setBrowsingFlag);

	return envelopenodelist;
}

template<typename F>
void	MeshSimplificationOctree::floodFillWithCondition(const nodeInfo& startPos, F&& condition,u32 setBrowsingFlag)
{
	recursiveFloodFill t(*this, nullptr, setBrowsingFlag);
	t.run(startPos, condition);
}

MeshSimplificationOctreeNode* MeshSimplificationOctree::getFinalNodeAtPos(MeshSimplificationOctreeNode* currentNode, const v3i& pos,int currentDepth)
{
	const int maxDepth = mMaxDepth;

	if (currentDepth <= maxDepth)
	{
		if (currentNode->isLeaf())
		{
			currentNode->split(currentNode->getContentType());
		}
	}

	int currentDecal = maxDepth - currentDepth;
	unsigned int index = ((pos.x >> currentDecal) & 1) | (((pos.y >> currentDecal) & 1) << 1) | (((pos.z >> currentDecal) & 1) << 2);

	MeshSimplificationOctreeNode* nextNode = static_cast<MeshSimplificationOctreeNode*>(currentNode->getChild(index));
	currentDepth++;
	if (currentDepth > maxDepth)
	{
		return nextNode;
	}

	return getFinalNodeAtPos(nextNode, pos, currentDepth);
	
}

u32			CollisionMeshSimplification::insertSurface(const MSSurfaceStruct& toadd)
{
	u32 surfaceindex = 0;
	for (auto& s : mAllSurfaces)
	{
		if (fabsf(s.mPlane.mDist - toadd.mPlane.mDist) < 0.001)
		{
			if (NormSquare(s.mPlane.mNormal - toadd.mPlane.mNormal) < 0.001)
			{
				s.mSurface += toadd.mSurface;
				return surfaceindex;
			}
		}
		surfaceindex++;
	}
	mAllSurfaces.push_back(toadd);
	return (u32)(mAllSurfaces.size()-1);
}


void		CollisionMeshSimplification::computeSurfaceList(const std::vector<u32>& indices, const std::vector<v3f>& vertices)
{
	// compute all planes in the mesh
	mAllSurfaces.clear();
	for (int i = 0; i < indices.size(); i += 3) // send each triangles
	{
		v3f	u(vertices[indices[i + 1]] - vertices[indices[i]]);
		v3f	v(vertices[indices[i + 2]] - vertices[indices[i]]);

		v3f c;
		c.CrossProduct(u, v);

		MSSurfaceStruct toAdd;
		toAdd.mPlane.mNormal = c;
		float norm = Norm(toAdd.mPlane.mNormal);
		toAdd.mPlane.mNormal /= norm;

		// take object shift into account
		toAdd.mPlane.mDist = Dot(vertices[indices[i]]+mObjectShift, toAdd.mPlane.mNormal);
		toAdd.mSurface = norm * 0.5f;
		mTriangleSurfaceIndex.push_back(insertSurface(toAdd));
	}

	// compute total surface
	float totalsurf = 0.0f;
	mMaxSurface = 0.0f;
	for (const auto& surf : mAllSurfaces)
	{
		if (mMaxSurface < surf.mSurface)
		{
			mMaxSurface = surf.mSurface;
		}
		totalsurf += surf.mSurface;
	}

	// then average one
	mAverageSurface = totalsurf / (float)mAllSurfaces.size();
	mNormalisedSurfaceCoef = 1.0f / mMaxSurface;
}

// for collisions, normals and materials does not matter
CollisionMeshSimplification::CollisionMeshSimplification(const std::vector<u32>& indices, const std::vector<v3f>& vertices,float precision)
{

	// compute BBox of object
	BBox	b;
	b.SetEmpty();
	for(const auto& p:vertices)
	{
		b.Update(p);
	}

	// now compute best switch to apply to avoid segment on octree cutting planes
	mOctreeShift.Set(0.0f,0.0f,0.0f);

	{
		float oneOnP = 1.0f / precision;
		std::vector<float>	switches[3];
		// start with 0
		switches[0].push_back(0.0f);
		switches[1].push_back(0.0f);
		switches[2].push_back(0.0f);

		float dontcare;
		for (const auto& p : vertices)
		{
			v3f p1(p);
			p1 -= b.m_Min;
			p1 *= oneOnP;
			switches[0].push_back(modf(p1.x, &dontcare));
			switches[1].push_back(modf(p1.y, &dontcare));
			switches[2].push_back(modf(p1.z, &dontcare));
		}

		// ends with 1
		switches[0].push_back(1.0f);
		switches[1].push_back(1.0f);
		switches[2].push_back(1.0f);

		// sort each vector
		std::sort(switches[0].begin(), switches[0].end());
		std::sort(switches[1].begin(), switches[1].end());
		std::sort(switches[2].begin(), switches[2].end());

		// then compute max delta for each dim
		for (u32 dim = 0; dim < 3; dim++)
		{
			const u32 maxsize = (u32)(switches[dim].size() - 1);
			float bestd = 0.0f;
			u32 besti = 0;
			for (u32 i = 0; i < maxsize; i++)
			{
				float d = switches[dim][i + 1] - switches[dim][i];
				if (d > bestd)
				{
					bestd = d;
					besti = i;
				}
			}

			mOctreeShift[dim] = 1.0f - (switches[dim][besti] + bestd * 0.5f);

		}

		mObjectShift = mOctreeShift;
		mObjectShift *= precision;
	}

	// create octree with previous precisop, and shift
	mOctree = KigsCore::GetInstanceOf("localOctree", "MeshSimplificationOctree");
	mOctree->setValue("Precision", precision);
	mOctree->setValue("Translate", mOctreeShift);
	mOctree->setBBox(b);
	mOctree->Init();


	mCellSurface = precision * precision;

	// compute all surfaces in object
	computeSurfaceList(indices, vertices);
	mOctree->setSurfaceList(mAllSurfaces);

	// then rasterize all triangles in octree
	u32 triangleIndex = 0;
	for (int i=0;i< indices.size();i+=3) // send each triangles
	{
		mOctree->setVoxelContent(vertices[indices[i]], vertices[indices[i+1]], vertices[indices[i + 2]], mTriangleSurfaceIndex[triangleIndex]);
		triangleIndex++;
	}

	// TODO
	// probably a way here to merge cells (not border cells where all 8 sons contains only 1 unique surface or are empty )


	// fill empty cases outside of the object
	// empty cases will be "flaged" with 2
	// retrieve envelope cell list
	std::vector<nodeInfo> envelopenodelist= mOctree->floodFillEmpty(2);
	
	
	// clear visited neighbor flag on nodes
	{

		auto clearFlag = [](OctreeNodeBase* currentNode)->void
		{
			currentNode->clearNFlag();
		};

		MeshSimplificationOctree::applyOnAllNodes clearVisitedN(*mOctree, clearFlag);
	}
	{

		auto condition = [](const nodeInfo& c)->bool
		{
			return (c.node->getBrowsingFlag() == 0);
		};

		mGroupCount = 0;
		// now count case group numbers inside object
		for (auto& n : envelopenodelist)
		{
			if (n.node->getBrowsingFlag() == 0)
			{
				// each group will be flagged with 4,6,8...
				mOctree->floodFillWithCondition(n, condition, (mGroupCount+2)*2);
				mGroupCount++;
			}
		}

		// now tag envelope nodes
		for (auto& n : envelopenodelist)
		{
			n.getNode<MeshSimplificationOctreeNode>()->getContentType().mData->mOtherFlags = 1;
		}
	}

	// check for empty neighbor for each node and set neighbors
	for (auto& n : envelopenodelist)
	{
		n.getNode<MeshSimplificationOctreeNode>()->checkEmptyNeighbors(*mOctree,n);
	}

	for (u32 i = 0; i < mGroupCount; i++)
	{
		rebuildMesh(i,envelopenodelist);
	}

	

	
	/*

	for (auto& n : nodeperfreefacecount[1])
	{
		if (n.node) // just in case
		{
			std::set<u32> i=n.getNode<MeshSimplificationOctreeNode>()->getTrianglesIndexes();
			std::set<u32> surfacesinCase;
			for (auto ti : i)
			{
				surfacesinCase.insert(mTriangleSurfaceIndex[ti]);
			}

		}
	}
	*/

}

void CollisionMeshSimplification::rebuildMesh(u32 groupIndex, const std::vector<nodeInfo>& envelopenodes)
{

	groupIndex += 2;
	groupIndex *= 2;


	// create node list for this group
	std::vector<nodeInfo> thisGroupEnvelopeNodes;

	for (u32 i = 0; i < envelopenodes.size(); i++)
	{
		auto& n = envelopenodes[i];
		if (n.getNode<MeshSimplificationOctreeNode>()->getBrowsingFlag() == groupIndex) // one group at a time
		{
			thisGroupEnvelopeNodes.push_back(n);
		}
	}

	if (thisGroupEnvelopeNodes.size() <= 1) // just one isolated cell for this group ? discard it
	{
		return;
	}

	// ok, let's build a mesh
	BuildMeshFromEnveloppe meshbuilder(thisGroupEnvelopeNodes,mAllSurfaces,mCellSurface);
	meshbuilder.Build();

}

void	BuildMeshFromEnveloppe::searchFirstEdge(u32 verticeIndex)
{
	for (const auto& n : mVertices[verticeIndex].mData->mNeighbors)
	{
		
	}
}

bool	BuildMeshFromEnveloppe::checkUniqueSurface(nodeInfo node,v3f& goodOne)
{
	MeshSimplificationOctreeNode* currentNode = node.getNode<MeshSimplificationOctreeNode>();
	const auto& content = currentNode->getContentType();

	// first check if a point corresponding to all present surfaces is available
	std::map<v3f, std::set<u32>>	perVSurfaces;
	std::set<u32>					allSurfaces;
	std::vector<float>				surfaceSize;

	for (const auto& v : content.mData->mVertices)
	{
		allSurfaces.insert(v.mSurfaceIndex);
		perVSurfaces[v.mVertex].insert(v.mSurfaceIndex);
		surfaceSize.push_back(mAllSurfaces[v.mSurfaceIndex].mSurface);
	}

	// retreive vertices in all available surfaces
	goodOne.Set(0.0f, 0.0f, 0.0f);
	u32 countGoodOnes = 0;
	for (const auto& v : perVSurfaces)
	{
		if (v.second.size() == allSurfaces.size())
		{
			goodOne += v.first;
			countGoodOnes++;
		}
	}

	if (countGoodOnes)
	{
		goodOne *= 1.0f / ((float)countGoodOnes);
		return true;
	}


	std::sort(surfaceSize.begin(), surfaceSize.end());
	// check if there is a good candidate for "bigger" surfaces only
	if ((surfaceSize[0] / surfaceSize[surfaceSize.size() - 1]) > 0.2f)
	{
		return false;
	}

	allSurfaces.clear();
	perVSurfaces.clear();

	float minsurf = surfaceSize[surfaceSize.size() - 1] * 0.2f;

	for (const auto& v : content.mData->mVertices)
	{
		if (mAllSurfaces[v.mSurfaceIndex].mSurface>= minsurf)
		{
			allSurfaces.insert(v.mSurfaceIndex);
			perVSurfaces[v.mVertex].insert(v.mSurfaceIndex);
		}
	}
	// only if at least two "bigger surfaces"
	if (allSurfaces.size() >= 2)
	{
		// retreive vertices in all available surfaces
		goodOne.Set(0.0f, 0.0f, 0.0f);
		u32 countGoodOnes = 0;
		for (const auto& v : perVSurfaces)
		{
			if (v.second.size() == allSurfaces.size())
			{
				goodOne += v.first;
				countGoodOnes++;
			}
		}

		if (countGoodOnes)
		{
			goodOne *= 1.0f / ((float)countGoodOnes);
			return true;
		}
	}


	return false;
	
}

// 
void BuildMeshFromEnveloppe::setUpEdgesInFourFreeFaceNode(const MSOctreeContent& node, std::map<std::pair<u32, u8>, std::set<u32>>& foundEdges)
{
	for (u32 i = 0; i < 6; i++)
	{
		// this face is "free"
		if (node.mData->mEmptyNeighborsFlag & (1 << i))
		{
			std::pair<u32,u8>* startP = node.mData->getVertexForFreeFace(1 << i);
			if (!startP)
			{
				printf("WTF");
			}
			for (auto adj : adjacent_faces[i / 2])
			{
				if (node.mData->mEmptyNeighborsFlag & adj) // this adjacent face is free
				{
					std::pair<u32, u8>* endP = node.mData->getVertexForFreeFace(1 << i);
					if (!endP)
					{
						printf("WTF");
					}
					foundEdges[*startP].insert(endP->first);
				}
			}
		}
	}
}

void	BuildMeshFromEnveloppe::setUpNormals()
{
	
	u32 vi = 0;
	for (auto& v :mVertices)
	{
		v.mN.Set(0.0f, 0.0f, 0.0f);
		u32 ei = (u32)(v.mEdges.size()-1);

		v3f v1;
		if (mEdges[v.mEdges[ei]].v[0] == vi)
		{
			v1.Set(mVertices[mEdges[v.mEdges[ei]].v[1]].mV - mVertices[vi].mV);
		}
		else
		{
			v1.Set(mVertices[mEdges[v.mEdges[ei]].v[0]].mV - mVertices[vi].mV);
		}
		v1.Normalize();

		v3f v2;

		for (u32 ein = 0; ein < v.mEdges.size(); ein++)
		{
			if (mEdges[v.mEdges[ein]].v[0] == vi)
			{
				v2.Set(mVertices[mEdges[v.mEdges[ein]].v[1]].mV - mVertices[vi].mV);
			}
			else
			{
				v2.Set(mVertices[mEdges[v.mEdges[ein]].v[0]].mV - mVertices[vi].mV);
			}
			v2.Normalize();

			v3f n;
			n.CrossProduct(v1, v2);
			v1 = v2;

			v.mN += n;
		}
		v.mN.Normalize();

		vi++;
	}
	
}

// for each free face, check if an edge exists with a neighbor not free face
void	BuildMeshFromEnveloppe::setUpEdges(nodeInfo node)
{
	MeshSimplificationOctreeNode* currentNode = node.getNode<MeshSimplificationOctreeNode>();
	const auto& content = currentNode->getContentType();


	std::map<std::pair<u32, u8>, std::set<u32>>	foundEdges;
	// special case when 4 vertices in the same case 
	// need to connect them
	if (content.mData->mVertices.size() == 4)
	{
		setUpEdgesInFourFreeFaceNode(content,foundEdges);
	}

	// treat each free face separately
	for (u32 i = 0; i < 6; i++)
	{
		// this face is "free"
		if (content.mData->mEmptyNeighborsFlag & (1 << i))
		{

			std::pair<u32, u8>* startP = content.mData->getVertexForFreeFace(1 << i);
			if (!startP)
			{
				printf("WTF");
			}

			for (auto adj : adjacent_faces[i / 2])
			{
				if (!(content.mData->mEmptyNeighborsFlag & adj)) // this adjacent face is not free
				{

					MeshSimplificationOctreeNode* frontNode = content.mData->getNeighbor(adj | (1 << i));
					if (frontNode)
					{
						// add edge between currentNode & frontNode
						std::pair<u32, u8>* endP = frontNode->getContentType().mData->getVertexForFreeFace(OctreeNodeBase::mOppositeFace[adj]);
						if (!endP)
						{
							printf("WTF");
						}
						foundEdges[*startP].insert(endP->first);
					}
					else
					{
						frontNode = content.mData->getNeighbor(adj);
						if (frontNode)
						{
							// add edge between currentNode & frontNode
							std::pair<u32, u8>* endP = frontNode->getContentType().mData->getVertexForFreeFace(1 << i);
							if (!endP)
							{
								printf("WTF");
							}
							foundEdges[*startP].insert(endP->first);
						}
						else
						{
							printf("WTF");
						}
					}
				}
			}
		}
	}

	if (foundEdges.size())
	{
		// add edges
		for (const auto& edges : foundEdges)
		{
			// sort end points according to angle 
			std::vector<std::pair<u32, float>>	sortedEndP;
			sortedEndP.resize(edges.second.size());
			
			v3f rotationAxis(0.0f, 0.0f, 0.0f);
			for (u32 i = 0; i < 6; i++)
			{
				if (edges.first.second & (1 << i))
				{
					v3i idirection = OctreeNodeBase::mNeightboursDecalVectors[OctreeNodeBase::mInvDir[i]];
					rotationAxis += v3f(idirection.x, idirection.y, idirection.z);
				}
			}
			rotationAxis.Normalize();

			auto pit = edges.second.begin();
			v3f	zeroAxis = mVertices[*pit].mV - mVertices[edges.first.first].mV;
			zeroAxis.Normalize();

			v3f rightAxis;
			rightAxis.CrossProduct(rotationAxis, zeroAxis);
			rightAxis.Normalize();

			zeroAxis.CrossProduct(rightAxis, rotationAxis);

			for (u32 i=0;i<edges.second.size();i++)
			{
				v3f	currentDir = mVertices[*pit].mV - mVertices[edges.first.first].mV;
				currentDir.Normalize();

				float proj1 = Dot(zeroAxis, currentDir);
				float proj2 = Dot(rightAxis, currentDir);
				float angle = atan2f(proj2, proj1);
				if (angle < 0.0f)
					angle = 2.0f * fPI + angle;

				sortedEndP[i].first = *pit;
				sortedEndP[i].second = angle;
				pit++;
			}

			if (sortedEndP.size() > 2)
			{

				std::sort(sortedEndP.begin()+1, sortedEndP.end(), [&](const std::pair<u32,float>& p1, const std::pair<u32, float>& p2)->bool {

					return p1.second < p2.second ;

						});
			}

			for (const auto& v2 : sortedEndP)
			{
				u32 edgeindex = addEdge(edges.first.first, v2.first);
				mVertices[edges.first.first].mEdges.push_back(edgeindex);
			}
		}
	}

}

// split quad faces
void	BuildMeshFromEnveloppe::splitFaces()
{
	// get size of vector now because it will grow when adding splitted faces
	u32 faceCount =(u32)( mFaces.size() );

	for (u32 fi=0;fi<faceCount;fi++)
	{
		if (mFaces[fi].edges[4] == -1) // triangle, don't need to split
		{
			continue;
		}
		MSFace& currentFace = mFaces[fi];
		v3f n[4];
		u32 ew[2];
		u32 ei[2];
		// check if quad is planar
		for (u32 eli = 0; eli < 4; eli+=2)
		{
			ew[0] = (currentFace.edges[eli] >> 31);
			ei[0] = (currentFace.edges[eli]&0x7fffffff);
			ew[1] = (currentFace.edges[eli+1] >> 31);
			ei[1] = (currentFace.edges[eli+1] & 0x7fffffff);

			v3f v1 = mVertices[mEdges[ei[0]].v[1 - ew[0]]].mV - mVertices[mEdges[ei[0]].v[ew[0]]].mV;
			v3f v2 = mVertices[mEdges[ei[1]].v[1 - ew[1]]].mV - mVertices[mEdges[ei[1]].v[ew[1]]].mV;

			v1.Normalize();
			v2.Normalize();

			n[eli / 2].CrossProduct(v1, v2);
			n[eli / 2].Normalize();
		}

		float dot = Dot(n[0], n[1])-1.0f;
		if ((dot * dot) < 0.001f)
		{
			// split quad => add edge 
			ew[0] = (currentFace.edges[0] >> 31);
			ei[0] = (currentFace.edges[0] & 0x7fffffff);
			ew[1] = (currentFace.edges[1] >> 31);
			ei[1] = (currentFace.edges[1] & 0x7fffffff);

			u32 v1 = mEdges[ei[0]].v[ew[0]];
			u32 v2 = mEdges[ei[1]].v[1-ew[1]];

			u32 edgeindex = addEdge(v1, v2);
			mVertices[v1].mEdges.push_back(edgeindex);
			mVertices[v2].mEdges.push_back(edgeindex);

			MSFace toAdd;
			toAdd.edges[0] = edgeindex;
			toAdd.edges[1] = currentFace.edges[2];
			toAdd.edges[2] = currentFace.edges[3];
			currentFace.edges[2] = edgeindex | (1 << 31);
			currentFace.edges[3] = -1;

			mFaces.push_back(toAdd);


		}
		else
		{
			// find better diagonal to split
			// first compute two other normals
			for (u32 eli = 1; eli < 4; eli += 2)
			{
				ew[0] = (currentFace.edges[eli] >> 31);
				ei[0] = (currentFace.edges[eli] & 0x7fffffff);
				ew[1] = (currentFace.edges[(eli + 1)&3] >> 31);
				ei[1] = (currentFace.edges[(eli + 1)&3] & 0x7fffffff);

				v3f v1 = mVertices[mEdges[ei[0]].v[1 - ew[0]]].mV - mVertices[mEdges[ei[0]].v[ew[0]]].mV;
				v3f v2 = mVertices[mEdges[ei[1]].v[1 - ew[1]]].mV - mVertices[mEdges[ei[1]].v[ew[1]]].mV;

				v1.Normalize();
				v2.Normalize();

				n[2+eli / 2].CrossProduct(v1, v2);
				n[2+eli / 2].Normalize();
			}
		}
	}
}

// do mesh simplification by merging triangles
void BuildMeshFromEnveloppe::mergeTriangles()
{

}

// for each free face, check if an edge exists with a neighbor not free face
void	BuildMeshFromEnveloppe::setUpFaces()
{
	int vindex = 0;

	auto edgeWay = [&](const MSEdge& e, int i)->int {if (e.v[0] == i) { return 0; } return 1; };

	// for each vertice
	for (const auto& v : mVertices)
	{
		// for each edge
		for (const auto& ei : v.mEdges)
		{
			MSEdge* e = &mEdges[ei];

			u32 ew = edgeWay(*e, vindex);

			if (e->t[ew] == -1) // not already done
			{
				MSFace toAdd;
				u32 currentEI = ei;
				u32 currentEdgeIndexInFace = 0;
				int nextVertice = e->v[1 - ew];
				int prevVertice = vindex;
				while (1)
				{
					e->t[ew] = (u32)mFaces.size();
					toAdd.edges[currentEdgeIndexInFace] = currentEI|(ew<<31);

					if (nextVertice == vindex)
						break;

					const MSVertice& destV = mVertices[nextVertice];

					// find current edge in destination vertex
					u32 nexte = destV.getEdgeIndexInThisList(prevVertice, mEdges);
					// and jump to next one
					nexte++;
					nexte = nexte % destV.mEdges.size();

					currentEI = destV.mEdges[nexte];
					e = &mEdges[currentEI];

					ew = edgeWay(*e, nextVertice);

					if (e->t[ew] != -1)
					{
						printf("WTF");
					}
					prevVertice = nextVertice;
					nextVertice = e->v[1 - ew];

					currentEdgeIndexInFace++;
				}

				mFaces.push_back(toAdd);

			}
			
			
		}
		vindex++;
	}



}

void		BuildMeshFromEnveloppe::setUpVertices(nodeInfo node)
{
	MeshSimplificationOctreeNode* currentNode = node.getNode<MeshSimplificationOctreeNode>();
	const auto& content = currentNode->getContentType();

	v3f result;
	switch (content.mData->mFreeFaceCount)
	{
	case 0:
		KIGS_ERROR("Can't occur",1);
		break;
	case 1: // easy case with only one free face
	case 3: // 3 free faces => 2 subcases corner or U 
	case 5: // 5 free faces => easy case

		if (checkUniqueSurface(node, result))
		{
			content.mData->mGoodIntersectionPoint.push_back({ addVertice(result,content.mData), content.mData->mEmptyNeighborsFlag });
		}
		else
		{
			manageDirectionalFreeFace(content);
		}
		break;
	case 2: // 2 free faces => 2 subcases for adjacent free faces or opposites
		if ((content.mData->mEmptyNeighborsFlag == 0x3) || (content.mData->mEmptyNeighborsFlag == 0xC) || (content.mData->mEmptyNeighborsFlag == 0x30))
		{
			manageSeparateFreeFaces(content);
		}
		else
		{
			if (checkUniqueSurface(node, result))
			{
				content.mData->mGoodIntersectionPoint.push_back({ addVertice(result,content.mData), content.mData->mEmptyNeighborsFlag });
			}
			else
			{
				manageDirectionalFreeFace(content);
			}
		}
		break;

	case 4: // 4 free faces => 2 subcases corner or tunnel
		if ((content.mData->mEmptyNeighborsFlag == 0xF) || (content.mData->mEmptyNeighborsFlag == 0x3C) || (content.mData->mEmptyNeighborsFlag == 0x33))
		{
			manageSeparateFreeFaces(content);
		}
		else
		{
			if (checkUniqueSurface(node, result))
			{
				content.mData->mGoodIntersectionPoint.push_back({ addVertice(result,content.mData), content.mData->mEmptyNeighborsFlag });
			}
			else
			{
				manageDirectionalFreeFace(content);
			}
		}
		break;
	case 6: 
		KIGS_ERROR("Can't occur", 1);
		break;

	}

}

v3f BuildMeshFromEnveloppe::searchGoodPointInCellForDirection(const MSOctreeContent& node, const v3f& direction)
{

	std::set<v3f>	verticeMap;
	for (const auto& v : node.mData->mVertices)
	{
		verticeMap.insert(v.mVertex);
	}

	// now put vertices and w in a vector
	std::vector<v3f> vvector;

	for (const auto& v : verticeMap)
	{
		vvector.push_back(v);
	}

	v3f up(0.0f, 0.0f, 1.0f);
	v3f right;

	if (fabsf(direction.z) > 0.7)
	{
		up.Set(1.0f, 0.0f, 0.0f);
	}
	right.CrossProduct(direction, up);
	right.Normalize();
	up.CrossProduct(right, direction);

	Matrix3x3 transform;
	transform.Axis[0] = direction;
	transform.Axis[1] = up;
	transform.Axis[2] = right;
	transform=Transpose(transform);
	// then sort vector according to direction

	std::sort(vvector.begin(), vvector.end(), [direction](const v3f& a, const v3f& b)->bool {

		float vala = Dot(direction, a);
		float valb = Dot(direction, b);


		return vala<valb; });


	BBox2D	currentBBox;
	currentBBox.SetEmpty();
	v3f		result(0.0f,0.0f,0.0f);

	std::vector<v3f> transformedV = vvector;

	transform.TransformVectors((Vector3D*)&(transformedV[0]), (int)transformedV.size());

	// first pass, compute full bbox surface
	for (u32 i = 0; i < vvector.size(); i++)
	{
		currentBBox.Update(transformedV[i].yz);
	}
	float surfmax = (currentBBox.m_Max.x - currentBBox.m_Min.x) * (currentBBox.m_Max.y - currentBBox.m_Min.y);

	currentBBox.SetEmpty();
	for (u32 i=0;i<vvector.size();i++)
	{
		currentBBox.Update(transformedV[i].yz);
		result += vvector[i];
		float surf = (currentBBox.m_Max.x - currentBBox.m_Min.x) * (currentBBox.m_Max.y - currentBBox.m_Min.y);
		if (surf > surfmax*0.25f)
		{
			result *= (1.0f / (i + 1.0f));
			return result;
		}
	}

	result*= (1.0f / (float)(vvector.size()));

	return result;
}


// all cases
void BuildMeshFromEnveloppe::manageDirectionalFreeFace(const MSOctreeContent& node)
{
	v3f direction(0.0f, 0.0f, 0.0f);

	for (u32 i = 0; i < 6; i++)
	{
		if (node.mData->mEmptyNeighborsFlag & (1 << i))
		{
			v3i idirection = OctreeNodeBase::mNeightboursDecalVectors[OctreeNodeBase::mInvDir[i]];
			direction += v3f(idirection.x, idirection.y, idirection.z);
		}
	}

	direction.Normalize();

	v3f pos= searchGoodPointInCellForDirection(node, direction);
	node.mData->mGoodIntersectionPoint.push_back({ addVertice(pos,node.mData), node.mData->mEmptyNeighborsFlag });
}
void BuildMeshFromEnveloppe::manageSeparateFreeFaces(const MSOctreeContent& node)
{
	v3f direction(0.0f, 0.0f, 0.0f);

	for (u32 i = 0; i < 6; i++)
	{
		if (node.mData->mEmptyNeighborsFlag & (1 << i))
		{
			v3i idirection = OctreeNodeBase::mNeightboursDecalVectors[OctreeNodeBase::mInvDir[i]];
			direction = v3f(idirection.x, idirection.y, idirection.z);

			v3f pos = searchGoodPointInCellForDirection(node, direction);
			node.mData->mGoodIntersectionPoint.push_back({ addVertice(pos,node.mData),  (1 << i) });
		}
	}
}


void BuildMeshFromEnveloppe::Build()
{

	// first setup all vertices in enveloppe nodes
	for (const auto& n : mNodeList)
	{
		setUpVertices(n);
	}

	// then setup edges for each nodes
	for (const auto& n : mNodeList)
	{
		setUpEdges(n);
	}
	// for each vertice, compute normals using edge list
	setUpNormals();
	
	// setup faces
	setUpFaces();
	
	// split quad faces
	splitFaces();

	// do mesh simplification by merging triangles
	mergeTriangles();
}

float	CollisionMeshSimplification::distFromPlane(const MSPlaneStruct& p, const v3f& P1)
{
	return Dot(p.mNormal, P1) + p.mDist;
}

void CollisionMeshSimplification::segmentPlaneIntersection(const v3f& P1, const v3f& P2, const MSPlaneStruct& p, std::set<v3f>& outSegTips)
{
	const float eps = 0.000001f;

	float	d1 = distFromPlane(p,P1),
			d2 = distFromPlane(p,P2);

	bool  bP1OnPlane = (abs(d1) < eps),
		bP2OnPlane = (abs(d2) < eps);

	if (bP1OnPlane)
		outSegTips.insert(P1);

	if (bP2OnPlane)
		outSegTips.insert(P2);

	if (bP1OnPlane && bP2OnPlane)
		return;

	if (d1 * d2 > eps)  // points on the same side of plane
		return;

	if (abs(d1 - d2) < eps)
		return;

	float t = d1 / (d1 - d2); // 'time' of intersection point on the segment

	v3f out = P1 + t * (P2 - P1);

	outSegTips.insert(out);
}

void CollisionMeshSimplification::trianglePlaneIntersection(const v3f& triA, const v3f& triB, const v3f& triC, const MSPlaneStruct& p, std::set<v3f>& outSegTips)
{
	segmentPlaneIntersection(triA, triB, p, outSegTips);
	segmentPlaneIntersection(triB, triC, p, outSegTips);
	segmentPlaneIntersection(triC, triA, p, outSegTips);
}

bool operator<(const v3i& U, const v3i& V)
{
	if (U.x == V.x)
	{
		if (U.y == V.y)
		{
			if (U.z == V.z)
			{
				return false;
			}
			return U.z < V.z;
		}
		return U.y < V.y;
	}
	return U.x < V.x;
}

void MeshSimplificationOctreeNode::checkEmptyNeighbors(MeshSimplificationOctree& octree, nodeInfo& node)
{
	
	u32 currentBrowsingFlag = getBrowsingFlag();

	// 00 =>  0
	// 01 => -1
	// 10 =>  1
	const u32 mask[3] = { 1,0,2 };
	const int mainDirFromDirMask[33] = {-1,0,1,-1,2,-1,-1,-1,3,
										-1,-1,-1,-1,-1,-1,-1,4,
										-1,-1,-1,-1,-1,-1,-1,-1,
										-1,-1,-1,-1,-1,-1,-1,5
	};

	node.getNode<MeshSimplificationOctreeNode>()->getContentType().mData->mNeighbors.clear();
	// check all 26 neighbors
	for (s32 axisX = 0; axisX < 3; axisX++)
	{
		for (s32 axisY = 0; axisY < 3; axisY++)
		{
			for (s32 axisZ = 0; axisZ < 3; axisZ++)
			{
				u32 dirmask = mask[axisX]|(mask[axisY]<<2) | (mask[axisZ] << 4);
				if (dirmask)
				{
					nodeInfo	n = octree.getVoxelNeighbour(node, dirmask);
					u32 maindir = (axisX & 1) + (axisY & 1) + (axisZ & 1);
					if (maindir == 2) // main axis
					{
						// browsing flag set to 2 means node is outside the object (set when flood fill empty)
						if ((n.node->getBrowsingFlag() == 2) && (n.node->isEmpty()))
						{
							node.getNode<MeshSimplificationOctreeNode>()->getContentType().mData->mEmptyNeighborsFlag |= 1 << (u32)mainDirFromDirMask[dirmask];
							node.getNode<MeshSimplificationOctreeNode>()->getContentType().mData->mFreeFaceCount++;
						}
					}

					if (n.node->getBrowsingFlag() == currentBrowsingFlag) // same group
					{
						if (n.getNode<MeshSimplificationOctreeNode>()->getContentType().mData) // not empty
						{
							if (n.getNode<MeshSimplificationOctreeNode>()->getContentType().mData->mOtherFlags & 1) // enveloppe
							{
								node.getNode<MeshSimplificationOctreeNode>()->getContentType().mData->mNeighbors.push_back({ dirmask ,n.getNode<MeshSimplificationOctreeNode>() });
							}
							else
							{
								node.getNode<MeshSimplificationOctreeNode>()->getContentType().mData->mNeighbors.push_back({ dirmask ,nullptr }); // this node is inside the object
							}
						}
					}

				}
			}
		}
	}

}
