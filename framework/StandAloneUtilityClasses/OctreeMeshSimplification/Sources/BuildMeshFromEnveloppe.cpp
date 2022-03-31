#include "BuildMeshFromEnveloppe.h"
#include "TecLibs/2D/BBox2D.h"
#include "TecLibs/Geometrie/ConvexHull2D.h"

unsigned int adjacent_faces[6][4] = { {8,32,4,16},{8,16,4,32},{2,16,1,32},{2,32,1,16},{8,1,4,2},{8,2,4,1} };


bool	BuildMeshFromEnveloppe::computeVerticeFromCell(nodeInfo node, v3f& goodOne)
{
	MeshSimplificationOctreeNode* currentNode = node.getNode<MeshSimplificationOctreeNode>();
	const auto& content = currentNode->getContentType();

#ifdef _DEBUG
	if (content.mData->mFreeFaceCount > 3)
	{
		// should not be there
		return false;
	}
#endif

	v3f direction = {0.0f,0.0f,0.0f};

	BBox	cellBBox(BBox::PreInit{});
	for (u32 i = 0; i < 6; i++)
	{
		// this face is "free"
		if (content.mData->mEmptyNeighborsFlag & (1 << i))
		{
			v3i idirection = OctreeNodeBase::mNeightboursDecalVectors[OctreeNodeBase::mInvDir[i]];
			direction += idirection;
		}
		else
		{
			//cellBBox.Update(OctreeNodeBase::mNeightboursDecalVectors[i] * 0.5f);
		}
	}
	direction.Normalize();

	goodOne=searchGoodVerticeInCellForDirection(node, content, direction, cellBBox);

	return true;
}

bool	BuildMeshFromEnveloppe::checkVerticeEasyCase(nodeInfo node, v3f& goodOne)
{
	MeshSimplificationOctreeNode* currentNode = node.getNode<MeshSimplificationOctreeNode>();
	const auto& content = currentNode->getContentType();

	// this test doesn't work well when only 1 free face
	if (content.mData->mFreeFaceCount == 1)
		return false;

	// get principal surfaces for each free face, then check if valid points belonging to all those surfaces can be found
	// key : surface index, value = <freeface index , score >
	std::map<u32, std::pair<u32, float>>	surfaceMap;
	u32 foundSurfaceCount = 0;
	u32 freeFaceIndex = 0;
	u32 surfaceMask = 0;
	for (u32 i = 0; i < 6; i++)
	{
		// this face is "free"
		if (content.mData->mEmptyNeighborsFlag & (1 << i))
		{
			v3i idirection = OctreeNodeBase::mNeightboursDecalVectors[OctreeNodeBase::mInvDir[i]];
			v3f direction(idirection.x, idirection.y, idirection.z);
			std::vector<std::pair<u32, float>> surf = content.mData->getPrincipalSurfacesForDirection(node, direction, mCellData.mCellSurfaces);
			if (surf.size())
			{
				foundSurfaceCount++;
				surfaceMask |= (1 << freeFaceIndex);
				for (const auto& s : surf)
				{
					surfaceMap[s.first] = { freeFaceIndex,s.second };
				}
			}
			freeFaceIndex++;
		}
	}

	// this test doesn't work well when only 1 free face
	if (foundSurfaceCount == 1)
		return false;

	std::vector<std::pair<v3f, float>> bestfoundv;
	// mark potentially valid vertices with scores
	for (const auto& v : mCellData.mPerVSurfaces)
	{
		if (v.second.size() >= foundSurfaceCount)
		{
			std::vector<float> scores;
			scores.resize(freeFaceIndex, 0.0f);
			u32   currentmask = 0;
			for (const auto& s : v.second)
			{
				const auto& found = surfaceMap.find(s);
				if (found != surfaceMap.end())
				{
					if ((currentmask & (1 << (*found).second.first)) == 0)
					{
						currentmask |= 1 << (*found).second.first;
						scores[(*found).second.first] = (*found).second.second;
					}
					else if (scores[(*found).second.first] < (*found).second.second)
					{
						scores[(*found).second.first] = (*found).second.second;
					}
				}
			}
			if (currentmask == surfaceMask)
			{
				float score = 1.0f;
				for (auto s : scores)
				{
					if (s)
					{
						score *= s;
					}
				}
				bestfoundv.push_back({ v.first,score });
			}
		}
	}

	if (bestfoundv.size() == 0)
	{
		return false;
	}

	std::sort(bestfoundv.begin(), bestfoundv.end(), [](const std::pair<v3f, float>& p1, const std::pair<v3f, float>& p2)->bool {
		return p1.second > p2.second;	});


	// retreive vertices at intersection of found surfaces
	goodOne.Set(0.0f, 0.0f, 0.0f);
	u32 countGoodOnes = 0;
	float bestScore = bestfoundv[0].second;

	for (const auto& v : bestfoundv)
	{
		if (v.second == bestScore)
		{
			goodOne += v.first;
			countGoodOnes++;
		}
	}

	if (countGoodOnes) // Standard case vertices were found
	{
		goodOne *= 1.0f / ((float)countGoodOnes);
		return true;
	}

	return false;
}

bool	BuildMeshFromEnveloppe::checkVerticeTrivialCase(nodeInfo node, v3f& goodOne)
{
	MeshSimplificationOctreeNode* currentNode = node.getNode<MeshSimplificationOctreeNode>();
	const auto& content = currentNode->getContentType();

	// if more surfaces than free face count, then this is a more complex case
	if (mCellData.mCellSurfaces.size() > content.mData->mFreeFaceCount)
	{
		return false;
	}

	// retreive vertices in all available surfaces
	goodOne.Set(0.0f, 0.0f, 0.0f);
	u32 countGoodOnes = 0;
	for (const auto& v : mCellData.mPerVSurfaces)
	{
		if (v.second.size() == mCellData.mCellSurfaces.size()) // this vertice belongs to all the surfaces 
		{
			goodOne += v.first;
			countGoodOnes++;
		}
	}

	if (countGoodOnes) // Standard case vertices were found
	{
		goodOne *= 1.0f / ((float)countGoodOnes);
		return true;
	}

	return false;

}

