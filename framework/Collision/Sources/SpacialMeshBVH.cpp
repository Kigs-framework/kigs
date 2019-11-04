#include "SpacialMeshBVH.h"
#include "ModernMesh.h"
#include "GLSLDebugDraw.h"

SpacialMeshBVH::SpacialMeshBVH(const Matrix3x4& LtoGMatrix) : CollisionBaseObject(), m_LtoGMatrix(LtoGMatrix), m_3DBox(BBox::PreInit())
{
}

SpacialMeshBVH::~SpacialMeshBVH()
{

}

SpacialMeshBVH* SpacialMeshBVH::BuildFromMesh(ModernMesh* mesh, const Matrix3x4& LtoGMatrix, bool onlyHorizontalAndVertical)
{
	int VCount;
	float* VArray = nullptr;
	int ICount;
	void* IArray = nullptr;

	mesh->GetTriangles(VCount, VArray, ICount, IArray);

	if (VCount == 0)
	{
		delete VArray;
		free(IArray);
		return nullptr;
	}

	SpacialMeshBVH * newBVH = new SpacialMeshBVH(LtoGMatrix);
	newBVH->BuildFromTriangleList((Point3D*)VArray, VCount, IArray, ICount / 3, onlyHorizontalAndVertical);
	delete VArray;
	free(IArray);
	return newBVH;
}


void SpacialMeshBVH::ReorderVerticesOnAxis(std::vector<v3f>& vertices, std::vector<unsigned int>& indices)
{
	struct vAndPos
	{
		v3f		 v;
		unsigned int old_index;
	};

	std::vector<vAndPos>	tmpVector;
	tmpVector.resize(vertices.size());

	// first copy vertices in sort structure
	for (size_t i = 0; i < vertices.size(); i++)
	{
		tmpVector[i].v = vertices[i];
		tmpVector[i].old_index = i;
	}

	// now sort vertices (x,y,z order)

	std::sort(tmpVector.begin(), tmpVector.end(), [&](const auto &a, const auto &b)
	{
		if (a.v[0] == b.v[0])
		{
			if (a.v[1] == b.v[1])
			{
				return a.v[2] < b.v[2];
			}
			return a.v[1] < b.v[1];
		}
		return a.v[0] < b.v[0];
	});

	// then rearange vertices and indices
	std::vector<unsigned int>	newIndices;
	newIndices.resize(vertices.size());
	for (size_t i = 0; i < vertices.size(); i++)
	{
		newIndices[tmpVector[i].old_index] = i;
		vertices[i] = tmpVector[i].v;
	}

	std::vector<unsigned int>	newIndicesVector;
	newIndicesVector.resize(indices.size());
	for (size_t i = 0; i < indices.size(); i++)
	{
		newIndicesVector[i] = newIndices[indices[i]];
	}
	indices = std::move(newIndicesVector);
}

int	SpacialMeshBVH::CreateVirtualIndicesList(std::vector<int>& virtualIndices, const std::vector<v3f>& vertices)
{
	virtualIndices.resize(vertices.size());
	int wi = 0;
	int virtualIndice = 0;

	for (size_t i = 0; i < vertices.size(); ++i)
	{
		if (i != wi)
		{
			if (memcmp(&vertices[wi], &vertices[i], sizeof(v3f)) != 0)
			{
				wi = i;
				virtualIndice++;
			}
		}
		virtualIndices[i] = virtualIndice;

	}

	return virtualIndice + 1;
}


bool FitHeightPlane(int numPoints, Vector3D*  points, Vector3D& bary, float& barA0, float& barA1)
{
	// Compute  the  mean  of  the  points .
	Vector3D  mean ={0 ,  0 ,  0};
	for(int i = 0;  i<numPoints ; ++i )
	{
		mean += points[ i ] ;
	}
	mean /= numPoints ;
	// Compute  the  l i n e a r  system  matrix  and  vector  elements .
	float  xxSum = 0 , xySum = 0 , xhSum = 0 , yySum = 0 , yhSum = 0;
	for(int i = 0;  i<numPoints ; ++i )
	{
		Vector3D  diff = points[i]-mean;

		xxSum += diff[ 0 ]*diff[ 0 ] ;
		xySum += diff [ 0 ]*diff [ 1 ] ;
		xhSum += diff [ 0 ]*diff [ 2 ] ;
		yySum += diff[ 1 ]*diff[ 1 ] ;
		yhSum += diff [ 1 ]*diff [ 2 ] ;
	}
	//  Solve  the  l i n e a r  system .
	float  det = xxSum*yySum-xySum*xySum ;
	if( det  != 0)
	{
		// Compute  the  f i t t e d  plane  h( x , y ) = barH + barA0?( x?barX ) + barA1?( y?barY ) .
		bary.x = mean[0];
		bary.y = mean[1];
		bary.z = mean[2];
		barA0 = (yySum*xhSum-xySum*yhSum) / det; 
		barA1 = (xxSum*yhSum-xySum*xhSum) / det; 
		return    true;
		
	}
	else
	{
		// The  output  i s  i n v a l i d .   The  p o i n t s  are  a l l  the  same  or  they  are  c o l l i n e a r .
		bary.x = 0; bary.y = 0; bary.z;barA0 = 0;barA1 = 0;
		return    false;
	}
}

