#include "OctreeMeshSimplification.h"
#include "MeshSimplification.h"

v3f	MSPlaneStruct::mAxisNormal[3] = { {1.0f,0.0f,0.0f},{0.0f,1.0f,0.0f},{0.0f,0.0f,1.0f} };

void	MSOctreeContent::ContentData::initEnvelopeData()
{
	if (mEnvelopeData)
		return;
	mEnvelopeData = new additionnalEnvelopeData();
}

std::vector<std::pair<u32, float>>	MSOctreeContent::ContentData::getPrincipalSurfacesForDirection(const nodeInfo& n, const v3f& dir, const std::vector<MSSurfaceStruct>& Surfaces)
{
	v3f refPoint(n.coord.x, n.coord.y, n.coord.z);
	refPoint *= 0.5f;
	refPoint -= dir;

	std::vector<std::pair<u32, float>> oksurfs;

	u32 s = 0;
	for (const auto& surfInfo : Surfaces)
	{
		float dot = (Dot(dir, surfInfo.mPlane.mNormal));
		float d = fabsf(dot);
		if (d >= 0.7071f)
		{
			d *= surfInfo.mSurface;

			v3f p0 = surfInfo.mInOctreePlaneP0;
			float dist = 1.0f + Dot(p0 - refPoint, surfInfo.mPlane.mNormal) / dot;
			d /= dist;

			// here I should just reject too low score
			oksurfs.push_back({ s,d });
		}
		s++;

	}

	if (oksurfs.size() == 0)
		return oksurfs;

	std::sort(oksurfs.begin(), oksurfs.end(), [&](const std::pair<u32, float>& p1, const std::pair<u32, float>& p2)->bool {return p1.second > p2.second;	});

	return oksurfs;
}


IMPLEMENT_CLASS_INFO(MeshSimplificationOctree);

IMPLEMENT_CONSTRUCTOR(MeshSimplificationOctree)
{
	mRootNode = new MeshSimplificationOctreeNode();
}

// set vertex list and transform it in octree coordinate
void			MeshSimplificationOctree::setVertexList(const std::vector<v3f>& vlist)
{
	mIOCVertices = vlist;
	for (auto& v : mIOCVertices)
	{
		TransformInOctreeCoord(v);
	}
}

void			MeshSimplificationOctree::transformBackVertexList(std::vector<v3f>& vlist)
{
	for (auto& v : vlist)
	{
		v -= mTranslate;
		v *= mPrecision;
		v += mBBox.m_Min;
	}
}

// utility class to iterate in rastered poly cell arrays
class doubleMapIter
{
protected:

	std::map<float, std::map<float, std::set<v3f>>>::const_iterator	mFirstAxisIterators[4];
	std::map<float, std::set<v3f>>									mSecondAxisMap;
	std::map<float, std::set<v3f>>::const_iterator					mSecondAxisIterators[4];

