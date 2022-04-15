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

	while (poweroftwodecal > mMaxOctreeDepth)
	{
		poweroftwodecal--;
		precision *= 2.0f;
	}
}


void	MeshSimplification::initOctree()
{

	// compute BBox of object
	BBox	b;
	b.SetEmpty();
	for (const auto& p : mInputVertices)
	{
		b.Update(p);
	}

	adjustPrecision(b,mPrecision);
#ifdef _DEBUG
	printf("adjusted precision : %f\n", mPrecision);
#endif

	// now compute best switch to apply to avoid segment on octree cutting planes
	mOctreeShift.Set(0.0f, 0.0f, 0.0f);

	{
		float oneOnP = 1.0f / mPrecision;
		std::vector<float>	switches[3];
		
		float dontcare;
		for (const auto& p : mInputVertices)
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
		mObjectShift *= mPrecision;
	}

	// create octree with previous precisop, and shift
	mOctree = KigsCore::GetInstanceOf("localOctree", "MeshSimplificationOctree");
	mOctree->setValue("Precision", mPrecision);
	mOctree->setValue("Translate", mOctreeShift);
	mOctree->setBBox(b);
	mOctree->Init();

	mOctree->setVertexList(mInputVertices);

	for (u32 grpIndex = 0; grpIndex < mInputIndices.size(); grpIndex++)
	{
		auto& currentGrp = mInputIndices[grpIndex];
		// then rasterize all triangles in octree
		for (int i = 0; i < currentGrp.mIndices.size(); i += 3) // send each triangles
		{
			mOctree->setVoxelContent(currentGrp.mIndices[i], currentGrp.mIndices[i + 1], currentGrp.mIndices[i + 2], grpIndex);
		}
	}

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

	// clear useless data if not envelope cell (free some memory)
	{

		auto clearDataIfNotEnveloppe = [](OctreeNodeBase* currentNode)->void
		{
			auto contentdata = static_cast<MeshSimplificationOctreeNode*>(currentNode)->getContentType().mData;
			if(contentdata)
			if (contentdata->mOtherFlags != 1)
			{
				contentdata->mVertices.clear();
			}
			
		};

		MeshSimplificationOctree::applyOnAllNodes clearData(*mOctree, clearDataIfNotEnveloppe);
	}

	// check for empty neighbor for each node and set neighbors
	for (auto& n : mEnvelopenodelist)
	{
		n.getNode<MeshSimplificationOctreeNode>()->getContentType().mData->initEnvelopeData();
		n.getNode<MeshSimplificationOctreeNode>()->checkEmptyNeighbors(*mOctree, n);
	}

}

MeshSimplification::MeshSimplification(const std::vector<v3f>& vertices, float precision, u32 maxOctreeDepth):mMaxOctreeDepth(maxOctreeDepth), mInputVertices(vertices), mPrecision(precision)
{

}
void	MeshSimplification::addTriangleGroup(const std::vector<u32>& indices, u32 groupIndex)
{
	mInputIndices.resize(mInputIndices.size() + 1);
	mInputIndices.back().mGroupIndex = groupIndex;
	mInputIndices.back().mIndices = indices;
}

void	MeshSimplification::doSimplification()
{
	initOctree();

	mFinalVertices.clear();
	mFinalIndices.clear();
	mFinalIndices.resize(mInputIndices.size()); // same group count as input
	for (u32 i = 0; i < mGroupCount; i++)
	{
		rebuildMesh(i, mEnvelopenodelist);
	}
	mOctree->transformBackVertexList(mFinalVertices);
}

// for collisions, normals and materials does not matter
MeshSimplification::MeshSimplification(const std::vector<u32>& indices, const std::vector<v3f>& vertices,float precision , u32 maxOctreeDepth) :mMaxOctreeDepth(maxOctreeDepth), mInputVertices(vertices), mPrecision(precision)
{
	addTriangleGroup(indices, 0);

	initOctree();
	
	mFinalVertices.clear();
	mFinalIndices.clear();
	mFinalIndices.resize(mInputIndices.size()); // same group count as input (here only 1 group)
	for (u32 i = 0; i < mGroupCount; i++)
	{
		rebuildMesh(i, mEnvelopenodelist);
	}
	mOctree->transformBackVertexList(mFinalVertices);
}

void MeshSimplification::rebuildMesh(u32 groupIndex, std::vector<nodeInfo>& envelopenodes)
{
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
	BuildMeshFromEnveloppe* meshBuilder = new BuildMeshFromEnveloppe(thisGroupEnvelopeNodes,mOctree->mTriangleInfos);
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
				if (vtoadd.V == toAdd.edges[e & 0x7fffffff].first.first)
				{
					vtoadd.N.push_back(toAdd.edges[e & 0x7fffffff].first.second);
				}
				else
				{
					vtoadd.N.push_back(toAdd.edges[e & 0x7fffffff].first.first);
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
std::vector<std::pair<std::pair<v3f, v3f>,u32>>	MeshSimplification::getEdges() const
{
	std::vector<std::pair<std::pair<v3f, v3f>, u32>> result;
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