void SpacialMeshBVH::BuildHalfEdgeList(	const std::vector<unsigned int>& indices,
										const std::vector<v3f>& vertices,
										const std::vector<int>&	virtualIndices, 
										std::vector<SegmentCheck>& PreallocatedSegments,
										std::vector<ConnectedTriangle>&	PreallocatedTriangles, 
										std::vector<SegmentCheck*>& linkedSegmentList)
{
	// then fill the list 
	int ti = 0;
	for (int i = 0; i < indices.size(); i += 3)
	{
		ConnectedTriangle& currentT = PreallocatedTriangles[ti];
		// compute triangle normal and surface
		Vector3D s1(vertices[indices[i]], vertices[indices[i + 1]], asVector());
		Vector3D s2(vertices[indices[i]], vertices[indices[i + 2]], asVector());

		// init normals and weight
		currentT.N.CrossProduct(s1, s2);
		currentT.s = Norm(currentT.N);
		currentT.N *= 1.0f / currentT.s;
		currentT.tmpN = currentT.N;
		currentT.s *= 0.5f;
		currentT.s = sqrtf(currentT.s); // minimize big surface weight

		currentT.w[0] = currentT.w[1] = currentT.w[2] = 0.0f;
		currentT.h = 0.001f;

		for (int j = 0; j < 3; j++)
		{
			SegmentCheck& s = PreallocatedSegments[i + j];
			int sI1 = virtualIndices[indices[i + j]];
			int sI2 = virtualIndices[indices[i + ((j + 1) % 3)]];

			if (sI1 < sI2)
			{
				s.maxIndice = sI2;
				s.triangleIndice = ti;
				s.nextSegment = linkedSegmentList[sI1];
				s.segmentFlag = 0;
				s.Twin = 0;
				linkedSegmentList[sI1] = &s;
			}
			else
			{
				s.maxIndice = sI1;
				s.triangleIndice = ti;
				s.nextSegment = linkedSegmentList[sI2];
				s.segmentFlag = 1;
				s.Twin = 0;
				linkedSegmentList[sI2] = &s;
			}
		}
		ti++;
	}

	// search twin for each segment if any

	for (int i = 0; i < linkedSegmentList.size(); i++)
	{
		SegmentCheck* current = linkedSegmentList[i];
		while (current)
		{
			if (current->Twin == 0) // twin not found ?
			{
				SegmentCheck* other = current->nextSegment;
				while (other)
				{
					if (other->maxIndice == current->maxIndice)
					{
						if (((current->segmentFlag & 1) ^ (other->segmentFlag & 1)) == 1) // good match
						{
							if ((current->Twin == 0) && (other->Twin == 0)) // check if we don't already found a twin in this pass
							{
								current->Twin = other;
								other->Twin = current;
							}
							else // several twins ?
							{
								// get the more "coplanar" triangles
								// TODO
								/*
								int CurrentTi = current->triangleIndice;
								int otherTI = other->triangleIndice;


								if (current->Twin)
								{

								}
								*/
							}
						}
						else // duplicate segment ? 
						{
							// nothing to do here
						}
					}
					other = other->nextSegment;
				}
			}

			current = current->nextSegment;
		}
	}

}

