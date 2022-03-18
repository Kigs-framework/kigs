#include "MeshSimplification.h"
//#include "SimpleShapeMeshCollider.h"
#include <math.h>
#include <map>
#include "TecLibs/2D/BBox2D.h"
#include "BuildMeshFromEnveloppe.h"

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


u32			MeshSimplification::surfaceListBuilder::insertSurface(const MSSurfaceStruct& toadd)
{
	int mapIndex = (toadd.mPlane.mDist * mOneOnPrecision);

	int indexes[3] = { mapIndex,mapIndex - 1,mapIndex + 1 };

	for (size_t i = 0; i < 3; i++)
	{
		for (auto si : mConstructionMap[indexes[i]])
		{
			auto& s = mParent.mAllSurfaces[si];
			if (fabsf(s.mPlane.mDist - toadd.mPlane.mDist) < 0.001)
			{
				if (NormSquare(s.mPlane.mNormal - toadd.mPlane.mNormal) < 0.001)
				{
					s.mSurface += toadd.mSurface;
					return si;
				}
			}
		}
	}

	mParent.mAllSurfaces.push_back(toadd);

	u32 newIndex = (u32)(mParent.mAllSurfaces.size() - 1);

	mConstructionMap[mapIndex].push_back(newIndex);

	return newIndex;
	
}

void	MeshSimplification::surfaceListBuilder::build()
{
	// compute all planes in the mesh
	// surfaces are not in octree coordinate system

	for (int i = 0; i < mIndices.size(); i += 3) // send each triangles
	{
		v3f	u(mVertices[mIndices[i + 1]] - mVertices[mIndices[i]]);
		v3f	v(mVertices[mIndices[i + 2]] - mVertices[mIndices[i]]);

		v3f c;
		c.CrossProduct(u, v);

		MSSurfaceStruct toAdd;
		toAdd.mPlane.mNormal = c;
		float norm = Norm(toAdd.mPlane.mNormal);
		if (norm == 0.0f)	// bad triangle
		{
			mParent.mTriangleSurfaceIndex.push_back(0);
			continue;
		}
		toAdd.mPlane.mNormal /= norm;

		// take object shift into account
		toAdd.mPlane.mDist = Dot(mVertices[mIndices[i]] + mParent.mObjectShift, toAdd.mPlane.mNormal);
		toAdd.mSurface = norm * 0.5f;

		mParent.mTriangleSurfaceIndex.push_back(insertSurface(toAdd));
	}
	// finish surface setup
	for (auto& s : mParent.mAllSurfaces)
	{
		s.mInOctreePlaneP0 = s.mPlane.mDist * s.mPlane.mNormal;
		s.mInOctreePlaneP0 -= mParent.mOctree->mBBox.m_Min;
		s.mInOctreePlaneP0 *= mParent.mOctree->mBBoxCoef;
	}
}


void		MeshSimplification::computeSurfaceList(const std::vector<u32>& indices, const std::vector<v3f>& vertices, float precision)
{
	surfaceListBuilder doIt(indices, vertices, precision,*this);
	mAllSurfaces.clear();

	// add invalid one at first pos
	MSSurfaceStruct toAdd;
	toAdd.mSurface = 0.0;
	toAdd.mPlane.mNormal.Set(0.0f, 0.0f, 0.0f);

	mAllSurfaces.push_back(toAdd);

	doIt.build();
}

// if precision gives a too deep octree, then change precision
void	MeshSimplification::adjustPrecision(const BBox& bbox, float& precision)
{
	v3f edges(bbox.m_Max);
	edges -= bbox.m_Min;

	float maxedge = edges.x;
	if (maxedge < edges.y)
		maxedge = edges.y;
	if (maxedge < edges.z)
		maxedge = edges.z;

	maxedge += 4.0f * precision;

	float subdivision = maxedge / precision;
	int poweroftwodecal = 0;
	while ((1 << poweroftwodecal) < subdivision)
	{
		poweroftwodecal++;
	}

	while (poweroftwodecal > 8)
	{
		poweroftwodecal--;
		precision *= 2.0f;
	}
}