	const std::map<float, std::map<float, std::set<v3f>>>& mDataStruct;

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
				const auto& v = (*mFirstAxisIterators[1]).second;

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

void MeshSimplificationOctree::trianglePlaneIntersection(const v3f& triA, const v3f& triB, const v3f& triC, const MSPlaneStruct& p, std::set<v3f>& outSegTips)
{
	segmentPlaneIntersection(triA, triB, p, outSegTips);
	segmentPlaneIntersection(triB, triC, p, outSegTips);
	segmentPlaneIntersection(triC, triA, p, outSegTips);
}
float	MeshSimplificationOctree::distFromPlane(const MSPlaneStruct& p, const v3f& P1)
{
	return Dot(p.mNormal, P1) + p.mDist;
}

void MeshSimplificationOctree::segmentPlaneIntersection(const v3f& P1, const v3f& P2, const MSPlaneStruct& p, std::set<v3f>& outSegTips)
{
	const float eps = 0.000001f;

	float	d1 = distFromPlane(p, P1),
		d2 = distFromPlane(p, P2);

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

u32 MeshSimplificationOctree::computeTriangleInfos(u32 P1, u32 P2, u32 P3)
{
	v3f	u(mIOCVertices[P2] - mIOCVertices[P1]);
	v3f	v(mIOCVertices[P3] - mIOCVertices[P1]);

	v3f c;
	c.CrossProduct(u, v);

	MSTriangleInfo	toAdd;
	toAdd.mNormal = c;
	float norm = Norm(toAdd.mNormal);
	if (norm == 0.0f)	// bad triangle
	{
		return -1;
	}
	toAdd.mNormal /= norm;
	toAdd.mSurface = norm * 0.5f;

	mTriangleInfos.push_back(toAdd);

	return mTriangleInfos.size() - 1;
}



// rasterize a triangle and put it in the octree
void			MeshSimplificationOctree::setVoxelContent(u32 P1, u32 P2, u32 P3, u32 groupIndex)
{

	v3f*	octreeCoords[3];

	octreeCoords[0] = &mIOCVertices[P1];
	octreeCoords[1] = &mIOCVertices[P2];
	octreeCoords[2] = &mIOCVertices[P3];

	BBox	triangleBBox(mIOCVertices[P1]);
	triangleBBox.Update(mIOCVertices[P2]);
	triangleBBox.Update(mIOCVertices[P3]);

	u32 triangleindex = computeTriangleInfos(P1,P2,P3);
	if (triangleindex == -1) // flat triangle
		return;

	// get triangle normal
	const v3f& triangleNormal = mTriangleInfos[triangleindex].mNormal;
	float inOctreePlanedist = Dot(triangleNormal, mIOCVertices[P1]);

	// precomputed "radius" for cell/plane intersection test
	float precompR = (0.5f * (fabsf(triangleNormal.x) + fabsf(triangleNormal.y) + fabsf(triangleNormal.z)));

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

	std::map<float, std::map<float, std::set<v3f>>> intersectionStruct;

	int proj2DAxis1 = coordsIndex[1];
	int proj2DAxis2 = coordsIndex[2];

	// first, add triangle vertices
	for (int pts = 0; pts < 3; pts++)
	{
		intersectionStruct[(*(octreeCoords[pts]))[proj2DAxis1]][(*(octreeCoords[pts]))[proj2DAxis2]].insert((*(octreeCoords[pts])));
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

		for (int pos1 = planestart + 1; pos1 < planeend; pos1++)
		{

			MSPlaneStruct p;
			p.mNormal = MSPlaneStruct::mAxisNormal[planeIndex];
			p.mDist = -pos1;
			std::set<v3f>	segments;

			trianglePlaneIntersection(mIOCVertices[P1], mIOCVertices[P2], mIOCVertices[P3], p, segments);

			for (auto& pt : segments)
			{
				v3f roundproblems(pt);
				roundproblems[planeIndex] = (float)pos1;
				intersectionStruct[roundproblems[proj2DAxis1]][roundproblems[proj2DAxis2]].insert(roundproblems);
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
						newintersection[secondplane] = (float)pos2; // avoid rounding problemns
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

					// check if triangleNormal is near axis aligned
					float fabscomp = fabsf(triangleNormal[projplaneindex]);
					if (fabscomp < 0.85f) // if not axis aligned, then check if triangle intersect cell
					{
						if (!nodeToFill->intersectSurface(octreeposfloat, precompR, inOctreePlanedist, triangleNormal))
						{
							continue;
						}
					}

					for (const auto& p : allptsInSquare)
					{
						if ((p[projplaneindex] >= thirdAxisFloat) && (p[projplaneindex] <= (thirdAxisFloat + 1.0f)))
						{
							nodeToFill->setContent(p, groupIndex,triangleindex);
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



MeshSimplificationOctreeNode* MeshSimplificationOctree::getFinalNodeAtPos(MeshSimplificationOctreeNode* currentNode, const v3i& pos, int currentDepth)
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