void	SpacialMeshBVH::IterativeTriangleConnection(const std::vector<unsigned int>& indices,
													std::vector<SegmentCheck>& PreallocatedSegments,
													std::vector<ConnectedTriangle>&	PreallocatedTriangles)
{
	// now some iteration on normals and neighbors to create groups of triangles
	int IterationCount = 4;
	do
	{
		int ti = 0;
		for (int i = 0; i < indices.size(); i += 3)	// copy previously computed N to wN
		{
			ConnectedTriangle& currentT = PreallocatedTriangles[ti];
			currentT.wN = currentT.tmpN;

			ti++;
		}


		ti = 0;
		for (int i = 0; i < indices.size(); i += 3)	// update neighbors confidence
		{
			ConnectedTriangle& currentT = PreallocatedTriangles[ti];

			float minw = 2.0f;
			float maxw = -1.0f;

			for (int j = 0; j < 3; j++)
			{
				SegmentCheck& s = PreallocatedSegments[i + j];
				float D;

				if (s.Twin) // this edge is linked to another triangle
				{
					ConnectedTriangle& nT = PreallocatedTriangles[s.Twin->triangleIndice];
					D = Dot(currentT.wN, nT.wN);
					if (D < 0.0f)
					{
						D = 0.0f;
					}

					currentT.w[j] = D;
					if (D > maxw)
					{
						maxw = D;
					}
					if (D < minw)
					{
						minw = D;
					}
				}
				else // no neighbor ? bad homogeneity 
				{
					if (minw > 0.2f)
					{
						minw = 0.2f;
					}
				}
				
			}
			if (maxw != -1.0f) // at least one neighbor
				currentT.h = 1.0f - ((maxw - minw)*(maxw - minw));

			ti++;
		}

		ti = 0;
		for (int i = 0; i < indices.size(); i += 3)	// update weighted normal
		{
			ConnectedTriangle& currentT = PreallocatedTriangles[ti];

			currentT.tmpN = currentT.wN;
			currentT.tmpN *= currentT.s*currentT.h;

			for (int j = 0; j < 3; j++)
			{
				SegmentCheck& s = PreallocatedSegments[i + j];

				if (s.Twin) // this edge is linked to another triangle
				{
					ConnectedTriangle& nT = PreallocatedTriangles[s.Twin->triangleIndice];

					currentT.tmpN += (currentT.w[j] * currentT.w[j] * nT.s)*nT.wN;
				}
			}

			currentT.tmpN.Normalize();
			ti++;
		}

	} while (--IterationCount);
}


void	SpacialMeshBVH::FloodFillTriangleGroups(	int triangleCount,
													const std::vector<unsigned int>& indices,
													std::vector<SegmentCheck>& PreallocatedSegments,
													std::vector<ConnectedTriangle>&	PreallocatedTriangles,
													std::vector<TriangleGrp>& triangleFlag,
													std::vector< std::set<unsigned int> >&	mergedGroups)
{

	std::vector<int>	NextPassTriangle;
	NextPassTriangle.resize(triangleCount + 1);
	std::vector<int>	NextPassTriangleTmp;
	NextPassTriangleTmp.resize(triangleCount + 1);
	int	NextPassTriangleIndex = 0;

	int grpID = 1; // groupID start at 1 

	float threshold = 0.96f;
	int ti = 0;
	for (int i = 0; i < indices.size(); i += 3)
	{
		ConnectedTriangle& currentT = PreallocatedTriangles[ti];
		float currentSH = currentT.s*currentT.h;

		if (currentT.s == 0.0f) // invalid triangle
		{
			triangleFlag[ti].grpID = -1;
			triangleFlag[ti].rootTriangleIndex = ti;
		}
		else
		{

			bool is_LocalMax = true;


			for (int j = 0; j < 3; j++)
			{
				SegmentCheck& s = PreallocatedSegments[i + j];

				if (s.Twin) // this edge is linked to another triangle
				{
					if (currentT.w[j] > threshold)
					{
						ConnectedTriangle& nT = PreallocatedTriangles[s.Twin->triangleIndice];
						float nSH = nT.s*nT.h;
						if (nSH > currentSH)
						{
							is_LocalMax = false;
							break;
						}
					}
				}
			}

			if (is_LocalMax)
			{
				NextPassTriangle[NextPassTriangleIndex] = ti;
				NextPassTriangleIndex++;
				triangleFlag[ti].grpID = grpID;
				triangleFlag[ti].rootTriangleIndex = ti;
				++grpID;
			}
		}

		ti++;
	}



	// extend local max influence to neighbors 
	
	mergedGroups.resize(grpID);

	bool needAnotherPass = true;
	while (needAnotherPass) // flag neighbors
	{
		needAnotherPass = false;
		int currentPassCount = NextPassTriangleIndex;
		NextPassTriangleIndex = 0;
		for (int i = 0; i < currentPassCount; i++)
		{
			ti = NextPassTriangle[i];
			ConnectedTriangle& currentT = PreallocatedTriangles[ti];

			for (int j = 0; j < 3; j++)
			{
				if (currentT.w[j] > threshold)
				{
					SegmentCheck& s = PreallocatedSegments[ti * 3 + j];

					if (triangleFlag[s.Twin->triangleIndice].grpID == 0) // no group set ?
					{
						triangleFlag[s.Twin->triangleIndice].grpID = triangleFlag[ti].grpID;
						triangleFlag[s.Twin->triangleIndice].rootTriangleIndex = triangleFlag[ti].rootTriangleIndex;

						NextPassTriangleTmp[NextPassTriangleIndex] = s.Twin->triangleIndice;
						NextPassTriangleIndex++;

						needAnotherPass = true;
					}
					else if (triangleFlag[s.Twin->triangleIndice].grpID > 0) // already have a group
					{
						int grp1 = triangleFlag[s.Twin->triangleIndice].grpID;
						int grp2 = triangleFlag[ti].grpID;
						if (grp1 != grp2)
						{
							// test if groups can merge

							int root1 = triangleFlag[s.Twin->triangleIndice].rootTriangleIndex;
							int root2 = triangleFlag[ti].rootTriangleIndex;

							float dot = Dot(PreallocatedTriangles[root1].wN, PreallocatedTriangles[root2].wN);

							if (dot > (threshold*0.95))
							{
								if (grp1 > grp2)
								{
									mergedGroups[grp2].insert(grp1);
								}
								else
								{
									mergedGroups[grp1].insert(grp2);
								}
							}
						}
					}
				}
			}

		}

		NextPassTriangle = std::move(NextPassTriangleTmp);
		NextPassTriangleTmp.resize(NextPassTriangle.size());

	}

}