void	BuildMeshFromEnveloppe::setUpNormals()
{

	u32 vi = 0;
	for (auto& v : mVertices)
	{
		v.mN.Set(0.0f, 0.0f, 0.0f);
		u32 ei = (u32)(v.mEdges.size() - 1);
		u32 ew = (v.mEdges[ei] >> 31);
		ei = v.mEdges[ei] & 0x7fffffff;
			
		v3f v1(mVertices[mEdges[ei].v[1-ew]].mV - mVertices[vi].mV);
		v1.Normalize();

		v3f v2;

		for (u32 ein = 0; ein < v.mEdges.size(); ein++)
		{
			ew = (v.mEdges[ein] >> 31);
			ei = v.mEdges[ein] & 0x7fffffff;
			
			v2.Set(mVertices[mEdges[ei].v[1-ew]].mV - mVertices[vi].mV);
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

std::vector<BuildMeshFromEnveloppe::MSVertice>					BuildMeshFromEnveloppe::getEnveloppeVertices() const
{
	return mVertices;
}

std::vector<std::pair<v3f, v3f>>	BuildMeshFromEnveloppe::getEdges() const
{
	std::vector<std::pair<v3f, v3f>> edgelist;

	for (const auto& e : mEdges)
	{
		edgelist.push_back({ mVertices[e.v[0]].mV, mVertices[e.v[1]].mV });
	}
	return edgelist;
}

void	BuildMeshFromEnveloppe::setUpEdges(nodeInfo node, std::map< u32, std::vector<u32>>& edgeMap)
{
	MeshSimplificationOctreeNode* currentNode = node.getNode<MeshSimplificationOctreeNode>();
	const auto& content = currentNode->getContentType();

	// key = start vertice index, val = end vertice index + inner corner flag
	std::map<u32, std::vector<std::pair<u32, u8>>>	foundEdges;

	// treat each face separately
	for (u32 i = 0; i < 6; i++)
	{
		// this face is free and concern current point
		if (content.mData->mEmptyNeighborsFlag & (1 << i)) // this face is free
		{
			u32 ip = (content.mData->getVertexForFreeFace(1 << i))->first;

			for (auto adj : adjacent_faces[i])
			{
				if (!(content.mData->mEmptyNeighborsFlag & adj)) // this adjacent face is not free
				{
					MeshSimplificationOctreeNode* frontNode = content.mData->getNeighbor(adj | (1 << i));
					if (frontNode)
					{
						// add edge between currentNode & frontNode
						std::pair<u32, u8>* endP = frontNode->getContentType().mData->getVertexForFreeFace(OctreeNodeBase::mOppositeFace[adj]);
#ifdef _DEBUG
						if (!endP)
						{
							printf("WTF");
						}
#endif
						u32 isInnerCorner = 0;

						// check that opposite to adj face is not free
						if (!(content.mData->mEmptyNeighborsFlag & OctreeNodeBase::mOppositeFace[adj]))
						{
							// and that 1<<i face in this node is free
							MeshSimplificationOctreeNode* opposite = content.mData->getNeighbor(OctreeNodeBase::mOppositeFace[adj]);
							if (opposite)
								if (opposite->getContentType().mData->mEmptyNeighborsFlag & (1 << i))
								{
									isInnerCorner = 1;
								}
						}
						
						foundEdges[ip].push_back({ endP->first,isInnerCorner }); // inner corner
					}
					else
					{
						frontNode = content.mData->getNeighbor(adj);
						if (frontNode)
						{
							// add edge between currentNode & frontNode
							std::pair<u32, u8>* endP = frontNode->getContentType().mData->getVertexForFreeFace(1 << i);
#ifdef _DEBUG
							if (!endP)
							{
								printf("WTF");
							}
#endif
							foundEdges[ip].push_back({ endP->first,0 }); // not inner corner
						}
#ifdef _DEBUG
						else
						{
							printf("WTF");
						}
#endif
					}
				}
				else // free adjacent face, get vertex in current cell
				{
					std::pair<u32, u8>* endP = content.mData->getVertexForFreeFace(adj);
#ifdef _DEBUG
					if (!endP)
					{
						printf("WTF");
					}
#endif

					foundEdges[ip].push_back({ endP->first,0 }); // not inner corner
				}
			}
		}
		
	}
	
	// add edges for each point separately 
	for (auto& startp : foundEdges)
	{
		for (auto& endp : startp.second)
		{
			// add edge to edge list
			u32 edgeindex = addEdge(startp.first, endp.first, edgeMap);

			// flag edge if needed (inner corner)
			if (endp.second)
			{
				u32 ew = (edgeindex >> 31);
				mEdges[edgeindex & 0x7fffffff].flags |= 2<< ew;
			}

			// and push it in the right order for later face building
			mVertices[startp.first].mEdges.push_back(edgeindex);
			
		}
	}
}

// split quad faces
void	BuildMeshFromEnveloppe::splitFaces()
{
	// get size of vector now because it will grow when adding splitted faces
	u32 faceCount = (u32)(mFaces.size());

	for (u32 fi = 0; fi < faceCount; fi++)
	{
		if (mFaces[fi].edges.size() <= 3) // triangle, don't need to split
		{
			continue;
		}
		
		splitMoreThanQuadFace(fi);
	
	}
	checkCoherency();
	checkVerticeCoherency();
	
}

void	BuildMeshFromEnveloppe::checkVerticeCoherency()
{
#ifdef _DEBUG
	u32 vindex = 0;
	for (const auto& v : mVertices) // for each vertice
	{

		for (u32 eii = 0; eii < v.mEdges.size();eii++) // look at each edge
		{
			u32 debuge = v.mEdges[eii];
			u32 debugei = debuge & 0x7fffffff;
			u32 debugew = debuge >> 31;
			u32 triangle=mEdges[debugei].t[1-debugew];

			u32 nextdebuge = v.mEdges[(eii+1)% v.mEdges.size()];
			u32 nextdebugei = nextdebuge & 0x7fffffff;
			u32 nextdebugew = nextdebuge >> 31;
			u32 nexttriangle = mEdges[nextdebugei].t[nextdebugew];

			if (triangle != nexttriangle)
			{
				printf("changed edge order\n");
			}

		}
		vindex++;
	}
#endif
}

void	BuildMeshFromEnveloppe::checkCoherency()
{
#ifdef _DEBUG
	// check coherency

	for (const auto& v : mVertices)
	{
		if ((v.mEdges.size() < 3) && (v.mEdges.size() != 0))
		{
			printf("Double sided face");
		}
	}

	// light check
	//return;

	for (const auto& f : mFaces)
	{
		if ((f.edges.size() != 3) && (f.edges.size() != 0))
		{
			printf("WTF");
		}
		if (f.edges.size())
		{
			u32 first = -1;
			std::set<u32>	pointlist;
			for (const auto& e : f.edges)
			{
				u32 ei = e & 0x7fffffff;
				u32 ew = e >> 31;
				if (first == -1)
				{
					first = mEdges[ei].t[ew];
					if (first == -1)
					{
						printf("WTF");
					}
				}
				else if (mEdges[ei].t[ew] != first)
				{
					printf("WTF");
				}

				pointlist.insert(mEdges[ei].v[0]);
				pointlist.insert(mEdges[ei].v[1]);

				if (mFaces[mEdges[ei].t[0]].edges.size() != 3)
				{
					printf("WTF");
				}
			}
			if (pointlist.size() != 3)
			{
				printf("WTF");
			}
			for (auto v : pointlist)
			{
				if (mVertices[v].mEdges.size() == 0)
				{
					printf("WTF");
				}
				if(mVertices[v].mFlag==16)
				{
					printf("WTF");
				}
			}
		}
	}
#endif
}

void	BuildMeshFromEnveloppe::splitQuadFace(u32 fi)
{
	MSFace& currentFace = mFaces[fi];
	v3f n[4];
	u32 ew[2];
	u32 ei[2];

	// get 4 normals
	for (u32 eli = 0; eli < 4; eli++)
	{
		ew[0] = (currentFace.edges[eli] >> 31);
		ei[0] = (currentFace.edges[eli] & 0x7fffffff);
		n[eli] = mVertices[mEdges[ei[0]].v[1 - ew[0]]].mN;
	}
	// split where the normal are the more similar

	float dots[2];
	dots[0] = Dot(n[0], n[2]);
	dots[1] = Dot(n[1], n[3]);
	int splitindex = 0;
	if (dots[0] > dots[1])
	{
		splitindex = 1;
	}

	// split quad => add edge 
	ew[0] = (currentFace.edges[splitindex] >> 31);
	ei[0] = (currentFace.edges[splitindex] & 0x7fffffff);
	ew[1] = (currentFace.edges[splitindex + 1] >> 31);
	ei[1] = (currentFace.edges[splitindex + 1] & 0x7fffffff);

	u32 v1 = mEdges[ei[0]].v[ew[0]];
	u32 v2 = mEdges[ei[1]].v[1 - ew[1]];

	u32 edgeindex = addEdge(v1, v2); // always add a new edge here

	mVertices[v1].insertEdgeBefore(ei[0], edgeindex);
	//mVertices[v1].mEdges.push_back(edgeindex);
	//mVertices[v2].mEdges.push_back(edgeindex ^ (1 << 31));
	mVertices[v2].insertEdgeBefore((currentFace.edges[splitindex + 2] & 0x7fffffff), edgeindex ^ (1 << 31));

	mEdges[edgeindex].t[1] = fi;

	MSFace toAdd;
	if (splitindex == 0)
	{
		toAdd.edges.push_back(edgeindex);
		toAdd.edges.push_back(currentFace.edges[2]);
		toAdd.edges.push_back(currentFace.edges[3]);
		currentFace.edges[2] = edgeindex ^ (1 << 31);
		currentFace.edges.pop_back();
	}
	else
	{
		toAdd.edges.push_back(currentFace.edges[0]);
		toAdd.edges.push_back(edgeindex);
		toAdd.edges.push_back(currentFace.edges[3]);

		currentFace.edges[0] = edgeindex ^ (1 << 31);
		currentFace.edges.pop_back();
	}

	setEdgesInteriorFace(toAdd, mFaces.size(),fi);

	mFaces.push_back(toAdd);
}

void	BuildMeshFromEnveloppe::setEdgesInteriorFace(MSFace& toSet, u32 fi,u32 oldfi)
{
	for (auto& e : toSet.edges)
	{
		u32 ei = e & 0x7fffffff;
		u32 ew = e >> 31;
#ifdef _DEBUG
		if ((mEdges[ei].t[ew] != -1) && ((mEdges[ei].t[ew] != oldfi)))
		{
			printf("WTF");
		}
#endif
		mEdges[ei].t[ew] = fi;
	}
}

void BuildMeshFromEnveloppe::splitMoreThanQuadFace(u32 fi)
{
	
	while (mFaces[fi].edges.size() > 4)
	{
		MSFace& currentFace = mFaces[fi];
		// compute face barycenter
		// first make a normalised v3f list (1 for each edge)
		std::vector<u32>	vlist;
		v3f bary(0.0f, 0.0f, 0.0f);
		for (u32 eli = 0; eli < currentFace.edges.size(); eli++)
		{
			u32 ew = (currentFace.edges[eli] >> 31);
			u32 ei = (currentFace.edges[eli] & 0x7fffffff);
			vlist.push_back(mEdges[ei].v[ew]);

			v3f edgv = mVertices[vlist.back()].mV;
			bary += edgv;
		}
		bary /= (float)currentFace.edges.size();

		// now find the farthest vertice from bary and consider it as an "ear"

		u32		bestvIndex = 0;
		float	longestSQrDist = 0.0f;

		for (u32 eli = 0; eli < vlist.size(); eli++)
		{
			v3f edgv = mVertices[vlist[eli]].mV - bary;
			float sqrdst = NormSquare(edgv);
			if (sqrdst > longestSQrDist)
			{
				longestSQrDist = sqrdst;
				bestvIndex = eli;
			}
		}

		// decal edges order so the edge to remove is the last in the list
		std::vector<u32> decalEdges;
		std::vector<u32> decalvlist;

		for (u32 eli = 0; eli < vlist.size(); eli++)
		{
			u32 decalindex = (eli + bestvIndex + 1) % vlist.size();
			decalEdges.push_back(currentFace.edges[decalindex]);
			decalvlist.push_back(vlist[decalindex]);
		}

		// so now, create a new edge between first vertice and last-2 vertice
		u32 edgeindex = addEdge(decalvlist[0], decalvlist[decalvlist.size()-2]);// always add a new edge here

		//mVertices[vlist[0]].mEdges.push_back(edgeindex);
		mVertices[decalvlist[0]].insertEdgeBefore(decalEdges[0]&0x7fffffff, edgeindex);
		//mVertices[vlist[vlist.size() - 2]].mEdges.push_back(edgeindex ^ (1 << 31));
		mVertices[decalvlist[vlist.size() - 2]].insertEdgeBefore(decalEdges[vlist.size() - 2] & 0x7fffffff, edgeindex ^ (1 << 31));

		mEdges[edgeindex].t[1] = fi;

		MSFace toAdd;
		
		toAdd.edges.push_back(edgeindex);
		toAdd.edges.push_back(decalEdges[vlist.size() - 2]);
		toAdd.edges.push_back(decalEdges[vlist.size() - 1]);
		decalEdges[decalEdges.size() - 2] = edgeindex ^ (1 << 31);
		decalEdges.pop_back();
		currentFace.edges = decalEdges;

		setEdgesInteriorFace(toAdd, mFaces.size(),fi);

		mFaces.push_back(toAdd);
		
	}
	splitQuadFace(fi);
}

//#pragma optimize("",off)
void BuildMeshFromEnveloppe::computeTriangleNormals()
{

	std::vector<u32>	flatTriangles;

	u32 fi=0;
	for (auto& f:mFaces)
	{
		v3f v1= getEdgeVector(f.edges[0]);
		v3f v2= getEdgeVector(f.edges[1]);
		f.normal.CrossProduct(-v1, v2);

		if (NormSquare(f.normal) ==0.0f)
		{
			flatTriangles.push_back(fi);
#ifdef _DEBUG
			printf("info : flat triangle found\n");
#endif
		}
		else
		{
			f.normal.Normalize();
		}
		fi++;
	}

	// manage flat triangles by setting normal of first not flat neighbor

	for (auto t : flatTriangles)
	{
		auto& currentT = mFaces[t];

		for (auto ein : currentT.edges)
		{
			u32 ew = (ein >> 31);
			u32 ei = (ein & 0x7fffffff);

			auto& currentE = mEdges[ei];

			if (NormSquare(mFaces[currentE.t[1 - ew]].normal))
			{
				currentT.normal = mFaces[currentE.t[1 - ew]].normal;
				break;
			}
		}
	}

}


void	BuildMeshFromEnveloppe::tagVerticesForSimplification()
{
#ifdef _DEBUG
	u32 vi = 0;
#endif
	for (auto& v : mVertices)
	{

		// init flag
		// 0 => 1 same normal for all triangles
		// 1 => 2 normals for all triangles
		// 8 => can't be merged (more than 2 normals)
		// >= 16 => removed vertice
		v.mFlag = 8;
		checkVOnCoplanarTriangles(v);

#ifdef _DEBUG
		vi++;
#endif
	}
}


void	BuildMeshFromEnveloppe::checkVOnCoplanarTriangles(MSVertice& v)
{

	v3f	foundNormals[2];
	u32 normalCount=0;

	std::vector<u32>	sharpEdges;
	// for each edge
	u32 eii = 0;
	for (auto& e : v.mEdges)
	{
		u32 ew = (e >> 31);
		u32 ei = (e & 0x7fffffff);

		MSEdge& currentE = mEdges[ei];

		const MSFace& currentT = mFaces[currentE.t[ew]];
		const v3f& n1 = mFaces[currentE.t[ew]].normal;
		const v3f& n2 = mFaces[currentE.t[1- ew]].normal;

		if (fabsf(Dot(n1, n2) - 1.0f) >= 0.001f) //edge
		{
			currentE.flags |= 1;
			sharpEdges.push_back(eii);
		}

		bool found = false;
		for (u32 i = 0; i < normalCount; i++)
		{
			if (fabsf(Dot(n1, foundNormals[i]) - 1.0f) < 0.001f)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			if (normalCount == 2 ) // not mergeable
				return;

			foundNormals[normalCount] = n1;
			normalCount++;
		}
		eii++;
	}

	if (sharpEdges.size() == 2)
	{
		u32 dist = (sharpEdges[1] - sharpEdges[0]);
		if ((dist == 1) || (dist == (v.mEdges.size() - 1)))
		{
			v.mFlag = 8;
		}
		else
		{
			v.mFlag = 1;
		}
	}
	else if (normalCount == 1)
		v.mFlag = 0;
	else
		v.mFlag = 8; 
}

void	BuildMeshFromEnveloppe::doShringEdge(MSVertice& v, const std::pair<u32, u32>& mergeWith,u32 vindex)
{

	auto removeFromGoodIntersection = [](MSVertice& v, u32 vindex)
	{
		MeshSimplificationOctreeNode* currentNode = v.mOctreeNode->getNode<MeshSimplificationOctreeNode>();
		const auto& content = currentNode->getContentType();

		for (auto it = content.mData->mEnvelopeData->mGoodIntersectionPoint.begin(); it != content.mData->mEnvelopeData->mGoodIntersectionPoint.end(); it++)
		{
			if ((*it).first == vindex)
			{
				content.mData->mEnvelopeData->mGoodIntersectionPoint.erase(it);
				break;
			}
		}

	};

	std::vector<u32>	InternEdgedToSwitch = findInternEdgesToSwitch(v, mergeWith);

	u32 shrinkEdge = v.mEdges[mergeWith.first];

	// now do merge
	// the edge to null
	MSEdge& currentE = mEdges[v.mEdges[mergeWith.first] & 0x7fffffff];

	// flatten both triangles
	for (u32 ti = 0; ti < 2; ti++)
	{
		// for each triangle
		MSFace& f = mFaces[currentE.t[ti]];
		flattenTriangles(f, currentE.t[ti], vindex, mergeWith.second, v.mEdges[mergeWith.first] & 0x7fffffff);
	}

	std::vector<u32>::iterator	iteii;
	for (iteii = InternEdgedToSwitch.begin(); iteii != InternEdgedToSwitch.end(); iteii++)
	{
		u32 eii = *iteii;
		u32 e = v.mEdges[eii];
		u32 ew = (e >> 31);
		u32 ei = (e & 0x7fffffff);
#ifdef _DEBUG
		if (mEdges[ei].v[ew] != vindex)
		{
			printf("WTF");
		}
#endif
		mEdges[ei].v[ew] = mergeWith.second;

		// add new edge to dest vertice 
		//mVertices[mergeWith.second].mEdges.push_back(e);
		mVertices[mergeWith.second].insertEdgeBefore(shrinkEdge & 0x7fffffff, e);
	}

	// remove edge from dest vertice
	for (std::vector<u32>::iterator ite = mVertices[mergeWith.second].mEdges.begin(); ite != mVertices[mergeWith.second].mEdges.end(); ite++)
	{
		u32 e = *ite;

		if ((e & 0x7fffffff) == (shrinkEdge & 0x7fffffff))
		{
			mVertices[mergeWith.second].mEdges.erase(ite);
			break;
		}
	}

	mEdges[shrinkEdge & 0x7fffffff].v[0] = mEdges[shrinkEdge & 0x7fffffff].v[1] = 0;

	removeFromGoodIntersection(v, vindex);
}

//#pragma optimize("",on)
// do mesh simplification by merging triangles
void BuildMeshFromEnveloppe::mergeTriangles()
{

#ifdef _DEBUG
	u32 countPass = 0;
#endif
	bool doItAgain = true;
	while (doItAgain)
	{
		doItAgain = false;
		u32 vindex = 0;
		for (auto& v : mVertices)
		{
			if (v.mEdges.size() < 3)
			{
				v.mFlag = 2;
			}
			if (v.mFlag < 2)
			{
				/*if (vindex >= 12233)
				{
					return;
					checkVerticeCoherency();
					for (auto& debuge : v.mEdges)
					{
						u32 debugei = debuge & 0x7fffffff;
						u32 debugew = debuge >> 31;
						printf("edge %d from v[%d] to v[%d], with triangle1[%d] and triangle2[%d]\n", debugei,mEdges[debugei].v[debugew], mEdges[debugei].v[1- debugew], mEdges[debugei].t[debugew], mEdges[debugei].t[1-debugew]);
					}
					checkCoherency();
				}*/
				// get best neighbor to merge with
				// first : edge index in current vertice edge list
				// second : vertice index 
				std::pair<u32,u32> mergeWith;
				if (v.mFlag == 0)
				{
					mergeWith = findBestPlanarMerge(vindex);
				}
				else
				{
					mergeWith = findBestLinearMerge(vindex);
				}

				if (mergeWith.first == (u32)-1) // can't find a good merge
				{
					vindex++;
					continue;
				}

				doShringEdge(v, mergeWith,vindex);

				// this vertice is now invalid
				v.mFlag = 16;
				v.mEdges.clear();

				

				//if(mergeWith.second < vindex)
				{
					// as long as we have found merge, check to do it again
					doItAgain = true;
				}
				
				//checkCoherency();
				//checkVerticeCoherency();
			}

			vindex++;
			//if (vindex > 3)
			//	break;
		}

#ifdef _DEBUG
		countPass++;
#endif
	}

}

void					BuildMeshFromEnveloppe::flattenTriangles(MSFace& t,u32 tIndex, u32 fromV, u32 toV,u32 flattenE)
{
	u32 fromE;
	u32 fromT;
	u32 toE;
	for (auto& tei : t.edges)
	{
		u32 ei = (tei & 0x7fffffff);
		if (ei == flattenE)
		{
			continue;
		}
		MSEdge& currentE = mEdges[ei];
		if( (currentE.v[0] == fromV) || (currentE.v[1] == fromV))
		{
			fromE = tei;
			if (currentE.t[0] == tIndex)
			{
				fromT = currentE.t[1];
			}
			else
			{
				fromT = currentE.t[0];
			}
		}
		else if ((currentE.v[0] == toV) || (currentE.v[1] == toV))
		{
			toE = tei;
		}
#ifdef _DEBUG
		else
		{
			printf("WTF");
		}
#endif
	}

	// manage from t
	for (auto& tei : mFaces[fromT].edges)
	{
		u32 ei = (tei & 0x7fffffff);
		if (ei == (fromE & 0x7fffffff))
		{
			tei = toE;
			break;
		}
	}

	// remove fromE from its end vertice
	auto& fromEStruct = mEdges[(fromE & 0x7fffffff)];
	u32 fromEW = 0;
	if (fromEStruct.v[0] == fromV)
	{
		fromEW = 1;
	}

#ifdef _DEBUG
	if (fromEStruct.v[fromEW] == fromV)
	{
		printf("WTF");
	}
#endif

	auto& FinalV = mVertices[fromEStruct.v[fromEW]]; 
	FinalV.removeEdge(fromE);
	fromEStruct.v[0] = fromEStruct.v[1]= 0;

	// manage to e
	MSEdge& toEStruct = mEdges[toE & 0x7fffffff];
	if (toEStruct.t[0] == tIndex)
	{
		toEStruct.t[0] = fromT;
	}
	else
	{
		toEStruct.t[1] = fromT;
	}

	t.edges.clear();
}


// return the list of edges just to be switch to new point
std::vector<u32>		BuildMeshFromEnveloppe::findInternEdgesToSwitch(MSVertice& v, std::pair<u32, u32> mergeWith)
{
	u32 e = v.mEdges[mergeWith.first];
	u32 ew = (e >> 31);
	u32 ei = (e & 0x7fffffff);

	MSEdge&	currentE = mEdges[ei];

	// get edges for each triangles
	std::set<u32> triangleEdges;
	for (u32 ti = 0; ti < 2; ti++)
	{
		MSFace& f = mFaces[currentE.t[ti]];
		for (auto& tei : f.edges)
		{
			ei = (tei & 0x7fffffff);
			triangleEdges.insert(ei);
		}
	}
	// start at mergeWith.first
	std::vector<u32>	toSwitch;
	e = 0;
	for (u32 eii=0;eii< v.mEdges.size();eii++)
	{
		e = (eii + mergeWith.first) % v.mEdges.size();
		const auto& vei = v.mEdges[e];
		ei = (vei & 0x7fffffff);
		if (triangleEdges.find(ei) == triangleEdges.end())
		{
			toSwitch.push_back(e);
		}
	}
	return toSwitch;
}



std::pair<u32, u32>		BuildMeshFromEnveloppe::findBestPlanarMerge(u32 vindex)
{
	MSVertice& v = mVertices[vindex];

	u32		bestVFound=0;
	u32		bestEFound = -1;
	u32		bestFoundFlag=0;
	u32		bestFoundEdgeCount=10000;

	// for each edge
	u32 internEdgeIndex = 0;
	for (auto& e : v.mEdges)
	{
		u32 ew = (e >> 31);
		u32 ei = (e & 0x7fffffff);

		const MSEdge& currentE = mEdges[ei];

		u32 endv = currentE.v[1-ew];
		MSVertice& otherV = mVertices[endv];

		bool isNewBest = false;

		if (otherV.mFlag > bestFoundFlag)
		{
			isNewBest = true;

		}
		else if (otherV.mFlag == bestFoundFlag)
		{
			if (bestFoundEdgeCount > otherV.mEdges.size())
			{
				isNewBest = true;
			}
			else if (bestFoundEdgeCount == otherV.mEdges.size())
			{
				if (endv > bestVFound)
				{
					isNewBest = true;
				}
			}
		}
		if (isNewBest)
		{
			if (isMergeValid(v, endv, internEdgeIndex))
			{
				bestEFound = internEdgeIndex;
				bestVFound = endv;
				bestFoundFlag = otherV.mFlag;
				bestFoundEdgeCount = otherV.mEdges.size();
			}
			
		}
		internEdgeIndex++;
		
	}
	return { bestEFound,bestVFound };
}

// check that merging two points will not generate "bad triangles" (concave poly)
bool	BuildMeshFromEnveloppe::isMergeValid(MSVertice& v, u32 endV, u32 internEdgeIndex)
{
	// check that generated triangles are well formed
	std::vector<v3f> newVectors;
	for (u32 eii=1;eii<v.mEdges.size();eii++)
	{
		u32 e = v.mEdges[(eii+ internEdgeIndex)% v.mEdges.size()];
		u32 ew = (e >> 31);
		u32 ei = (e & 0x7fffffff);
		MSEdge& current = mEdges[ei];
		v3f edgeDir = mVertices[current.v[1-ew]].mV - mVertices[endV].mV;
		newVectors.push_back(edgeDir);
	}

	v3f n1,n2;
	n1.CrossProduct(newVectors[0], newVectors[1]);
	float signe = 0.0f;

	for (u32 n = 1; n < newVectors.size()-1; n++)
	{
		n2.CrossProduct(newVectors[n], newVectors[n+1]);
		
		signe = Dot(n1, n2);
		
		if (signe  <= 0.0f)
			return false;

		n1 = n2;
	}
	return true;
}


std::pair<u32, u32> 	BuildMeshFromEnveloppe::findBestLinearMerge(u32 vindex)
{
	MSVertice& v = mVertices[vindex];

	u32		bestVFound = 0;
	u32		bestEFound = -1;
	u32		bestFoundFlag = 0;
	u32		bestFoundEdgeCount = 10000;

	// for each edge
	u32 internEdgeIndex = 0;
	for (auto& e : v.mEdges)
	{
		u32 ew = (e >> 31);
		u32 ei = (e & 0x7fffffff);

		const MSEdge& currentE = mEdges[ei];

		if ((currentE.flags&1)==0)
		{
			internEdgeIndex++;
			continue;
		}

		u32 endv = currentE.v[1-ew];
		MSVertice& otherV = mVertices[endv];

		bool isNewBest = false;

		if (otherV.mFlag > bestFoundFlag)
		{
			isNewBest = true;

		}
		else if (otherV.mFlag == bestFoundFlag)
		{
			if (bestFoundEdgeCount > otherV.mEdges.size())
			{
				isNewBest = true;
			}
			else if (bestFoundEdgeCount == otherV.mEdges.size())
			{
				if (endv > bestVFound)
				{
					isNewBest = true;
				}
			}
		}
		if (isNewBest)
		{
			if (isLinearMergeValid(v, endv, internEdgeIndex))
			{
				bestEFound = internEdgeIndex;
				bestVFound = endv;
				bestFoundFlag = otherV.mFlag;
				bestFoundEdgeCount = otherV.mEdges.size();
			}
		}
		internEdgeIndex++;
	}
	return { bestEFound ,bestVFound };
}

bool	BuildMeshFromEnveloppe::isLinearMergeValid(MSVertice& v, u32 endV, u32 internEdgeIndex)
{
	// check that generated triangles are well formed on each side of edge
	std::vector<v3f> newVectors[2];
	u32 listIndex = 0;

	for (u32 eii = 1; eii < v.mEdges.size(); eii++)
	{
		u32 e = v.mEdges[(eii + internEdgeIndex) % v.mEdges.size()];
		u32 ew = (e >> 31);
		u32 ei = (e & 0x7fffffff);
		MSEdge& current = mEdges[ei];
		v3f edgeDir = mVertices[current.v[1 - ew]].mV - mVertices[endV].mV;
		newVectors[listIndex].push_back(edgeDir);
		if (current.flags&1) // corner edge
		{
			listIndex++; 
			if (listIndex > 1) // if more than two edges are on corner, there's a problem here
				return false;
			newVectors[listIndex].push_back(edgeDir); // this vector is on both sides
		}
	}

	if ((newVectors[0].size() < 2) || (newVectors[1].size() < 2))
		return false;

	for (listIndex = 0; listIndex < 2; listIndex++)
	{
		v3f n1, n2;
		n1.CrossProduct(newVectors[listIndex][0], newVectors[listIndex][1]);
		float signe = 0.0f;

		for (u32 n = 1; n < newVectors[listIndex].size() - 1; n++)
		{
			n2.CrossProduct(newVectors[listIndex][n], newVectors[listIndex][n + 1]);
			signe = Dot(n1, n2);
			
			if (signe <= 0.0f)
				return false;
			
			n1 = n2;
		}
	}
	return true;
}


// for each free face, check if an edge exists with a neighbor not free face
void	BuildMeshFromEnveloppe::setUpFaces()
{
	int vindex = 0;

	// for each vertice
	for (const auto& v : mVertices)
	{
		// for each edge
		for (const auto& ei : v.mEdges)
		{
			MSEdge* e = &mEdges[ei& 0x7fffffff];
			u32 ew = ei>>31;

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
					toAdd.edges.push_back(currentEI);

					if (nextVertice == vindex)
						break;

					const MSVertice& destV = mVertices[nextVertice];

					// find current edge in destination vertex
					u32 nexte = destV.getEdgeIndexInThisList(prevVertice, mEdges);
					// and jump to next one
					nexte++;
					nexte = nexte % destV.mEdges.size();

					currentEI = destV.mEdges[nexte];
					e = &mEdges[currentEI& 0x7fffffff];

					ew = currentEI >> 31;
#ifdef _DEBUG
					if (e->t[ew] != -1)
					{
						printf("WTF");
					}
#endif
					prevVertice = nextVertice;
					nextVertice = e->v[1 - ew];

					currentEdgeIndexInFace++;
					if (currentEdgeIndexInFace > 7)
					{
#ifdef _DEBUG
						printf("WTF");
#endif
						break;
					}
				}

				mFaces.push_back(toAdd);

			}


		}
		vindex++;
	}
}

// remove empty vertice with no more edges
void BuildMeshFromEnveloppe::removeEmptyVertice(u32 verticeindex, std::vector<u32>& verticelist)
{
	// swap this vertice with last vertice
	u32 lastVerticeIndex = (mVertices.size() - 1);

	if (verticeindex != lastVerticeIndex)
	{
		const auto& last = mVertices.back();

		for (auto ein : last.mEdges)
		{
			u32 ew = ein >> 31;
			u32 ei = ein & 0x7fffffff;

			auto& e = mEdges[ei];

#ifdef _DEBUG
			if (e.v[ew] != lastVerticeIndex)
			{
				printf("Problem here");
			}
#endif
			e.v[ew] = verticeindex;
		}
		mVertices[verticeindex] = last;

		// swap in list
		for (auto& v : verticelist)
		{
			if (v == lastVerticeIndex)
				v = verticeindex;
		}

		// swap in node goodintersectionpoint
		{
			MeshSimplificationOctreeNode* currentNode = last.mOctreeNode->getNode<MeshSimplificationOctreeNode>();
			const auto& content = currentNode->getContentType();

			for (auto& v : content.mData->mEnvelopeData->mGoodIntersectionPoint)
			{
				if (v.first == lastVerticeIndex)
					v.first = verticeindex;
			}
		}

	}
	mVertices.pop_back();
}


// remove face with no more edges
void BuildMeshFromEnveloppe::removeEmptyFace(u32 faceindex,std::vector<u32>& facelist)
{
	// swap this face with last face
	u32 lastFaceIndex = (mFaces.size() - 1);
	if (faceindex != lastFaceIndex)
	{
		const auto& last = mFaces.back();

		for (auto ein: last.edges)
		{
			u32 ew = ein >> 31;
			u32 ei = ein & 0x7fffffff;

			auto& e = mEdges[ei];

			for (size_t i=0;i<2;i++)
			{
				if (e.t[i] == lastFaceIndex)
				{
					e.t[i] = faceindex;
					break;
				}
			}
		}
		mFaces[faceindex] = last;

		// swap in list
		for (auto& v : facelist)
		{
			if (v == lastFaceIndex)
				v = faceindex;
		}
	}
	mFaces.pop_back();

}

// remove given edge from given face
void	BuildMeshFromEnveloppe::removeEdgeFromFace(u32 edgeindex, u32 faceindex)
{
	auto& f = mFaces[faceindex];
	
	for (auto eit = f.edges.begin(); eit != f.edges.end(); eit++)
	{
		u32 ein = *eit;
		u32 ei = ein & 0x7fffffff;
		if (ei == edgeindex)
		{
			f.edges.erase(eit);
			break;
		}
	}
	if(f.edges.size()==0)
	{
		std::vector<u32>	empty;
		removeEmptyFace(faceindex, empty);
	}
}

// remove given edge from given vertice
void	BuildMeshFromEnveloppe::removeEdgeFromVertice(u32 edgeindex, u32 verticeindex)
{
	auto& v = mVertices[verticeindex];

	for (auto vit = v.mEdges.begin(); vit != v.mEdges.end(); vit++)
	{
		u32 ein = *vit;
		u32 ei = ein & 0x7fffffff;
		if (ei == edgeindex)
		{
			v.mEdges.erase(vit);
			break;
		}
	}

}

// remove edge from list after removing it from faces
void	BuildMeshFromEnveloppe::removeEdge(u32 edgeindex, std::vector<u32>& edgelist)
{
	const auto& currentE = mEdges[edgeindex];
	// remove edge from faces
	for (size_t i = 0; i < 2; i++)
	{
		u32 fi = currentE.t[i];
		removeEdgeFromFace(edgeindex, fi);
	}

	// remove edge from vertices
	for (size_t i = 0; i < 2; i++)
	{
		u32 vi = currentE.v[i];
		removeEdgeFromVertice(edgeindex, vi);
	}

	// swap last edge with this edge
	u32 lastEdgeIndex = (mEdges.size() - 1);
	if (edgeindex != lastEdgeIndex)
	{
		const auto& last = mEdges.back();
		// swap edge reference in vertices
		for (size_t i = 0; i < 2; i++)
		{
			u32 vi = last.v[i];
			auto& v = mVertices[vi];
			for (u32& ein : v.mEdges)
			{
				u32 ew = ein >> 31;
				u32 ei = ein & 0x7fffffff;
				if (ei == lastEdgeIndex)
				{
					ein = (ew << 31) | edgeindex;
					break;
				}
			}
		}
		// swap edge reference in faces
		for (size_t i = 0; i < 2; i++)
		{
			u32 fi = last.t[i];
			auto& f = mFaces[fi];
			for (u32& ein : f.edges)
			{
				u32 ew = ein >> 31;
				u32 ei = ein & 0x7fffffff;
				if (ei == lastEdgeIndex)
				{
					ein = (ew << 31) | edgeindex;
					break;
				}
			}
		}

		// swap edge reference in edge list
		for (auto& ei : edgelist)
		{
			if (ei == lastEdgeIndex)
			{
				ei = edgeindex;
			}
		}

		mEdges[edgeindex] = last;
	}
	mEdges.pop_back();
}

void	BuildMeshFromEnveloppe::finalClean()
{

	mFinalMergedVIndex.resize(mVertices.size());
	// starting with a index->index array
	for (size_t i = 0; i < mFinalMergedVIndex.size();i++)
	{
		mFinalMergedVIndex[i] = i;
	}

	std::vector<u32>	VerticesToRemove;
	u32 nodeIndex = 0;

	// fast merge mergeable vertices
	for (const auto n : mNodeList)
	{
		MeshSimplificationOctreeNode* currentNode = n->getNode<MeshSimplificationOctreeNode>();
		const auto& content = currentNode->getContentType();

		const u32 vlistSize = content.mData->mEnvelopeData->mGoodIntersectionPoint.size();

		if (vlistSize > 1)
		{
			std::vector<std::pair<u32, u8>> mergedGoodIntersections;

			u32	mergedMask = 0;

			for (size_t vi1 = 0; vi1 < vlistSize; vi1++)
			{
				if (!(mergedMask & (1 << vi1)))
				{
					auto& v1 = content.mData->mEnvelopeData->mGoodIntersectionPoint[vi1];

					for (size_t vi2 = vi1 + 1; vi2 < vlistSize; vi2++)
					{
						if (!(mergedMask & (1 << vi2)))
						{
							auto& v2 = content.mData->mEnvelopeData->mGoodIntersectionPoint[vi2];
							if (DistSquare(mVertices[v1.first].mV, mVertices[v2.first].mV) < 0.05f)
							{
								// merge vi2 => vi1
								mergedMask |= (1 << vi2);
								mFinalMergedVIndex[v2.first] = v1.first;
								VerticesToRemove.push_back(v2.first);
							}
						}
					}
					
				}
			}
			
		}

	}
	
	DetectFlatTriangles(mFinalMergedVIndex);

}
#pragma optimize("",off)
void	BuildMeshFromEnveloppe::setupInnerCorners()
{
	auto getEdgeNormal = [this](auto& e)->v3f {
		
		// get first vertice
		u32 vindex = e.v[0];
		auto& v = mVertices[e.v[0]];

		v3f edge(mVertices[e.v[1]].mV - mVertices[e.v[0]].mV);
		edge.Normalize();

		MeshSimplificationOctreeNode* currentNode = v.mOctreeNode->getNode<MeshSimplificationOctreeNode>();
		auto& content = currentNode->getContentType();
		
		u32 freefacemask = 0;
		// find current vertice in node
		for (const auto& f : content.mData->mEnvelopeData->mGoodIntersectionPoint)
		{
			if (f.first == vindex)
			{
				freefacemask = f.second;
				break;
			}
		}
#ifdef _DEBUG
		if (!freefacemask)
		{
			printf("problem here\n");
		}
#endif

		v3f right;
		right.CrossProduct(OctreeNodeBase::mNeightboursDecalVectors[OctreeNodeBase::mSetBitIndex[freefacemask]],edge);
		right.Normalize();
		v3f normal;
		normal.CrossProduct(edge,right );
	
		return normal;
	};

	mInnerCornersList.clear();

	// search inner corner edges
	u32 edgeindex = 0;
	for (auto& e : mEdges)
	{
		if ((e.flags & 6)==6) // this one is a good candidate
		{
			// compute "edge normal"
			v3f normal = getEdgeNormal(e);

			// retreive two edges to "link"
			v3f P[2];
			v3f	inoutvector[2];
			
			// check that angle between in/out vector and normal is compatible
			bool isGoodEdge = true;
			for (size_t i = 0; i < 2; i++)
			{
				u32 ei=mVertices[e.v[i]].getLocalEdgeIndex(edgeindex);
				P[i] = mVertices[e.v[i]].mV;

				u32 Linkededges = mVertices[e.v[i]].mEdges[(ei+2)&3];
				u32 lew = Linkededges >> 31;
				u32 lei = Linkededges & 0x7fffffff;

				inoutvector[i] = mVertices[mEdges[lei].v[1-lew]].mV - mVertices[e.v[i]].mV;
				
				float testnormal = Dot(inoutvector[i], normal);
				if (testnormal <= 0.1f)
				{
					isGoodEdge = false;
					break;
				}
			}

			// not a good candidate
			if (!isGoodEdge)
			{
				e.flags ^= 6; // remove flag
				edgeindex++;
				continue;
			}

			// search intersection point
			v3f DP(P[0] - P[1]);
			v3f v1(inoutvector[0]);
			v1.Normalize();
			v3f v2(inoutvector[1]);
			v2.Normalize();

			// perp vector
			v3f perp;
			perp.CrossProduct(v1, v2);

			float perpn = NormSquare(perp);
			if (perpn>0.4f) // only if enough angle
			{
				// D1 = P[0] + j.V1
				// D2 = P[1] + k.V2
				// DP = P[0] - P[1]
				// Vt = P[0] + j.V1 - P[1] - k.V2 = DP + j.V1 -k.V2

				// we want to find j & k so that Vt.V1 = 0 & Vt.V2 = 0 (perpendicular to both)
				// DP.V1 + j.V1.V1 - k.V2.V1 = 0
				// DP.V2 + j.V1.V2 - k.V2.V2 = 0
				// V1.V1 = V2.V2 =1
				//  { DP.V1 + j - k.V2.V1 = 0
				//  { DP.V2 + j.V1.V2 - k = 0
				//
				//  multiply first equation with -V1.V2
				//  { -(DP.V1).(V1.V2) - j.V1.V2 + k.(V2.V1)^2 = 0
				//  { DP.V2 + j.V1.V2 - k = 0
				// add the two equations to eliminate j
				//  DP.V2 - (DP.V1).(V1.V2) + k.(V2.V1)^2 - k = 0
				// k.((V2.V1)^2 - 1) = (DP.V1).(V1.V2) - DP.V2
				// k = ((DP.V1).(V1.V2) - DP.V2) / ((V2.V1)^2 - 1)

				float k, j;
				float dpv1 = Dot(DP, v1);
				float dpv2 = Dot(DP, v2);
				float v1v2 = Dot(v1, v2);
				if (v1v2 != 0.0f)
				{
					k = (dpv1 * v1v2 - dpv2)  / (v1v2 * v1v2 - 1.0f);
					j = (k * v1v2) - dpv1;
				}
				else
				{
					j = -dpv1;
					k = dpv2;
				}

				v3f foundp1(P[0] + v1 * j);
				v3f foundp2(P[1] + v2 * k);

				//if (DistSquare(foundp1, foundp2) < 1.0f) // valid distance
				{
					v3f bestP(foundp1 + foundp2);
					bestP *= 0.5f;

					if ((DistSquare(bestP, mVertices[e.v[0]].mV) < 4.0f) && (DistSquare(bestP, mVertices[e.v[1]].mV) < 4.0f))
					{
						
						innerCornerEdge toAdd;
						toAdd.mFinalPos = bestP;
						toAdd.mVertices[0] = mVertices[e.v[0]].mV;
						toAdd.mVertices[1] = mVertices[e.v[1]].mV;
						toAdd.mOctreeNode[0] = mVertices[e.v[0]].mOctreeNode;
						toAdd.mOctreeNode[1] = mVertices[e.v[1]].mOctreeNode;

						mInnerCornersList.push_back(toAdd);
						
					}
					/*else
					{
						printf("");
					}*/
				}
				/*else
				{
					printf("");
				}*/
			}
			/*else
			{
				printf("");
			}*/
		}
		edgeindex++;
	}
}


void	BuildMeshFromEnveloppe::moveInnerCorners()
{
	auto isVerticeThere = [this](const v3f& v, const nodeInfo* node)->u32 {

		MeshSimplificationOctreeNode* currentNode = node->getNode<MeshSimplificationOctreeNode>();
		auto& content = currentNode->getContentType();

		u32 freefacemask = 0;
		// find current vertice in node
		for (const auto& f : content.mData->mEnvelopeData->mGoodIntersectionPoint)
		{
			if (DistSquare(mVertices[f.first].mV, v)<0.01f) 
			{
				return f.first;
			}
		}
		return -1;
	};

	struct validIC
	{
		size_t	index;
		u32		vertices[2];
	};
	std::vector<validIC>	validOnes;
	size_t index = 0;


	struct linkedVandE
	{
		std::set<u32>		linkedV;
		std::vector<u32>	linkedE;
	};

	std::map<u32, linkedVandE>	linkedVertices;
	// build list of still valid corners and linked vertices

	for (auto& e : mInnerCornersList)
	{
		validIC	current;
		
		// check if edge still exists
		for (u32 i = 0; i < 2; i++)
		{
			current.vertices[i] = isVerticeThere(e.mVertices[i], e.mOctreeNode[i]);
		}

		if ((current.vertices[0] != -1) && (current.vertices[1] != -1))
		{
			current.index = index;
			validOnes.push_back(current);
			linkedVertices[current.vertices[0]].linkedV.insert(current.vertices[1]);
			linkedVertices[current.vertices[0]].linkedE.push_back(index);
			linkedVertices[current.vertices[1]].linkedV.insert(current.vertices[0]);
			linkedVertices[current.vertices[1]].linkedE.push_back(index);
		}
		
		++index;
	}

	// group vertices
	std::map<u32, std::vector<u32>>	Groups;

	std::function<void(u32, u32)> recurseInsertInGroup = [this,&Groups, &linkedVertices,&recurseInsertInGroup](u32 group, u32 toInsert)->void
	{
		if (group == toInsert)
			return;
		for (auto i : Groups[group]) // check if already inserted
		{
			if (i == toInsert)
			{
				return;
			}
		}

		Groups[group].push_back(toInsert);
		for (auto i : linkedVertices[toInsert].linkedV)
		{
			recurseInsertInGroup(group, i);
		}
		linkedVertices[toInsert].linkedV.clear();
	};

	std::function<void(u32, std::set<u32>&)> insertInGroup = [&Groups,&linkedVertices,&recurseInsertInGroup](u32 toInsert,std::set<u32>& linkList)->void
	{
		if( (Groups.find(toInsert) == Groups.end()) && linkList.size())
		{
			Groups[toInsert]; // create entry
			for (auto link : linkList)
			{
				recurseInsertInGroup(toInsert, link);
			}
			linkList.clear();
		}
	};

	for (auto& v : linkedVertices)
	{
		insertInGroup(v.first, v.second.linkedV);
	}



	// move simple cases
	for (auto& ve : validOnes)
	{
		auto& e = mInnerCornersList[ve.index];

		if ((linkedVertices[ve.vertices[0]].linkedE.size() == 1) && (linkedVertices[ve.vertices[1]].linkedE.size() == 1))
		{
			u32 edgeToRemove = mVertices[ve.vertices[1]].getEdgeIndexInThisList(ve.vertices[0], mEdges);

			if (edgeToRemove != -1)
			{
				// ok, so move vertices here
				mVertices[ve.vertices[0]].mV = e.mFinalPos;
				doShringEdge(mVertices[ve.vertices[1]], { edgeToRemove ,ve.vertices[0] }, ve.vertices[1]);
			}
		}
	}
}

#pragma optimize("",on)
void	BuildMeshFromEnveloppe::firstClean()
{

	// test if there's a edge between two vertices
	auto isLink = [&](u32 v1, u32 v2)->bool {

		for (auto& ein : mVertices[v1].mEdges)
		{
			u32 ew = ein >> 31;
			u32 ei = ein & 0x7fffffff;

			if (mEdges[ei].v[1 - ew] == v2)
			{
				return true;
			}
		}
		return false;
	};

	std::vector<u32>	VerticesToRemove;
	std::vector<u32>	EdgesToRemove;

	u32 nodeIndex = 0;

	// check nodes with multiple points
	for (const auto n : mNodeList)
	{

		MeshSimplificationOctreeNode* currentNode = n->getNode<MeshSimplificationOctreeNode>();
		const auto& content = currentNode->getContentType();

		const u32 vlistSize = content.mData->mEnvelopeData->mGoodIntersectionPoint.size();
		
		if ( (vlistSize > 1) && (vlistSize < 4)) // don't merge if 4 or 5 vertices to avoid manifold problems
		{
			if(content.mData->countOppositeFreeFaces() == 0) // don't merge if opposite free faces
			{
				std::vector<std::pair<u32, u8>> mergedGoodIntersections;

				u32	mergedMask = 0;

				EdgesToRemove.clear();

				for (size_t vi1 = 0; vi1 < vlistSize; vi1++)
				{
					if (!(mergedMask & (1 << vi1)))
					{
						auto& v1 = content.mData->mEnvelopeData->mGoodIntersectionPoint[vi1];

						for (size_t vi2 = vi1+1; vi2 < vlistSize; vi2++)
						{
							if (!(mergedMask & (1 << vi2)))
							{
								auto& v2 = content.mData->mEnvelopeData->mGoodIntersectionPoint[vi2];
								if (DistSquare(mVertices[v1.first].mV, mVertices[v2.first].mV) < 0.05f)
								{
									if (isLink(v1.first, v2.first)) // merge only if edges are connected
									{
										// merge vi2 => vi1
										mergedMask |= (1 << vi2);
										// merge in edges
										for (u32 eii = 0; eii < mVertices[v2.first].mEdges.size(); eii++)
										{
											u32& ein = mVertices[v2.first].mEdges[eii];
											u32 ew = ein >> 31;
											u32 ei = ein & 0x7fffffff;

											auto& e = mEdges[ei];


											if (e.v[ew] != v2.first)
											{
#ifdef _DEBUG
												printf("Problem here");
#endif
											}
											else
											{
												if (e.v[1 - ew] == v1.first)
												{
													EdgesToRemove.push_back(ei);
												}
												else
												{
													// edge are reordered at the end of this method
													mVertices[v1.first].mEdges.push_back(ein);
												}
												e.v[ew] = v1.first;

											}

										}

										mVertices[v2.first].mEdges.clear();

										VerticesToRemove.push_back(v2.first);

										v1.second |= v2.second;
									}
								}
								
							}
						}
						mergedGoodIntersections.push_back(content.mData->mEnvelopeData->mGoodIntersectionPoint[vi1]);
					}
				}

				content.mData->mEnvelopeData->mGoodIntersectionPoint = mergedGoodIntersections;

				while (EdgesToRemove.size())
				{
					removeEdge(EdgesToRemove.back(), EdgesToRemove);
					EdgesToRemove.pop_back();
				}
			}
		}
		nodeIndex++;
		
	}

	while (VerticesToRemove.size())
	{
		removeEmptyVertice(VerticesToRemove.back(), VerticesToRemove);
		VerticesToRemove.pop_back();
	}

	removeFlatFaces();
	
	reorderEdgesInVertices();
}

void	BuildMeshFromEnveloppe::DetectFlatTriangles(const std::vector<u32>& verticesIndex)
{

	// when a flat face is found, clear it's edges
	for (size_t fi = 0; fi < mFaces.size(); fi++)
	{
		if (mFaces[fi].edges.size() ==3)
		{
			std::set<u32> vertices;

			for (auto ein : mFaces[fi].edges)
			{
				u32 ew = ein >> 31;
				u32 ei = ein & 0x7fffffff;

				const auto& e = mEdges[ei];

				vertices.insert(verticesIndex[e.v[ew]]);
			}

			if (vertices.size() != 3)
			{
				mFaces[fi].edges.clear();
			}
		}
	}
}

void	BuildMeshFromEnveloppe::removeFlatFaces()
{
	std::vector<u32>	EdgesToRemove;
	std::vector<u32>	facesToRemove;
	// now remove flat faces
	for (size_t fi = 0; fi < mFaces.size(); fi++)
	{
		if ((mFaces[fi].edges.size() < 3) && mFaces[fi].edges.size())
		{
			if (mFaces[fi].edges.size() != 2)
			{
#ifdef _DEBUG
				printf("Problem here");
#endif
			}
			else
			{
				u32 ew[2];
				u32 ei[2];

				// find the two edges of the flat face
				for (size_t eii = 0; eii < 2; eii++)
				{
					u32 ein = mFaces[fi].edges[eii];
					ew[eii] = ein >> 31;
					ei[eii] = ein & 0x7fffffff;
				}

				auto& e1 = mEdges[ei[0]];
				auto& e2 = mEdges[ei[1]];

				// replace flat face in edge1 by opposite face in edge2 
				e1.t[ew[0]] = e2.t[1 - ew[1]];
				// add edge2 to remove list 
				EdgesToRemove.push_back(ei[1]);

				// remove edge2 from vertices
				for (u32 vi : e2.v)
				{
					removeEdgeFromVertice(ei[1], vi);
				}

				// replace edge2 by edge1 in opposite face 
				auto& f = mFaces[e1.t[ew[0]]];

				for (auto& ein : f.edges)
				{
					u32 lew = ein >> 31;
					u32 lei = ein & 0x7fffffff;

					if (lei == ei[1]) // edge2 was found
					{
						if (lew == ew[1])
						{
#ifdef _DEBUG
							printf("Problem here");
#endif
						}
						else
						{
							ein = (((lew ^ ew[0]) & 1) << 31) | ei[0];
						}
						break;
					}
				}
			}
			facesToRemove.push_back(fi);
		}
	}

	while (EdgesToRemove.size())
	{
		removeEdge(EdgesToRemove.back(), EdgesToRemove);
		EdgesToRemove.pop_back();
	}
	while (facesToRemove.size())
	{
		removeEmptyFace(facesToRemove.back(), facesToRemove);
		facesToRemove.pop_back();
	}
}

// reorder edges in vertices
void	BuildMeshFromEnveloppe::reorderEdgesInVertices()
{
	auto getNextEdge= [&](u32 edge)->u32 {
		u32 ew = edge >> 31;
		u32 ei = edge & 0x7fffffff;

		auto& currentf=mFaces[mEdges[ei].t[1-ew]];
		
		edge ^= (1 << 31);

		u32 inFaceIndex = 0;
		for (u32 e : currentf.edges)
		{
			if (e == edge)
				break;
			inFaceIndex++;
		}

		inFaceIndex ++;
		inFaceIndex = inFaceIndex % currentf.edges.size();

		return currentf.edges[inFaceIndex];
	};

	// reorder edges in vertices
	
	// for each vertice in node
	for(auto& v : mVertices)
	{
		auto edgeList = std::move(v.mEdges);

		u32 currentEdge = edgeList[0];
		do
		{
			v.mEdges.push_back(currentEdge);
			currentEdge = getNextEdge(currentEdge);

			if (v.mEdges.size() > edgeList.size())
			{
#ifdef _DEBUG
				printf("problem here\n");
#endif
				break;
			}

		} while (currentEdge != edgeList[0]);
	}
}


void	BuildMeshFromEnveloppe::addMultipleVertices(nodeInfo& node, const v3f& goodpoint)
{
	MeshSimplificationOctreeNode* currentNode = node.getNode<MeshSimplificationOctreeNode>();
	const auto& content = currentNode->getContentType();

	for (u32 i = 0; i < 6; i++)
	{
		if (content.mData->mEmptyNeighborsFlag & (1 << i))
		{
			content.mData->mEnvelopeData->mGoodIntersectionPoint.push_back({ addVertice(goodpoint,&node), 1<<i });
		}
	}

}

void	BuildMeshFromEnveloppe::initCellSurfaceList(nodeInfo& node)
{
	MeshSimplificationOctreeNode* currentNode = node.getNode<MeshSimplificationOctreeNode>();
	const auto& content = currentNode->getContentType();

	v3f nodepos = node.coord;
	nodepos *= 0.5f;

	// construct triangle list, associated with first point found and surface dist 
	std::map<u32, std::tuple<MSTriangleVertex*,float,v3f>>	trianglelist;

	for (auto& v : content.mData->mVertices)
	{
		if (trianglelist.find(v.mTriangleIndex) == trianglelist.end())
		{
			v3f inNodeP(v.mVertex - nodepos);
			v3f normal = mTriangleInfos[v.mTriangleIndex].mNormal;
			float dist=Dot(normal, inNodeP);
			
			if (dist < 0.0f)
			{
				dist = -dist;
				normal = -normal;
			}
			trianglelist.insert({ v.mTriangleIndex,std::make_tuple(&v, dist, normal) });
		}
	}

	// then compute surface list
	std::map<int, std::vector<u32>>	constructionMap;
	
	for (auto& t : trianglelist)
	{
		int mapIndex = (int)(std::get<1>(t.second)*16.0f);

		int indexes[3] = { mapIndex,mapIndex - 1,mapIndex + 1 };

		bool found = false;
		for (size_t i = 0; i < 3; i++)
		{
			if ( (indexes[i] >= 0) && (constructionMap.find(indexes[i]) != constructionMap.end()))
			{
				for (auto si : constructionMap[indexes[i]])
				{
					auto& s = mCellData.mCellSurfaces[si];
					if (fabsf(s.mPlane.mDist - std::get<1>(t.second)) < 0.01f)
					{
						if (Dot(s.mPlane.mNormal , std::get<2>(t.second))>0.98f)
						{
							s.mSurface += mTriangleInfos[t.first].mSurface;
							found = true;
							std::get<0>(t.second)->mSurfaceIndex = si;
							break;
						}
					}
				}
				if (found)
					break;
			}
		}

		if (!found)
		{
			MSSurfaceStruct	toAdd;
			toAdd.mSurface = mTriangleInfos[t.first].mSurface;
			toAdd.mPlane.mNormal = std::get<2>(t.second);
			toAdd.mPlane.mDist = std::get<1>(t.second);
			toAdd.mInOctreePlaneP0 = std::get<0>(t.second)->mVertex;
			mCellData.mCellSurfaces.push_back(toAdd);

			u32 newIndex = (u32)(mCellData.mCellSurfaces.size() - 1);

			constructionMap[mapIndex].push_back(newIndex);

			std::get<0>(t.second)->mSurfaceIndex = newIndex;
		}

	}


	// keep only convex hull vertices for each surface
	 
	// vertex list per surface
	std::vector<std::vector<u32> >	perSurfaceVertex;
	perSurfaceVertex.resize(mCellData.mCellSurfaces.size());

	{
		u32 vindex = 0;
		for (auto& v : content.mData->mVertices)
		{
			v.mSurfaceIndex = std::get<0>(trianglelist[v.mTriangleIndex])->mSurfaceIndex;
			perSurfaceVertex[v.mSurfaceIndex].push_back(vindex);
			vindex++;
		}
	}

	{
		std::vector<MSTriangleVertex>	newverticelist;

		u32 surfindex = 0;
		for (const auto& s : perSurfaceVertex)
		{
			v3f normal=mCellData.mCellSurfaces[surfindex].mPlane.mNormal;
			v3f	up(1.0f, 0.0f, 0.0f);
			if (fabsf(Dot(normal, up)) > 0.9f)
			{
				up.Set(0.0f, 1.0f, 0.0f);
			}
			v3f right;
			right.CrossProduct(normal, up);
			right.Normalize();
			up.CrossProduct(right, normal);

			std::vector<std::tuple<v2f,u32>>	project2D;

			for (auto p : s)
			{
				v2f toadd(Dot(content.mData->mVertices[p].mVertex, right), Dot(content.mData->mVertices[p].mVertex, up));
				project2D.push_back({ toadd,p });
			}

			ConvexHull2D	computeHull(project2D,0.05); // 20% precision
			auto hull=computeHull.getHull();

			for (auto h : hull)
			{
				newverticelist.push_back(content.mData->mVertices[std::get<1>(project2D[h])]);
			}

			surfindex++;
		}
		content.mData->mVertices = std::move(newverticelist);
	}

	for (auto& v : content.mData->mVertices)
	{
		mCellData.mPerVSurfaces[v.mVertex].insert(v.mSurfaceIndex);
	}
	
}


void		BuildMeshFromEnveloppe::setUpVertices(nodeInfo& node)
{
	mCellData.clear();

	MeshSimplificationOctreeNode* currentNode = node.getNode<MeshSimplificationOctreeNode>();
	const auto& content = currentNode->getContentType();

	initCellSurfaceList(node);

#ifdef _DEBUG
	node.mDebugFlag = 0;
#endif
	v3f result;
	switch (content.mData->mFreeFaceCount)
	{
	case 0:
		KIGS_ERROR("Can't occur", 1);
		break;
	case 1: // easy case with only one free face
		checkVertice(node, result);
		content.mData->mEnvelopeData->mGoodIntersectionPoint.push_back({ addVertice(result,&node), content.mData->mEmptyNeighborsFlag });
		
		break;
	case 2: // 2 free faces => 2 subcases for adjacent free faces or opposites
		if (content.mData->countOppositeFreeFaces())
		{
			manageSeparateFreeFaces(content, node);
		}
		else
		{
			checkVertice(node, result);
			addMultipleVertices(node, result);
		}
		break;
	case 3: // 3 free faces => 2 subcases corner or U
		if (content.mData->countOppositeFreeFaces())
		{
			manageSeparateFreeFaces(content, node);
		}
		else
		{
			checkVertice(node, result);
			addMultipleVertices(node, result);
		}
		break;
	case 4: // 4 free faces => 2 subcases corner or tunnel 
	{
		bool testTrivial = true;
		if (content.mData->countOppositeFreeFaces() == 2) // tunnel
		{
			testTrivial = false;
		}
		
		if (testTrivial && checkVerticeTrivialCase(node, result)) // if opposite free faces but trivial case, set only one point
		{
			addMultipleVertices(node, result);
		}
		else
		{
			manageSeparateFreeFaces(content, node);
		}

	}
		break;
	case 5: // 5 free faces => 5 points
	{
		bool testTrivial = true;
		
		if (testTrivial && checkVerticeTrivialCase(node, result)) 
		{
			addMultipleVertices(node, result);
		}
		else
		{
			manageSeparateFreeFaces(content, node);
		}
	}
	break;
	case 6:
		KIGS_ERROR("Can't occur", 1);
		break;

	}
	mCellData.clear();
}

v3f BuildMeshFromEnveloppe::searchGoodVerticeInCellForDirection(nodeInfo node, const MSOctreeContent& cnode, const v3f& direction, BBox currentBbox)
{
	v3f nodecoord(node.coord.x, node.coord.y, node.coord.z);
	nodecoord *= 0.5f;
	v3f	result;
	// put vertices in a vector
	std::vector<std::pair<v3f,u8>> vvector;
	
	for (const auto& v : mCellData.mPerVSurfaces)
	{
		v3f toAdd = v.first - nodecoord;
		currentBbox.Update(toAdd);
		u8 toAddFlag = 0;
		v3f toCheck(fabsf(toAdd.x), fabsf(toAdd.y), fabsf(toAdd.z));
		toCheck -= 0.5f;
		toCheck.x = fabsf(toCheck.x);
		toCheck.y = fabsf(toCheck.y);
		toCheck.z = fabsf(toCheck.z);

		if ((toCheck.x < 0.05f) || (toCheck.y < 0.05f) || (toCheck.z < 0.05f))
		{
			toAddFlag = 1;
		}

		vvector.push_back({ toAdd,toAddFlag });

	}

	// add bbox center to the list 

	v3f bboxCenter = currentBbox.Center();

	vvector.push_back({ bboxCenter,2 });

	// sort vector according to direction
	std::sort(vvector.begin(), vvector.end(), [direction](const std::pair<v3f, u8>& a, const std::pair<v3f, u8>& b)->bool {

		float vala = Dot(direction, a.first);
		float valb = Dot(direction, b.first);
		return vala < valb; });

	// compute transform matrix (should be done just by swapping vector composant index)
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
	transform = Transpose(transform);

	for (auto& v : vvector)
	{
		// transform vertices in new coordinate system
		transform.TransformVector((Vector3D*) &v.first);
	}

	transform.TransformVector((Vector3D*)&bboxCenter);

	searchGoodVerticeInSortedList search(vvector, bboxCenter);

	if (search.getFoundVertice(result))
	{
		
	}
	else
	{
		result = bboxCenter;
	}

	transform = Transpose(transform);
	transform.TransformVectors((Vector3D*)&result, 1);
	result += nodecoord;

	return result;
}


// utility class to search for a good plane in a cell
bool BuildMeshFromEnveloppe::searchGoodVerticeInSortedList::findSecondPoint()
{
	float	bestDelta = 1000.0f;
	float	bestDist = 0.0f;
	size_t	bestIndex = mCurrentIndex;

	for (size_t i = mCurrentIndex + 1; i < mList.size(); i++)
	{
		float sqrd = DistSquare(mList[mCurrentIndex].first.yz, mList[i].first.yz);
		if (sqrd > 0.001f) // not the same point
		{
			float sqrdelta = (mList[mCurrentIndex].first.x - mList[i].first.x);
			sqrdelta *= sqrdelta;

			float scr1 = sqrdelta * bestDist;
			float scr2 = bestDelta * sqrd;

			if ((scr1 < scr2) || ((scr1 == scr2) && (sqrd > bestDist)))
			{
				bestDist = sqrd;
				bestDelta = sqrdelta;
				bestIndex = i;
			}
		}
		
	}
	if (bestIndex != mCurrentIndex)
	{
		mPointList[1].i = bestIndex;
		mPointList[1].bestSQRDist = bestDist;
		mPointList[1].bestSQRDelta = bestDelta;
		return false;
	}
	// retry with another point
	return true;
}

bool BuildMeshFromEnveloppe::searchGoodVerticeInSortedList::findThirdPoint()
{

	bool isbboxcenter = mList[mPointList[1].i].second == 2;
	v2f firstTwoVector(mList[mPointList[1].i].first.yz);
	firstTwoVector -= mList[mPointList[0].i].first.yz;
	float invNormFTV = 1.0f / Norm(firstTwoVector);
	float deltaX = mList[mPointList[1].i].first.x - mList[mPointList[0].i].first.x;
	deltaX *= invNormFTV;
	firstTwoVector *= invNormFTV;

	v2f normalV(-firstTwoVector.y, firstTwoVector.x);

	v2f toCenter = mBBoxCenter.yz - mList[mPointList[0].i].first.yz;
	float dist = Dot(toCenter, normalV);
	
	// segment is on the bbox center ?
	if (fabsf(dist) < 0.0001f)
	{
		return true; // we don't need a third point then
	}
	
	// set normal pointing to bbox center
	if (dist < 0.0f)
	{
		dist = -dist;
		normalV = -normalV;
	}

	float	bestDelta = 1000.0f;
	float	bestDist = dist * dist;
	size_t	bestIndex = mCurrentIndex;

	for (size_t i = mCurrentIndex + 1; i < mList.size(); i++)
	{
		if (isbboxcenter && (mList[i].second == 2))
			break;

		if (i == mPointList[1].i) // don't check already done 
			continue;

		v2f testV(mList[i].first.yz);
		testV -= mList[mCurrentIndex].first.yz;

		float tstdist = Dot(testV, normalV); // the tested point is on the good side of the bbox center
		if (tstdist >= dist)
		{
			// squared dist to segment
			tstdist *= tstdist;

			// project point on segment
			float ptOnSegment = Dot(testV, firstTwoVector);
			// compute projected point x
			float projX = mList[mPointList[0].i].first.x + deltaX * ptOnSegment;

			float sqrdelta = (projX - mList[i].first.x);
			sqrdelta *= sqrdelta;

			float scr1 = sqrdelta * bestDist;
			float scr2 = bestDelta * tstdist;

			if ((scr1 < scr2) || ((scr1 == scr2) && (tstdist > bestDist)))
			{
				bestDist = tstdist;
				bestDelta = sqrdelta;
				bestIndex = i;
			}
		}

		
	}
	if (bestIndex != mCurrentIndex)
	{
		mPointList[2].i = bestIndex;
		mPointList[2].bestSQRDelta = bestDelta;
		mPointList[2].bestSQRDist = bestDist;

		return false;
	}

	return true;
}

BuildMeshFromEnveloppe::searchGoodVerticeInSortedList::searchGoodVerticeInSortedList(const std::vector<std::pair<v3f, u8>>& vlist, const v3f& bboxCenter) :mList(vlist)
{
	mFound = true;

	mBBoxCenter = bboxCenter;

	mCurrentIndex = 0;
	bool tryagain = true;
	while (tryagain)
	{
		tryagain = false;
		mPointList[0].i = mCurrentIndex;
		mPointList[0].bestSQRDist = mPointList[0].bestSQRDelta = 0.0f;

		if (vlist[mCurrentIndex].second == 2)
		{
			mFound = true;
			mFoundV = mList[mCurrentIndex].first;
			return;
		}

		tryagain = findSecondPoint();
		if (!tryagain)
		{
			tryagain = findThirdPoint();

			// if first point is not on a cell border, check if we can just choose it 
			if (mCurrentIndex == 0)
			{
				if (((4.0f * mPointList[1].bestSQRDelta) > mPointList[1].bestSQRDist) && ((mList[0].second&1) == 0)) // slope is high
				{
					mFound = true;
					mFoundV = mList[0].first;
					return;
				}
				else // small slope
				{
					bool getMiddle = tryagain;
					if (!tryagain)
					{
						if ((4.0f * mPointList[2].bestSQRDelta) > mPointList[2].bestSQRDist)
						{
							getMiddle = true;
						}
					}

					if (getMiddle)  // return middle of first & second point 
					{
						tryagain = false;
						mFound = true;
						mFoundV = (mList[0].first + mList[mPointList[1].i].first) * 0.5f;
						return;
					}
				}
			}
		}

		if (tryagain)
		{
			mCurrentIndex++;
			if (mCurrentIndex > vlist.size() - 3)
			{
				break;
			}
		}
	}

	if (tryagain)
	{
		// out of the loop with tryagain to true => no plane found
		mFound = false;
	}
	else
	{
		mFoundV = mList[mPointList[0].i].first + mList[mPointList[1].i].first + mList[mPointList[2].i].first;
		mFoundV *= 1.0f / 3.0f;
	}
}

void BuildMeshFromEnveloppe::manageSeparateFreeFaces(const MSOctreeContent& node, const nodeInfo& n)
{
	v3f direction(0.0f, 0.0f, 0.0f);

	for (u32 i = 0; i < 6; i++)
	{
		if (node.mData->mEmptyNeighborsFlag & (1 << i))
		{
			v3i idirection = OctreeNodeBase::mNeightboursDecalVectors[OctreeNodeBase::mInvDir[i]];
			direction = v3f(idirection.x, idirection.y, idirection.z);
			BBox emptybbox(BBox::PreInit{});
			v3f pos = searchGoodVerticeInCellForDirection(n,node, direction, emptybbox);
			node.mData->mEnvelopeData->mGoodIntersectionPoint.push_back({ addVertice(pos,&n),  (1 << i) });
		}
	}	
}

void	BuildMeshFromEnveloppe::addFinalizedMesh(std::vector<v3f>& vertices, std::vector<u32>& indices)
{
	u32 verticesStartSize = vertices.size();
	std::vector<u32>	FinalVerticeIndexMatching;
	FinalVerticeIndexMatching.resize(mVertices.size(),-1);

	for (const auto& f : mFaces)
	{
		for (const auto& e : f.edges)
		{
			u32 ew = e >> 31;
			u32 ei = e & 0x7fffffff;

			const auto& currentE = mEdges[ei];
			u32 vi = mFinalMergedVIndex[currentE.v[ew]];

			if (FinalVerticeIndexMatching[vi] == -1)
			{
				FinalVerticeIndexMatching[vi] = vertices.size();
				vertices.push_back(mVertices[vi].mV);
			}
			indices.push_back(FinalVerticeIndexMatching[vi]);
		}
	}
}


void BuildMeshFromEnveloppe::Build()
{
#ifdef _DEBUG
	u32 debug_index=0;
#endif

	// first setup all vertices in enveloppe nodes
	for (auto& n : mNodeList)
	{
		setUpVertices(*n);
#ifdef _DEBUG
		debug_index++;
#endif
	}

	if (mVertices.size() < 4)
	{
		mVertices.clear();
		return;
	}

#ifdef _DEBUG
	debug_index = 0;
#endif
	// then setup edges for each nodes
	{
		// edge cache for faster insertion
		std::map < u32, std::vector<u32>> edgeMap;
		for (const auto& n : mNodeList)
		{
			setUpEdges(*n, edgeMap);
#ifdef _DEBUG
			debug_index++;
#endif
		}
	}
	// setup faces
	setUpFaces();

	// setup now, treat later
	setupInnerCorners();
	
	// merge vertices and edges, remove useless faces
	firstClean();

	// for each vertice, compute normals using edge list
	setUpNormals();

	// in debug, check everything is fine
	checkVerticeCoherency();

	// do triangulation
	splitFaces();
	
	// compute per triangle normal
	computeTriangleNormals();

	// prepare triangle decimation
	tagVerticesForSimplification();

	// do mesh simplification by merging triangles
	mergeTriangles();

	// now move inner corners
	moveInnerCorners();

	// merge vertices at same position and flat/point triangles
	finalClean();
}
