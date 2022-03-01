#include "BuildMeshFromEnveloppe.h"
#include "TecLibs/2D/BBox2D.h"

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
			std::vector<std::pair<u32, float>> surf = content.mData->getPrincipalSurfacesForDirection(node, direction, mAllSurfaces);
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
	for (const auto& v : content.mData->mEnvelopeData->mPerVSurfaces)
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
	if (content.mData->mEnvelopeData->mAllSurfaces.size() > content.mData->mFreeFaceCount)
	{
		return false;
	}

	// retreive vertices in all available surfaces
	goodOne.Set(0.0f, 0.0f, 0.0f);
	u32 countGoodOnes = 0;
	for (const auto& v : content.mData->mEnvelopeData->mPerVSurfaces)
	{
		if (v.second.size() == content.mData->mEnvelopeData->mAllSurfaces.size()) // this vertice belongs to all the surfaces 
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

// 
void BuildMeshFromEnveloppe::setUpInternCellEdge(u8 mask, const MSOctreeContent& node, std::map<std::pair<u32, u8>, std::set<u32>>& foundEdges)
{
	for (u32 i = 0; i < 6; i++)
	{
		// create edges for this point
		if (mask & (1 << i))
		{
			std::pair<u32, u8>* startP = node.mData->getVertexForFreeFace(1 << i);
			if (!startP)
			{
				printf("WTF");
			}
			for (auto adj : adjacent_faces[i])
			{
				if (node.mData->mEmptyNeighborsFlag & adj) // this adjacent face is free
				{
					std::pair<u32, u8>* endP = node.mData->getVertexForFreeFace(adj);
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

void	BuildMeshFromEnveloppe::setUpEdges(nodeInfo node)
{
	MeshSimplificationOctreeNode* currentNode = node.getNode<MeshSimplificationOctreeNode>();
	const auto& content = currentNode->getContentType();

	bool needmerge = false;
	std::map<std::pair<u32, u8>, std::vector<std::pair<u32, u8>>>	foundEdges;

	// treat each face separately
	for (u32 i = 0; i < 6; i++)
	{
		// this face is free and concern current point
		if (content.mData->mEmptyNeighborsFlag & (1 << i)) // this face is free
		{
			std::pair<u32, u8> ip = *(content.mData->getVertexForFreeFace(1 << i));
			ip.second = 1 << i;
			for (auto adj : adjacent_faces[i])
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
						foundEdges[ip].push_back({ endP->first,0 }); // for outter vertice, don't set flag
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
							foundEdges[ip].push_back({ endP->first,0 }); // outter vertice => don't set flag
						}
						else
						{
							printf("WTF");
						}
					}
				}
				else // free adjacent face, get vertex in current cell
				{
					std::pair<u32, u8>* endP = content.mData->getVertexForFreeFace(adj);
					if (!endP)
					{
						printf("WTF");
					}
					if (endP->first == ip.first)
					{
						needmerge = true;
					}
					foundEdges[ip].push_back({ endP->first,adj });
				}
			}
		}
		
	}

	if (needmerge) // merge separate free faces with unique vertice
	{
		std::map<std::pair<u32, u8>, std::vector<std::pair<u32, u8>>>	mergedEdges;
		std::set<std::pair<u32, u8>> treated;

		u32 startpI = (* (foundEdges.begin())).first.first;

		std::function<void(std::pair<u32, u8> , std::pair<u32, u8>,int )> insertNext;

		insertNext = [&](std::pair<u32,u8> from, std::pair<u32, u8> to,int index)->void {
			
			auto& toInsertList = foundEdges[to];
			// search index
			if (index < 0)
			{
				for (index = 0; index < toInsertList.size(); index++)
				{
					if (toInsertList[index] == from)
						break;
				}
			}
			treated.insert(to);

			for (u32 i = 0; i < toInsertList.size(); i++)
			{
				auto& endp = toInsertList[(i + index) % toInsertList.size()];
				if (treated.find(endp) == treated.end())
				{
					if (endp.first == startpI) // 
					{
						// recurse
						insertNext(to, endp,-1);
					}
					else
					{
						mergedEdges[{startpI, 0}].push_back(endp);
					}
					treated.insert(endp);
				}
			}
		};

		for (auto& startp : foundEdges)
		{
			insertNext(startp.first, startp.first, 0);
		}

		foundEdges = mergedEdges;
	}
	
	// add edges for each point separately 
	
	for (auto& startp : foundEdges)
	{
		for (auto& endp : startp.second)
		{
			// add edge to edge list
			u32 edgeindex = addEdge(startp.first.first, endp.first);
			// and push it in the right order for later face building
			mVertices[startp.first.first].mEdges.push_back(edgeindex);
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
		if (mFaces[fi].edges.size() == 3) // triangle, don't need to split
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
				printf("WTF");
			}

		}
		vindex++;
	}
#endif
}

void	BuildMeshFromEnveloppe::checkCoherency()
{
	// check coherency

	for (const auto& v : mVertices)
	{
		if ( (v.mEdges.size() < 3) && (v.mEdges.size() !=0))
		{
			printf("WTF");
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

	u32 edgeindex = addEdge(v1, v2,false); // always add a new edge here

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

		if ((mEdges[ei].t[ew] != -1) && ((mEdges[ei].t[ew] != oldfi)))
		{
			printf("WTF");
		}

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
		u32 edgeindex = addEdge(decalvlist[0], decalvlist[decalvlist.size()-2],false );// always add a new edge here

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

void BuildMeshFromEnveloppe::finishTriangleSetup()
{
	u32 fi = 0;
	for (auto& f:mFaces)
	{
		v3f v1= getEdgeVector(f.edges[0]);
		v3f v2= getEdgeVector(f.edges[1]);
		v3f v3 = getEdgeVector(f.edges[2]); // just to setup edge direction
		f.normal.CrossProduct(-v1, v2);

		if (NormSquare(f.normal) ==0.0f)
		{
			printf("WTF");
		}

		f.normal.Normalize();

		for (u32 e = 0; e < 3; e++)
		{
			u32 ew = (f.edges[e] >> 31);
			u32 ei = (f.edges[e] & 0x7fffffff);

			mEdges[ei].t[ew] = fi;
		}
		++fi;
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
#ifdef _DEBUG
		//if (vi == 538)
		//	printf("");
#endif

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
			currentE.FlagAsOnCorner = true;
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
				// first : edge index in current vertice list
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
					flattenTriangles(f, currentE.t[ti], vindex,mergeWith.second, v.mEdges[mergeWith.first] & 0x7fffffff);
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

				// this vertice is now invalid
				v.mFlag = 16;
				v.mEdges.clear();

				mEdges[shrinkEdge & 0x7fffffff].v[0] = mEdges[shrinkEdge & 0x7fffffff].v[1] = 0;

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
		else
		{
			printf("WTF");
		}
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
	u32		bestFoundFlag=8;
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
	u32		bestFoundFlag = 8;
	u32		bestFoundEdgeCount = 10000;

	// for each edge
	u32 internEdgeIndex = 0;
	for (auto& e : v.mEdges)
	{
		u32 ew = (e >> 31);
		u32 ei = (e & 0x7fffffff);

		const MSEdge& currentE = mEdges[ei];

		if (currentE.FlagAsOnCorner==false)
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
		if (current.FlagAsOnCorner)
		{
			listIndex++; 
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

					if (e->t[ew] != -1)
					{
						printf("WTF");
					}
					prevVertice = nextVertice;
					nextVertice = e->v[1 - ew];

					currentEdgeIndexInFace++;
					if (currentEdgeIndexInFace > 7)
					{
						printf("WTF");
						break;
					}
				}

				mFaces.push_back(toAdd);

			}


		}
		vindex++;
	}
}

void		BuildMeshFromEnveloppe::setUpVertices(nodeInfo& node)
{
	MeshSimplificationOctreeNode* currentNode = node.getNode<MeshSimplificationOctreeNode>();
	const auto& content = currentNode->getContentType();


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
		content.mData->mEnvelopeData->mGoodIntersectionPoint.push_back({ addVertice(result,node), content.mData->mEmptyNeighborsFlag });
		
		break;
	case 2: // 2 free faces => 2 subcases for adjacent free faces or opposites
		if (content.mData->countOppositeFreeFaces())
		{
			manageSeparateFreeFaces(content, node);
		}
		else
		{
			checkVertice(node, result);
			content.mData->mEnvelopeData->mGoodIntersectionPoint.push_back({ addVertice(result,node), content.mData->mEmptyNeighborsFlag });
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
			content.mData->mEnvelopeData->mGoodIntersectionPoint.push_back({ addVertice(result,node), content.mData->mEmptyNeighborsFlag });
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
			content.mData->mEnvelopeData->mGoodIntersectionPoint.push_back({ addVertice(result,node), content.mData->mEmptyNeighborsFlag });
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
			content.mData->mEnvelopeData->mGoodIntersectionPoint.push_back({ addVertice(result,node), content.mData->mEmptyNeighborsFlag });
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
	validateGoodIntersectionPoint(content, node);

}

v3f BuildMeshFromEnveloppe::searchGoodVerticeInCellForDirection(nodeInfo node, const MSOctreeContent& cnode, const v3f& direction, BBox currentBbox)
{
	v3f nodecoord(node.coord.x, node.coord.y, node.coord.z);
	nodecoord *= 0.5f;
	v3f	result;
	// put vertices in a vector
	std::vector<std::pair<v3f,u8>> vvector;
	
	for (const auto& v : cnode.mData->mEnvelopeData->mPerVSurfaces)
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

void	BuildMeshFromEnveloppe::separateVertices(const MSOctreeContent& node, const nodeInfo& n)
{
#ifdef _DEBUG
	if (node.mData->mEnvelopeData->mGoodIntersectionPoint.size() < 2)
		return;
#endif
	
	struct vseparator
	{
		v3f			pos;
		size_t		index;
		v3f			direction;
	};
	v3f nodecenter(n.coord.x, n.coord.y, n.coord.z);
	nodecenter *= 0.5f;

	std::vector<vseparator>	vlist;
	
	// build list
	for (size_t i1 = 0; i1 < node.mData->mEnvelopeData->mGoodIntersectionPoint.size(); i1++)
	{
		vseparator toAdd;
		toAdd.index = node.mData->mEnvelopeData->mGoodIntersectionPoint[i1].first;
		toAdd.pos = mVertices[toAdd.index].mV;
		toAdd.direction.Set(0.0f, 0.0f, 0.0f);
		for (u32 i = 0; i < 6; i++)
		{
			if (node.mData->mEnvelopeData->mGoodIntersectionPoint[i1].second & (1 << i))
			{
				v3i idirection = OctreeNodeBase::mNeightboursDecalVectors[i];
				toAdd.direction += v3f(idirection.x, idirection.y, idirection.z);
			}
		}
		toAdd.direction.Normalize();
		vlist.push_back(toAdd);
	}
	
	// now separate points
	for (auto& v : vlist)
	{
		v.pos += v.direction * 0.1f;
		v3f dir(v.pos);
		dir -= nodecenter;
		float sqrn = NormSquare(dir);
		if (sqrn > 0.25f)
		{
			dir *= 1.0f / sqrtf(sqrn);
			dir *= 0.5f;
			v.pos = nodecenter + dir;
		}
		mVertices[v.index].mV = v.pos;
	}

}

void BuildMeshFromEnveloppe::validateGoodIntersectionPoint(const MSOctreeContent& node, const nodeInfo& n)
{
	// check if some mGoodIntersectionPoint are the same and separate them if needed
	if (node.mData->mEnvelopeData->mGoodIntersectionPoint.size() > 1)
	{
		bool separationDone = false;
		for (size_t i1 = 0; i1 < node.mData->mEnvelopeData->mGoodIntersectionPoint.size(); i1++)
		{
			const v3f& v1 = mVertices[node.mData->mEnvelopeData->mGoodIntersectionPoint[i1].first].mV;
			for (size_t i2 = i1 + 1; i2 < node.mData->mEnvelopeData->mGoodIntersectionPoint.size(); i2++)
			{
				const v3f& v2 = mVertices[node.mData->mEnvelopeData->mGoodIntersectionPoint[i2].first].mV;
				float sqrDist = DistSquare(v1, v2);
				if (sqrDist < 0.0001)
				{
					separateVertices(node, n);
					separationDone = true;
					break;
				}
			}
			if (separationDone)
				break;
		}
	}

	// now check if a point is near a cell face
	v3f nodecenter(n.coord.x, n.coord.y, n.coord.z);
	nodecenter *= 0.5f;

	for (size_t i1 = 0; i1 < node.mData->mEnvelopeData->mGoodIntersectionPoint.size(); i1++)
	{
		v3f& v1 = mVertices[node.mData->mEnvelopeData->mGoodIntersectionPoint[i1].first].mV;

		v3f localCoord(v1);
		localCoord -= nodecenter;

		for (u32 i = 0; i < 3; i++)
		{
			if (fabsf(localCoord[i]) > 0.49f)
			{
				v1[i] = nodecenter[i] + localCoord[i] * 0.8f;
			}
		}
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
			node.mData->mEnvelopeData->mGoodIntersectionPoint.push_back({ addVertice(pos,n),  (1 << i) });
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
			u32 vi = currentE.v[ew];

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

#ifdef _DEBUG
	debug_index = 0;
#endif
	// then setup edges for each nodes
	for (const auto& n : mNodeList)
	{
		setUpEdges(*n);
#ifdef _DEBUG
		debug_index++;
#endif
	}
	
	// for each vertice, compute normals using edge list
	setUpNormals();
	// setup faces
	setUpFaces();
	checkVerticeCoherency();
	// do triangulation
	splitFaces();
	// compute per triangle normal
	finishTriangleSetup();

	// prepare triangle decimation
	tagVerticesForSimplification();

	// do mesh simplification by merging triangles
	mergeTriangles();
}