int	SpacialMeshBVH::MergeConnectedGroups(	const std::vector< std::set<unsigned int> >&	mergedGroups,
												std::vector < std::pair<int, int> >& MergeGroupIndex)
{

	std::function<int(int, int)> SearchRecurseGroups;
	SearchRecurseGroups = [&](int currentG, int GID)->int
	{
		int result = GID;
		if (MergeGroupIndex[currentG].second == 0)
		{
			for (auto g : mergedGroups[currentG])
			{
				int lresult = SearchRecurseGroups(g, result);
				if (lresult < result) // already connected group
				{
					result = lresult;
				}
			}
		}
		else if(MergeGroupIndex[currentG].second < result)
		{
			result = MergeGroupIndex[currentG].second;
		}
		return result;
	};


	std::function<void(int,int)> recurseGroups;
	recurseGroups = [&](int currentG,int GID)->void
	{
		if (MergeGroupIndex[currentG].second == 0)
		{
			MergeGroupIndex[currentG].second = GID;
			for (auto g : mergedGroups[currentG])
			{
				recurseGroups(g, GID);
			}
		}
	};

	int currentGID = 1;
	for (int i = 1; i < mergedGroups.size(); i++)
	{
		if (MergeGroupIndex[i].second == 0)
		{
			int setGID = SearchRecurseGroups(i, currentGID);
			recurseGroups(i, setGID);

			if(setGID == currentGID)
				currentGID++;
		}
	}

	return currentGID;

}