void	MeshSimplification::initOctree(const std::vector<u32>& indices, const std::vector<v3f>& vertices, float precision)
{

	// compute BBox of object
	BBox	b;
	b.SetEmpty();
	for (const auto& p : vertices)
	{
		b.Update(p);
	}

	adjustPrecision(b,precision);

	// now compute best switch to apply to avoid segment on octree cutting planes
	mOctreeShift.Set(0.0f, 0.0f, 0.0f);

	{
		float oneOnP = 1.0f / precision;
		std::vector<float>	switches[3];
		
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

			// don't take exactly the middle of the bestd so try to avoid 45° edges also
			mOctreeShift[dim] = 1.0f - (switches[dim][besti] + bestd * 0.47f);

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

	// compute all surfaces in object
	computeSurfaceList(indices, vertices,precision);
	mOctree->setSurfaceList(mAllSurfaces);


	mOctree->setVertexList(vertices);
	// then rasterize all triangles in octree
	u32 triangleIndex = 0;
	for (int i = 0; i < indices.size(); i += 3) // send each triangles
	{
		if (mTriangleSurfaceIndex[triangleIndex] != 0) // only valid triangles
		{
			mOctree->setVoxelContent(indices[i], indices[i + 1], indices[i + 2], mTriangleSurfaceIndex[triangleIndex]);
		}
		triangleIndex++;
	}

	// TODO
	// probably a way here to merge cells (not border cells where all 8 sons contains only 1 unique surface or are empty )


	// fill empty cases outside of the object
	// empty cases will be "flaged" with 2
	// retrieve envelope cell list
	mEnvelopenodelist = mOctree->floodFillEmpty(2);


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
		for (auto& n : mEnvelopenodelist)
		{
			if (n.node->getBrowsingFlag() == 0)
			{
				// each group will be flagged with 4,6,8...
				mOctree->floodFillWithCondition(n, condition, (mGroupCount + 2) * 2);
				mGroupCount++;
			}
		}

		// now tag envelope nodes
		for (auto& n : mEnvelopenodelist)
		{
			n.getNode<MeshSimplificationOctreeNode>()->getContentType().mData->mOtherFlags = 1;
		}
	}

	// check for empty neighbor for each node and set neighbors
	for (auto& n : mEnvelopenodelist)
	{
		n.getNode<MeshSimplificationOctreeNode>()->getContentType().mData->initEnvelopeData();
		n.getNode<MeshSimplificationOctreeNode>()->checkEmptyNeighbors(*mOctree, n);
	}

}

// for collisions, normals and materials does not matter
MeshSimplification::MeshSimplification(const std::vector<u32>& indices, const std::vector<v3f>& vertices,float precision )
{
	initOctree(indices, vertices, precision);
	
	mFinalVertices.clear();
	mFinalIndices.clear();
	for (u32 i = 0; i < mGroupCount; i++)
	{
		rebuildMesh(i, mEnvelopenodelist);
	}
	mOctree->transformBackVertexList(mFinalVertices);
}

void MeshSimplification::rebuildMesh(u32 groupIndex, std::vector<nodeInfo>& envelopenodes)
{
	if (groupIndex != 6)
		return;

	groupIndex += 2;
	groupIndex *= 2;

	// create node list for this group
	std::vector<nodeInfo*> thisGroupEnvelopeNodes;

	for (u32 i = 0; i < envelopenodes.size(); i++)
	{
		auto& n = envelopenodes[i];
		if (n.getNode<MeshSimplificationOctreeNode>()->getBrowsingFlag() == groupIndex) // one group at a time
		{
			thisGroupEnvelopeNodes.push_back(&n);
		}
	}

	if (thisGroupEnvelopeNodes.size() <= 2) // not enough nodes
	{
		return;
	}

	// ok, let's build a mesh
	BuildMeshFromEnveloppe* meshBuilder = new BuildMeshFromEnveloppe(thisGroupEnvelopeNodes,mAllSurfaces);
	meshBuilder->Build();
#ifdef _DEBUG
	enveloppeMesh toAdd;
	auto vertices= meshBuilder->getEnveloppeVertices();
	toAdd.edges = meshBuilder->getEdges();
	for (auto& v : vertices)
	{
		vAndN	vtoadd;
		vtoadd.V = v.mV;

		if (v.mFlag <= 8)
		{
			for (auto e : v.mEdges)
			{
				if (vtoadd.V == toAdd.edges[e & 0x7fffffff].first)
				{
					vtoadd.N.push_back(toAdd.edges[e & 0x7fffffff].second);
				}
				else
				{
					vtoadd.N.push_back(toAdd.edges[e & 0x7fffffff].first);
				}
			}
		}
		toAdd.vertices.push_back(vtoadd);
	}

	mMeshes.push_back(toAdd);
#endif

	meshBuilder->addFinalizedMesh(mFinalVertices, mFinalIndices);

	delete meshBuilder;
}

#ifdef _DEBUG
std::vector<std::pair<v3f, v3f>>	MeshSimplification::getEdges() const
{
	std::vector<std::pair<v3f, v3f>> result;
	for (const auto& m : mMeshes)
	{
		result.insert(result.end(), m.edges.begin(), m.edges.end());
	}
	return result;
}

std::vector<MeshSimplification::vAndN>					MeshSimplification::getEnveloppeVertices() const
{
	/*if (mMeshBuilder)
	{
		return mMeshBuilder->getEnveloppeVertices();
	}*/
	std::vector<MeshSimplification::vAndN> result;
	for (const auto& m : mMeshes)
	{
		result.insert(result.end(), m.vertices.begin(), m.vertices.end());
	}
	return result;
}
#endif

MeshSimplification::~MeshSimplification()
{
	
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

	node.getNode<MeshSimplificationOctreeNode>()->getContentType().mData->mEnvelopeData->mNeighbors.clear();
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
					if (n.node)
					{
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
									node.getNode<MeshSimplificationOctreeNode>()->getContentType().mData->mEnvelopeData->mNeighbors.push_back({ dirmask ,n.getNode<MeshSimplificationOctreeNode>() });
								}
								else
								{
									node.getNode<MeshSimplificationOctreeNode>()->getContentType().mData->mEnvelopeData->mNeighbors.push_back({ dirmask ,nullptr }); // this node is inside the object
								}
							}
						}
					}
				}
			}
		}
	}

}