void	SpacialMeshBVH::CreatePlanarGroups(	const std::vector<unsigned int>& indices,
											const std::vector<v3f>& vertices,
											std::vector<TriangleGrp>& triangleFlag,
											std::vector < std::pair<int, int> >& MergeGroupIndex,
											std::vector<SegmentCheck>& PreallocatedSegments,
											std::vector<ConnectedTriangle>&	PreallocatedTriangles,
											std::vector<PlanarGroup >& planarGroupList,
											bool onlyHorizontalAndVertical)
{
	int ti = 0;
	for (int i = 0; i < indices.size(); i += 3)
	{
		auto &t = triangleFlag[ti];
		if (t.grpID <= 0) // bad triangles
		{
			++ti;
			continue;
		}

		int bestGrpID = 0;

		t.isValidTriangle = true;

		if (MergeGroupIndex[t.grpID].first > 2) // at least 3 triangles to find a plane
		{
			bestGrpID = t.grpID;
		}
		else // connect this triangle to the best matching group if any
		{
			float bestGroupCoef = 0.0f;
			ConnectedTriangle& currentT = PreallocatedTriangles[ti];
			for (int j = 0; j < 3; j++)
			{
				SegmentCheck& s = PreallocatedSegments[ti * 3 + j];

				if (s.Twin)
				{
					int otherGrpID = triangleFlag[s.Twin->triangleIndice].grpID;
					if ((otherGrpID != t.grpID) && (otherGrpID>0))
						if (MergeGroupIndex[otherGrpID].first > 2)
						{
							float groupCoef = currentT.w[j] * MergeGroupIndex[otherGrpID].first;
							if (groupCoef > bestGroupCoef)
							{
								bestGroupCoef = groupCoef;
								bestGrpID = otherGrpID;
								t.isValidTriangle = false;
							}
						}
				}
			}
		}

		// sort more "stable" triangles
		

		if (bestGrpID != 0)
		{
			t.grpID = bestGrpID;

			PlanarGroup& pg= planarGroupList[bestGrpID];

			if (pg.triangles.size())
			{
				pg.triangles.push_back(ti);
			}
			else
			{
				pg.triangles.reserve(MergeGroupIndex[bestGrpID].first);
				pg.triangles.push_back(ti);

				pg.surface = 0.0f;
				pg.normal.Set(0,0,0);
				pg.origin.Set(0, 0, 0);
				
				pg.is_H_Or_V = false;

				pg.validcount = 0;

			}


			if (t.isValidTriangle)
			{
				std::pair<float, int> toInsert;
				toInsert.first = 1.0f - Dot(PreallocatedTriangles[ti].wN, PreallocatedTriangles[ti].N);
				toInsert.second = ti;
				pg.sortStableTriangles.insert(toInsert);
				pg.validcount++;
			}
			else
			{
				pg.invalid_triangles.push_back(ti);
			}
		}

		++ti;
	}

	// normalize normal and compute planar equation
	int countHV = 0;
	for (auto& g : planarGroupList)
	{
		if (g.triangles.size() == 0)
		{
			continue;
		}
		// take 60% of "best case" triangles
		int i = 0;
		for (auto& p : g.sortStableTriangles)
		{
			ti = p.second;
			g.normal += PreallocatedTriangles[ti].N*PreallocatedTriangles[ti].s;
			g.surface += PreallocatedTriangles[ti].s;
			Point3D	tc(vertices[indices[ti * 3]]);
			tc += vertices[indices[ti * 3 + 1]];
			tc += vertices[indices[ti * 3 + 2]];
			tc *= PreallocatedTriangles[ti].s;

			g.origin += tc;
			if ((i * 100) > (g.validcount * 60))
			{
				break;
			}
			++i;
		}

		g.origin *= 1.0f / (3.0f*g.surface);
		g.normal.Normalize();
#ifdef USE_LEAST_SQUARES
		if (fabsf(g.normal.z) > 0.75)
		{
			g.u.Set(1.0f, 0.0f, 0.0f);
			g.v.CrossProduct(g.normal, g.u);
			gv.Normalize();
			g.u.CrossProduct(g.v, g.normal);
		}
		else
		{
			g.u.Set(0.0f, 0.0f, 1.0f);
			g.v.CrossProduct(g.normal, g.u);
			g.v.Normalize();
			g.u.CrossProduct(g.v, g.normal);
		}
#endif
		if (onlyHorizontalAndVertical) // check if normal is horizontal or vertical
		{
			Vector3D gnormal(g.normal);

			m_LtoGMatrix.TransformVector(&gnormal);

			float ay = fabsf(gnormal.y);
			if ((ay > 0.95f) || (ay < 0.05))
			{
				g.is_H_Or_V = true;
				countHV++;
			}
		}

#ifdef USE_LEAST_SQUARES

		// least square fitting
		// first collect each triangle center
		std::vector<Vector3D>	verticelistForLSQ;
		verticelistForLSQ.reserve(g.triangles.size());

		for (auto ti : g.triangles)
		{
			Vector3D& current = verticelistForLSQ.emplace_back();
			current = vertices[indices[ti * 3]];
			current += vertices[indices[ti * 3 + 1]];
			current += vertices[indices[ti * 3 + 2]];
			current *= 1.0f / 3.0f;
		}

		Vector3D TestNormal(g.normal);
		Matrix3x3 transformNormalToZ(g.u, g.v, g.normal);
		Matrix3x3 trspose = Transpose(transformNormalToZ);
		// transform points in a coordinate system where normal is the z axis
		trspose.TransformPoints(&verticelistForLSQ[0], verticelistForLSQ.size());
		trspose.TransformVector(&TestNormal);

		Vector3D	bary, resultNormal;

		FitHeightPlane(verticelistForLSQ.size(), &verticelistForLSQ[0], bary, resultNormal.x, resultNormal.y);

		Vector3D	newVX(1, 0, resultNormal.x);
		Vector3D	newVY(0, 1, resultNormal.y);
		newVX.Normalize();
		newVY.Normalize();
		Vector3D NewNormal;
		NewNormal.CrossProduct(newVX, newVY);

		// convert back result

		transformNormalToZ.TransformVector(&TestNormal);
		transformNormalToZ.TransformVector(&NewNormal);
		transformNormalToZ.TransformPoints(&bary, 1);

		g.origin = bary;
		g.normal = NewNormal;
#endif
	}


}

void	SpacialMeshBVH::MergeTwoPlanarGroups(std::vector<TriangleGrp>& triangleFlag, std::vector<SegmentCheck>& PreallocatedSegments, PlanarGroup& grp1, const PlanarGroup& grp2,int grpID1,int grpID2)
{
	grp1.normal *= grp1.surface;
	grp1.normal += grp2.normal * grp2.surface;
	grp1.normal.Normalize();
	grp1.origin *= grp1.surface;
	grp1.origin += grp2.origin * grp2.surface;
	grp1.surface += grp2.surface;
	grp1.origin /= grp1.surface;

	// remove common edges
	std::vector<std::pair<int, int>> newBorders;
	for (auto& edge : grp1.borderEdges)
	{
		SegmentCheck& s = PreallocatedSegments[edge.first * 3 + edge.second];
		if (triangleFlag[s.Twin->triangleIndice].grpID != grpID2)
		{
			newBorders.push_back(edge);
		}
	}
	for (auto& edge : grp2.borderEdges)
	{
		SegmentCheck& s = PreallocatedSegments[edge.first * 3 + edge.second];
		if (triangleFlag[s.Twin->triangleIndice].grpID != grpID1)
		{
			newBorders.push_back(edge);
		}
	}

	grp1.borderEdges = std::move(newBorders);
	grp1.triangles.insert(grp1.triangles.end(), grp2.triangles.begin(), grp2.triangles.end());

	for (int ti : grp2.triangles)
	{
		triangleFlag[ti].grpID = grpID1;
	}
}

void SpacialMeshBVH::BuildFromTriangleList(Point3D* vertexList, int vertexCount, void* indexList, int triangleCount, bool onlyHorizontalAndVertical)
{

	std::vector<v3f>				vertices;	// 3d coords
	std::vector<unsigned int>		indices;	// indice 

	// construct inner structures
	vertices.resize(vertexCount);
	memcpy(&vertices[0], vertexList, vertexCount * sizeof(Point3D));
	indices.resize(triangleCount * 3);

	if (vertexCount <= USHRT_MAX)
	{
		unsigned short* indexRead = (unsigned short*)indexList;

		for (unsigned int i = 0; i < (triangleCount * 3); i++)
		{
			indices[i] = indexRead[i];
		}
	}
	else
	{
		
		memcpy(&indices[0], indexList, 3 * triangleCount * sizeof(unsigned int));
	}

	// reorder vertices based on x axis order
	ReorderVerticesOnAxis(vertices, indices);

	// then check for duplicate points ( needed for correct triangle neighbors search)

	std::vector<int>	virtualIndices;
	int virtualIndiceCount = CreateVirtualIndicesList(virtualIndices, vertices);

	// now construct half edge list ( vertex indices, triangle indice )  

	// then create list of segments
	// prealloc everything useful
	std::vector<SegmentCheck>	PreallocatedSegments;
	PreallocatedSegments.resize(indices.size());	// same segment count as point count in triangle list

	std::vector<ConnectedTriangle>	PreallocatedTriangles;
	PreallocatedTriangles.resize(triangleCount);

	std::vector<SegmentCheck*> linkedSegmentList; // list of segment sorted by min index
	linkedSegmentList.resize(virtualIndiceCount);
	memset(linkedSegmentList.data(), 0, virtualIndiceCount * sizeof(SegmentCheck*));

	// build half edge structure for following steps
	BuildHalfEdgeList(indices, vertices, virtualIndices, PreallocatedSegments, PreallocatedTriangles, linkedSegmentList);

	// iterative smooth normals and neighbors coherency
	IterativeTriangleConnection(indices, PreallocatedSegments, PreallocatedTriangles);

	// now group connected triangles with similar orientation
	std::vector<TriangleGrp>	triangleFlag;
	triangleFlag.resize(triangleCount);
	memset(&triangleFlag[0], 0, triangleCount * sizeof(TriangleGrp));

	std::vector< std::set<unsigned int> >	mergedGroups;
	FloodFillTriangleGroups(triangleCount, indices, PreallocatedSegments, PreallocatedTriangles, triangleFlag, mergedGroups);

	// manage merged groups
	std::vector < std::pair<int,int> > MergeGroupIndex;
	MergeGroupIndex.resize(mergedGroups.size());
	memset(&MergeGroupIndex[0], 0, mergedGroups.size() * sizeof(std::pair<int, int>));

	int finalGroupCount=MergeConnectedGroups(mergedGroups, MergeGroupIndex);

	for (auto &t : triangleFlag)
	{
		if (t.grpID > 0)
		{
			t.grpID = MergeGroupIndex[t.grpID].second;
			MergeGroupIndex[t.grpID].first++; // count triangle in each group
		}

	}
	std::vector<PlanarGroup >	planarGroupList;
	planarGroupList.resize(finalGroupCount);
	// create planar group list
	CreatePlanarGroups(indices, vertices, triangleFlag, MergeGroupIndex, PreallocatedSegments, PreallocatedTriangles, planarGroupList, onlyHorizontalAndVertical);

	// merge identical planar groups (even if unconnected)
	for (int i1=0;i1< planarGroupList.size();i1++)
	{
		auto& g1 = planarGroupList[i1];
		if (g1.triangles.size() == 0)
		{
			continue;
		}
		for (int i2 = i1 + 1; i2 < planarGroupList.size(); i2++)
		{
			auto& g2 = planarGroupList[i2];
			if (g2.triangles.size() == 0)
			{
				continue;
			}

			if (Dot(g1.normal, g2.normal) > 0.97)
			{

				// check if the two planes "merge well"
				v3f mido = (g2.origin + g1.origin)*0.5f;
				v3f medN = g1.normal + g2.normal;
				medN.Normalize();

				v3f oo=g1.origin-mido;
				float proj = Dot(oo, medN);
				v3f projG1 = g1.origin - proj * medN;
				float dist = Dist(projG1, mido);

				if ( fabsf(proj) < (0.05*dist)) // 5% error
				{
					MergeTwoPlanarGroups(triangleFlag, PreallocatedSegments, g1, g2, i1, i2);
					g2.triangles.clear();
				}
			}

		}
	}

	// search connection between planar groups
	mergedGroups.clear();
	mergedGroups.resize(finalGroupCount);

	for (auto& g : planarGroupList)
	{
		if (g.triangles.size() == 0)
		{
			continue;
		}
		int currentGrp = triangleFlag[g.triangles[0]].grpID;
		for (int ti : g.triangles)
		{
			for (int j = 0; j < 3; j++)
			{
				SegmentCheck& s = PreallocatedSegments[ti * 3 + j];
				if (s.Twin)
				{
					int othergrp = triangleFlag[s.Twin->triangleIndice].grpID;
					if (othergrp > currentGrp)
					{
						g.borderEdges.push_back({ ti,j });
						mergedGroups[currentGrp].insert(othergrp);
					}
					else if (othergrp < currentGrp)
					{
						g.borderEdges.push_back({ ti,j });
						mergedGroups[othergrp].insert(currentGrp);
					}
				}
				else
				{
					g.borderEdges.push_back({ti,j});
				}
			}
		}
	}

	// check groups with a common edge
	// TODO : for corner matching

	float surfaceThreshold = 0.25f;
	// now create each node from triangle group
	for (auto& g : planarGroupList)
	{
		if (g.triangles.size() == 0)
		{
			continue;
		}
		// only big enough surface 
		if (g.surface > surfaceThreshold)
		{
			if ((g.is_H_Or_V & onlyHorizontalAndVertical) || (!onlyHorizontalAndVertical))
			{
				m_NodeList.push_back(SpacialMeshBVHNode());
				SpacialMeshBVHNode& newOne = m_NodeList.back();
				newOne.m_Normal = g.normal;
				newOne.m_o= g.origin;
				newOne.m_surface = g.surface;

				// set uv depending on normal
				if (fabsf(newOne.m_Normal.z) > 0.75)
				{
					newOne.m_u.Set(1.0f, 0.0f, 0.0f);
					newOne.m_v.CrossProduct(newOne.m_Normal, newOne.m_u);
					newOne.m_v.Normalize();
					newOne.m_u.CrossProduct(newOne.m_v, newOne.m_Normal);
				}
				else
				{
					newOne.m_u.Set(0.0f, 0.0f, 1.0f);
					newOne.m_v.CrossProduct(newOne.m_Normal, newOne.m_u);
					newOne.m_v.Normalize();
					newOne.m_u.CrossProduct(newOne.m_v, newOne.m_Normal);
				}

				// get triangle list and find outline
				

				for (auto tri : g.sortStableTriangles)
				{
					auto ti = tri.second;
					newOne.add2DVertice(vertices[indices[3 * ti + 0]]);
					newOne.add2DVertice(vertices[indices[3 * ti + 1]]);
					newOne.add2DVertice(vertices[indices[3 * ti + 2]]);
				}

				for (auto ti : g.invalid_triangles)
				{
					newOne.add2DVertice(vertices[indices[3 * ti + 0]]);
					newOne.add2DVertice(vertices[indices[3 * ti + 1]]);
					newOne.add2DVertice(vertices[indices[3 * ti + 2]]);
				}

				int max_nb_triangles = std::min(((int)g.sortStableTriangles.size() / 32) + 1, 4);
				for (int i = 0; i < max_nb_triangles; ++i)
				{
					if (i >= g.sortStableTriangles.size()) break;
					newOne.m_StableTriangles.push_back(i);
				}
				
				/*
				std::unordered_map<int, int> lookup;
				for (auto ti : g.triangles)
				{
					int i = ti * 3;
					lookup[ti] = newOne.m_2D_Triangles.size();
					for (int j = 0; j < 3; j++)
					{
						// add 2d vertice
						newOne.add2DVertice(vertices[indices[i+j]]);
					}
					
				}
				// choose 4 "stable" triangles
				int i = 0;

				for (auto& t : g.sortStableTriangles)
				{
					//newOne.m_StableTriangles.push_back(lookup[t.second]);
					newOne.m_StableTriangles.push_back(t.second);
					i++;
					if (i >= 4)
					{
						break;
					}
				}
				*/

				newOne.computeBBox();
				m_3DBox.Update(newOne.m_3DBox);
			}
		}
	}
}

bool	SpacialMeshBVHNode::intersect(Hit& hit, const v3f& start, const v3f& dir) const
{
	float dot = Dot(dir, m_Normal);

	if (dot < -0.001) 
	{
		Point3D P(m_o - start);

		float s = Dot(P, m_Normal) / dot;
		if (s >= 0.0f)
		{
			hit.HitDistance=s;
			hit.HitNormal = m_Normal;
			hit.HitPosition = start + dir * s;

			v3f	toProject(hit.HitPosition);
			toProject -= m_o;

			// project 
			v2f projected;
			projected.x = Dot(toProject, m_u);
			projected.y = Dot(toProject, m_v);

			if (m_BBox.IsIn(projected))
			{
				for (int i = 0; i < m_2D_Triangles.size(); i += 3)
				{
					if (PointInTriangle(projected, m_2D_Triangles[i], m_2D_Triangles[i + 1], m_2D_Triangles[i + 2]))
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool SpacialMeshBVH::CallLocalRayIntersection(Hit &hit, const Point3D& start, const Vector3D& dir)  const
{
	// just test bbox for each group
	bool result = false;
	const SpacialMeshBVHNode* hit_node = nullptr;
	for (const auto& n : m_NodeList)
	{
		Hit tmphit;
		double distance = DBL_MAX;

		if (n.intersect(tmphit, start, dir))
		{
			if (tmphit.HitDistance < distance)
			{
				hit_node = &n;
				hit = tmphit;
				distance = tmphit.HitDistance;
				result = true;
			}
		}
	}

#ifdef KIGS_TOOLS
	if (hit_node) hit_node->m_Hit = true;
#endif
	return result;
}

bool SpacialMeshBVH::CallLocalRayIntersection(std::vector<Hit> &hits, const Point3D& start, const Vector3D& dir)  const
{

	for (const auto& n : m_NodeList)
	{
		Hit tmphit;

		if (n.intersect(tmphit, start, dir))
		{
#ifdef KIGS_TOOLS
			n.m_Hit = true;
#endif
			hits.push_back(tmphit);
		}
	}

	return hits.size();
}

#ifdef KIGS_TOOLS
#include <GLSLDebugDraw.h>
void SpacialMeshBVH::DrawDebug(const Point3D& pos, const  Matrix3x4* mat, Timer *timer)
{
	v3f RGB[2];
	srand(((unsigned int)this) / 16); // initialisation de rand
	RGB[0].Set((rand() % 1000) / 1000.0f, (rand() % 1000) / 1000.0f, (rand() % 1000) / 1000.0f);
	RGB[1].Set(1.0f - RGB[0].x, 1.0f - RGB[0].y, 1.0f - RGB[0].z);

	for(auto& newOne : m_NodeList)
	// debug draw 
	{
		if (!newOne.m_Hit) continue;
		newOne.m_Hit = false;

		std::vector<v3f>	Triangles3D;
		Triangles3D.resize(newOne.m_2D_Triangles.size());

		for (int i = 0; i < newOne.m_2D_Triangles.size(); i++)
		{
			Triangles3D[i] = newOne.m_o + newOne.m_u * newOne.m_2D_Triangles[i].x + newOne.m_v * newOne.m_2D_Triangles[i].y;
		}

		m_LtoGMatrix.TransformPoints(&Triangles3D[0], Triangles3D.size());
		for (int i = 0; i < Triangles3D.size(); i+=3)
		{
			dd::line(Triangles3D[i], Triangles3D[i+1], RGB[0]);
			dd::line(Triangles3D[i+1], Triangles3D[i + 2], RGB[0]);
			dd::line(Triangles3D[i+2], Triangles3D[i], RGB[0]);
		}

		// draw normal 
		{
			Vector3D bary(newOne.m_o);
			Vector3D normal(newOne.m_Normal);
			m_LtoGMatrix.TransformPoints(&bary, 1);
			m_LtoGMatrix.TransformVector(&normal);
			dd::arrow(bary, bary + normal * 0.1f, RGB[0], 0.02f);
		}
	}
}
#endif