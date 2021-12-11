#include "ModernMeshBuilder.h"
#include "ModuleRenderer.h"

#include "TecLibs/Math/Algorithm.h"
#include <algorithm>

template<typename T>
static T* GetVertexComp(void* v, unsigned int offset)
{
	return (T*)((unsigned char*)v + offset);
}

ModernMeshBuilder::ModernMeshBuilder() : mVertexArrayMask(0)
{
	// init default parameters
	mGroupCount = 0;
	mTriangleChunkSize = 1024;
	mTexCoordsScale = 1.0f;
}

ModernMeshBuilder::~ModernMeshBuilder()
{
	delete[] mOneVertexData;
}

void	ModernMeshBuilder::StartGroup(CoreVector* description, int hintVertexBufferBufferSize, int hintTriangleBufferSize)
{
	if (mGroupBuilding)
	{
		KIGS_ERROR("ModernMeshBuilder::StartGroup called but another group is not finished\n", 1);
		return;
	}
	mGroupBuilding = true;
	mBBoxInit = false;
	mTriangles.init(hintTriangleBufferSize, sizeof(ModernMesh::Triangle<unsigned int>));
	mVertexBuilder.clear();
	mVertexDesc.clear();
	mVertexMergeBarriers.clear();
	mIndicesMergeBarriers.clear();

	// parse descrition to get vertex size
	mCurrentVertexSize = 0;
	mCurrentVertexInSize = 0;

	unsigned int vertexElemPos = 0;
	unsigned int inpos = 0;

	CoreItemIterator	checkdescit = description->begin();
	CoreItemIterator	checkdescitend = description->end();
	
	while (checkdescit != checkdescitend)
	{
		CoreNamedItem& found = (CoreNamedItem&)(CoreItem&)(*checkdescit);

		if (found.getName() == "vertices")
		{
			ModernMesh::VertexElem	toAdd;
			toAdd.name = "vertices";
			toAdd.size = 4 * 3;
			toAdd.startpos = vertexElemPos;
			toAdd.mask = ModuleRenderer::VERTEX_ARRAY_MASK;
			toAdd.elemCount = 3;
			toAdd.inSize = 4 * 3;
			mCurrentVertexInSize += toAdd.inSize;
			toAdd.inStartPos = inpos;
			inpos += toAdd.inSize;
			toAdd.type = ModernMesh::VertexElem::Type::Position3D;
			mVertexDesc.push_back(toAdd);

			vertexElemPos += toAdd.size;
			mCurrentVertexSize += toAdd.size;
			mVertexArrayMask |= toAdd.mask;

		}
		else if (found.getName() == "colors")
		{
			ModernMesh::VertexElem	toAdd;
			toAdd.name = "colors";
			toAdd.size = 4; // 4 u8 [0:255] out
			toAdd.startpos = vertexElemPos;
			toAdd.mask = ModuleRenderer::COLOR_ARRAY_MASK;
			toAdd.elemCount = 4;
			toAdd.inSize = 4 * 4; // 4 float [0:1] in
			mCurrentVertexInSize += toAdd.inSize;
			toAdd.inStartPos = inpos;
			inpos += toAdd.inSize;
			toAdd.type = ModernMesh::VertexElem::Type::ColorRGBA;
			mVertexDesc.push_back(toAdd);
			mCurrentVertexSize += toAdd.size;
			vertexElemPos += toAdd.size;
			mVertexArrayMask |= toAdd.mask;
		}
		else if (found.getName() == "texCoords")
		{
			ModernMesh::VertexElem	toAdd;
			toAdd.name = "texCoords";
			toAdd.size = 4 * 2;
			toAdd.startpos = vertexElemPos;
			toAdd.mask = ModuleRenderer::TEXCOORD_ARRAY_MASK;
			toAdd.elemCount = 2;
			toAdd.inSize = 4 * 2;
			mCurrentVertexInSize += toAdd.inSize;
			toAdd.inStartPos = inpos;
			inpos += toAdd.inSize;
			toAdd.type = ModernMesh::VertexElem::Type::TextureCoordinate2D;
			mVertexDesc.push_back(toAdd);

			mCurrentVertexSize += toAdd.size;
			vertexElemPos += toAdd.size;
			mVertexArrayMask |= toAdd.mask;
		}
		else if (found.getName() == "tangents" || found.getName() == "generate_tangents")
		{
			ModernMesh::VertexElem	toAdd;
			toAdd.name = "tangents";
			toAdd.size = 4;
			toAdd.startpos = vertexElemPos;
			toAdd.mask = ModuleRenderer::TANGENT_ARRAY_MASK;
			toAdd.elemCount = 3;
			toAdd.inSize = found.getName() == "tangents" ? 4 * 3 : 0;
			if (toAdd.inSize == 0)
				mGenerateTangents = true;
			mCurrentVertexInSize += toAdd.inSize;
			toAdd.inStartPos = inpos;
			inpos += toAdd.inSize;
			toAdd.type = ModernMesh::VertexElem::Type::Tangent3D;
			mVertexDesc.push_back(toAdd);

			mCurrentVertexSize += toAdd.size;
			vertexElemPos += toAdd.size;
			mVertexArrayMask |= toAdd.mask;
		}
		else if (found.getName() == "normals" || found.getName() == "generate_normals")
		{
			ModernMesh::VertexElem	toAdd;
			toAdd.name = "normals";
			toAdd.size = 4;
			toAdd.startpos = vertexElemPos;
			toAdd.mask = ModuleRenderer::NORMAL_ARRAY_MASK;
			toAdd.elemCount = 3;
			toAdd.inSize = found.getName() == "normals" ? 4 * 3 : 0; 
			if (toAdd.inSize == 0)
				mGenerateNormals = true;
			mCurrentVertexInSize += toAdd.inSize;
			toAdd.inStartPos = inpos;
			inpos += toAdd.inSize;
			toAdd.type = ModernMesh::VertexElem::Type::Normal3D;
			mVertexDesc.push_back(toAdd);

			mCurrentVertexSize += toAdd.size;
			vertexElemPos += toAdd.size;
			mVertexArrayMask |= toAdd.mask;
		}
		else if (found.getName() == "bone_weights")
		{
			ModernMesh::VertexElem	toAdd;
			toAdd.name = "bone_weights";
			toAdd.size = 4;
			toAdd.startpos = vertexElemPos;
			toAdd.mask = ModuleRenderer::BONE_WEIGHT_ARRAY_MASK;
			toAdd.elemCount = 4;
			toAdd.inSize = 4;
			mCurrentVertexInSize += toAdd.inSize;
			toAdd.inStartPos = inpos;
			inpos += toAdd.inSize;
			toAdd.type = ModernMesh::VertexElem::Type::BoneWeights;
			mVertexDesc.push_back(toAdd);

			mCurrentVertexSize += toAdd.size;
			vertexElemPos += toAdd.size;
			mVertexArrayMask |= toAdd.mask;
		}
		else if (found.getName() == "bone_indexes")
		{
			ModernMesh::VertexElem	toAdd;
			toAdd.name = "bone_indexes";
			toAdd.size = 4;
			toAdd.startpos = vertexElemPos;
			toAdd.mask = ModuleRenderer::BONE_INDEX_ARRAY_MASK;
			toAdd.elemCount = 4;
			toAdd.inSize = 4;
			mCurrentVertexInSize += toAdd.inSize;
			toAdd.inStartPos = inpos;
			inpos += toAdd.inSize;
			toAdd.type = ModernMesh::VertexElem::Type::BoneIndexes;
			mVertexDesc.push_back(toAdd);

			mCurrentVertexSize += toAdd.size;
			vertexElemPos += toAdd.size;
			mVertexArrayMask |= toAdd.mask;
		}
		else if (found.getName() == "TexCoordsScale")
		{
			found.getValue(mTexCoordsScale);
		}
		else if (found.getName() == "smooth_normals_angle")
		{
			found.getValue(mSmoothNormalsThreshold);
			mSmoothNormalsThreshold = cos(mSmoothNormalsThreshold);
		}
		else if (found.getName() == "no_merge")
		{
			mNoMerge = true;
		}

		++checkdescit;
	}

	mCurrentVertexBuilderSize = 0;
	delete[] mOneVertexData;
	mOneVertexData = new unsigned char[mCurrentVertexSize];
	mVertexArray.init(hintVertexBufferBufferSize, mCurrentVertexSize);
}

void	ModernMeshBuilder::OptimiseForCache()
{
	unsigned int vc = mVertexArray.size();
	OptimiseBuildVertexStruct*	vl = new OptimiseBuildVertexStruct[vc];

	unsigned int i;
	// init values for array of vertices
	for (i = 0; i < vc; i++)
	{
		vl[i].useCount = 0;
		vl[i].newIndex = -1;
	}

	unsigned int vt = mTriangles.size(); 

	ModernMesh::Triangle<unsigned int>*	triangleArray = reinterpret_cast< ModernMesh::Triangle<unsigned int>*>(mTriangles.getArray());

	unsigned int*	sortedTriangleIndex = new unsigned int[vt];

	std::vector<SortTriangle>	sortedTriangleList;

	for (i = 0; i < vt; i++)
	{
		// increment use count for each vertice used by current triangle
		vl[triangleArray[i].indices[0]].useCount++;
		vl[triangleArray[i].indices[1]].useCount++;
		vl[triangleArray[i].indices[2]].useCount++;

		// for each triangle, score is the minimum value for each vertice index
		SortTriangle toAdd;
		toAdd.Index = i;
		toAdd.Score = MIN(MIN(triangleArray[i].indices[0], triangleArray[i].indices[1]), triangleArray[i].indices[2]);

		sortedTriangleList.push_back(toAdd);

	}

	// rearrange triangles using score (min vertice index)
	// create an array of sorted triangle indexes
	std::sort(sortedTriangleList.begin(), sortedTriangleList.end());
	auto itSortTriangle = sortedTriangleList.begin();
	for (i = 0; i < vt; i++)
	{
		sortedTriangleIndex[i] = (*itSortTriangle).Index;
		++itSortTriangle;
	}

	int currentVertexIndex = 0;

	AbstractDynamicGrowingBuffer newmyTriangles(256, sizeof(ModernMesh::Triangle<unsigned int>));
	AbstractDynamicGrowingBuffer newmyVertexArray(256, mCurrentVertexSize);

	std::vector<OptimiseTriangle>	optimisedTriangleList;

	int remainingTriangleCount = (int)vt;
	int currentChunkStart = 0;
	// optimise triangle chunk one by one (as brute force is used)
	while (remainingTriangleCount)
	{
		int triangleCountInChunk = mTriangleChunkSize;

		if (remainingTriangleCount < mTriangleChunkSize)
		{
			triangleCountInChunk = remainingTriangleCount;
		}
		optimisedTriangleList.clear();
		optimisedTriangleList.resize(triangleCountInChunk);

		// create triangle list for this chunk
		for (i = 0; i < triangleCountInChunk; i++)
		{
			optimisedTriangleList[i].triangleStruct = &triangleArray[sortedTriangleIndex[i + currentChunkStart]];
			optimisedTriangleList[i].Score1 = 0;
		}

		currentChunkStart += triangleCountInChunk;
		remainingTriangleCount -= triangleCountInChunk;

		auto	optBegin = optimisedTriangleList.begin();
		auto	optEnd = optimisedTriangleList.end();

		while (optBegin != optEnd)
		{
			// for each triangle, compute score
			(*optBegin).ComputeScore(vl, 0);
			++optBegin;
		}

		while (optimisedTriangleList.size())
		{
			// sort triangles using score
			std::sort(optimisedTriangleList.begin(), optimisedTriangleList.end());

			// pick best one
			OptimiseTriangle& bestOne = optimisedTriangleList.back();


			ModernMesh::Triangle<unsigned int>	toAdd;
			for (int vertice_in_triangle = 0; vertice_in_triangle < 3; vertice_in_triangle++)
			{
				int indice= bestOne.triangleStruct->indices[vertice_in_triangle];
				// decrease use count for this triangle vertice
				vl[indice].useCount--;
				// check if freshly added
				if (vl[indice].newIndex == -1)
				{
					vl[indice].newIndex = currentVertexIndex++;
					newmyVertexArray.push_back(mVertexArray[indice]);
				}
				toAdd.indices[vertice_in_triangle] = vl[indice].newIndex;
			}
			
			// add new triangle to list
			newmyTriangles.push_back(&toAdd);

			// remove best found triangle from chunk
			optimisedTriangleList.pop_back();
			
			// recompute score for all the remaining triangles in chunk
			// brute force
			optBegin = optimisedTriangleList.begin();
			optEnd = optimisedTriangleList.end();

			while (optBegin != optEnd)
			{
				(*optBegin).ComputeScore(vl, currentVertexIndex);
				++optBegin;
			}

		}
	}

	mVertexArray = std::move(newmyVertexArray);
	mTriangles = std::move(newmyTriangles);

	delete[] sortedTriangleIndex;
	delete[] triangleArray;
	delete[] vl;
}


// compute triangle score based on several clues :
// - vertices are already in use
// - minimal use count is better
// - maximal - minimal use count is better (try to decrease count of highly used vertices)
// - added recently in buffer is better
void	ModernMeshBuilder::OptimiseTriangle::ComputeScore(OptimiseBuildVertexStruct* varray, int currentVertexIndex)
{

	int countCached = 0;
	unsigned int minUse = 10000;
	unsigned int maxUse = 0;
	int recentlyAdded = 0;

	int i;
	for (i = 0; i < 3; i++)
	{
		if (varray[triangleStruct->indices[i]].newIndex != -1)
		{
			countCached++;
		}
		if (varray[triangleStruct->indices[i]].useCount < minUse)
		{
			minUse = varray[triangleStruct->indices[i]].useCount;
		}
		if (varray[triangleStruct->indices[i]].useCount > maxUse)
		{
			maxUse = varray[triangleStruct->indices[i]].useCount;
		}
		if (varray[triangleStruct->indices[i]].newIndex != -1)
		{
			int recent = 80 - currentVertexIndex - varray[triangleStruct->indices[i]].newIndex;
			if (recent > 0)
			{
				recentlyAdded += recent;
			}
		}
	}

	if (minUse > 255)
	{
		minUse = 255;
	}
	if (maxUse > 255)
	{
		maxUse = 255;
	}

	Score1 = (countCached << 24) | ((255 - minUse) << 16) | ((maxUse - minUse) << 8) | (recentlyAdded);
}

SP<ModernMeshItemGroup> ModernMeshBuilder::EndGroup(int vertex_count, v3f* vertices, v3f* normals, v4f* colors, v2f* texCoords, int face_count, v3u* faces, v3f offset, SP<ModernMeshItemGroup> reuse_group)
{
	char tmp[32];
	sprintf(tmp, "group%d", mGroupCount++);
	SP<ModernMeshItemGroup> result(nullptr);
	if (!vertex_count) return result;

	result = reuse_group;
	unsigned char * interleaved_vertex_buffer = new unsigned char[vertex_count*mCurrentVertexSize];
	if (!result)
		result = KigsCore::GetInstanceOf(tmp, "ModernMeshItemGroup");
	else
	{
		result->~ModernMeshItemGroup();
		new (result.get()) ModernMeshItemGroup(tmp);
		/*result->mBoundaries.clear();
		result->mColliderBoundaries.clear();
		result->mCanFreeBuffers = 0;
		result->mBoundariesMask = UINT64_MAX;*/
	}

	result->mVertexCount = vertex_count;
	result->mVertexSize = mCurrentVertexSize;
	result->mVertexArrayMask = mVertexArrayMask;
	result->mVertexDesc = mVertexDesc;
	
	result->setValue("TexCoordsScale", mTexCoordsScale);

	int vertex_offset, texcoord_offset, normal_offset, color_offset;
	auto itr = mVertexDesc.begin();
	for (; itr != mVertexDesc.end(); ++itr)
	{
		if (itr->name == "vertices")
			vertex_offset = itr->startpos;
		else if (itr->name == "texCoords")
			texcoord_offset = itr->startpos;
		else if (itr->name == "normals")
			normal_offset = itr->startpos;
		else if (itr->name == "colors")
			color_offset = itr->startpos;
	}
	std::vector<v3f> nCalcArray;
	if (mVertexArrayMask & ModuleRenderer::NORMAL_ARRAY_MASK)
	{
		if (!normals)
		{
			nCalcArray.resize(vertex_count);
			normals = nCalcArray.data();
			auto nArray = nCalcArray.data();
			v3f* vertexReader = (v3f*)vertices;
				
			memset(nArray, 0, sizeof(v3f)*vertex_count);
			Vector3D N;

			auto calcNormal = [&](u32 a, u32 b, u32 c)
			{
				v3f v1 = vertexReader[a];
				v3f v2 = vertexReader[b];
				v3f v3 = vertexReader[c];

				N.CrossProduct(v2 - v1, v3 - v1);
				N.Normalize();

				nArray[a] += N;
				nArray[b] += N;
				nArray[c] += N;
			};

			v3u* tri = faces;
			for (int i = 0; i < face_count; i++)
			{
				calcNormal(tri->x, tri->y, tri->z);
				tri++;
			}
		}
	}


	auto write_ptr = interleaved_vertex_buffer;
	kfloat texcoordscale = mTexCoordsScale;
	for (int index = 0; index < vertex_count; index++)
	{
		v3f pos;
		memcpy(&pos.x, vertices + index, 3 * sizeof(float));
		pos += offset;
		memcpy(write_ptr + vertex_offset, &pos.x, 3 * sizeof(float));


		if (mVertexArrayMask & ModuleRenderer::TEXCOORD_ARRAY_MASK)
		{
			v2f* tcWriter = (v2f*)(write_ptr + texcoord_offset);
			if (texCoords)
			{
				*tcWriter = texCoords[index];
			}
			else
			{
				tcWriter->x = (vertices[index].x+offset.x) * texcoordscale;
				tcWriter->y = (vertices[index].y+offset.y) * texcoordscale;
			}
		}
		if (mVertexArrayMask & ModuleRenderer::NORMAL_ARRAY_MASK)
		{
			auto n = (normals + index)->Normalized();
			n.x = clamp(n.x, -1.0f, 1.0f);
			n.y = clamp(n.y, -1.0f, 1.0f);
			n.z = clamp(n.z, -1.0f, 1.0f);
			n = (n*255.0f) / 2.0f;
			*(write_ptr + normal_offset + 0) = (signed char)n.x;
			*(write_ptr + normal_offset + 1) = (signed char)n.y;
			*(write_ptr + normal_offset + 2) = (signed char)n.z;
		}

			
		if (colors && mVertexArrayMask & ModuleRenderer::COLOR_ARRAY_MASK)
		{
			*(write_ptr + color_offset + 0) = colors[index].r * 255;
			*(write_ptr + color_offset + 1) = colors[index].g * 255;
			*(write_ptr + color_offset + 2) = colors[index].b * 255;
			*(write_ptr + color_offset + 3) = colors[index].a * 255;
		}
		write_ptr += mCurrentVertexSize;
	}

	// set vertex array
	result->mVertexBufferArray.SetBuffer(interleaved_vertex_buffer, vertex_count*mCurrentVertexSize);

	if (vertex_count < 65536)
	{
		// use unsigned short indices
		u16* indices = new u16[face_count * 3];
		for (int i = 0; i < face_count; ++i)
		{
			indices[3 * i + 0] = faces[i].x;
			indices[3 * i + 1] = faces[i].y;
			indices[3 * i + 2] = faces[i].z;
		}
		result->mTriangleBuffer.SetBuffer(indices, face_count * 3 * sizeof(u16));
	}
	else
	{
		// unsigned int indices 
		u32* indices = new u32[face_count * 3];
		for (int i = 0; i < face_count; ++i)
		{
			indices[3 * i + 0] = faces[i].x;
			indices[3 * i + 1] = faces[i].y;
			indices[3 * i + 2] = faces[i].z;
		}
		result->mTriangleBuffer.SetBuffer(indices, face_count * 3 * sizeof(u32));
	}
	result->mTriangleCount = face_count;
	result->Init();
	mGroupBuilding = false;
	return result;
}

SP<ModernMeshItemGroup>	ModernMeshBuilder::EndGroup(void * vertex, int vertexCount, void * index, int indexCount)
{
	KIGS_WARNING("Deprecated EngGroup function", 3);
	Vector3D*	nArray = nullptr;

	char tmp[32];
	sprintf(tmp, "group%d", mGroupCount++);

	int tricount = indexCount / 3;

	SP<ModernMeshItemGroup>	result(nullptr);
	if (vertexCount)
	{
		//compute normal if needed
		if (mVertexArrayMask & ModuleRenderer::NORMAL_ARRAY_MASK)
		{
			Vector3D* vertexReader = (Vector3D*)vertex;

			nArray = new Vector3D[vertexCount];
			memset(nArray, 0, sizeof(Vector3D)*vertexCount);
			Vector3D N;

			auto calcNormal = [&](int a, int b, int c)
			{
				Vector3D v1 = vertexReader[a];
				Vector3D v2 = vertexReader[b];
				Vector3D v3 = vertexReader[c];

				N.CrossProduct(v2 - v1, v3 - v1);
				N.Normalize();

				nArray[a] += N;
				nArray[b] += N;
				nArray[c] += N;
			};

			if (vertexCount < 65536)
			{
				unsigned short* trireader = (unsigned short*)index;
				// we have to use triangles to compute tangents, each vertice apart is not useful
				for (int i = 0; i < tricount; i++)
				{
					calcNormal(trireader[0], trireader[1], trireader[2]);
					trireader += 3;
				}
			}
			else
			{
				unsigned int* trireader = (unsigned int*)index;
				// we have to use triangles to compute tangents, each vertice apart is not useful
				for (int i = 0; i < tricount; i++)
				{
					calcNormal(trireader[0], trireader[1], trireader[2]);
					trireader += 3;
				}
			}
		}

		// create interleaved vertex array
		unsigned char * buff = new unsigned char[vertexCount*mCurrentVertexSize];
		memset(buff, 0, vertexCount*mCurrentVertexSize);
		unsigned char * writer = buff;
		float * reader = (float*)vertex;

		result = KigsCore::GetInstanceOf(tmp, "ModernMeshItemGroup");
		result->mVertexCount = vertexCount;
		result->mVertexSize = mCurrentVertexSize;
		result->mVertexArrayMask = mVertexArrayMask;
		result->mVertexDesc = mVertexDesc;
		result->setValue("TexCoordsScale", mTexCoordsScale);

		int vertexPos, texcoordPos, NormalPos;
		auto itr = mVertexDesc.begin();
		for (; itr != mVertexDesc.end(); ++itr)
		{
			if (itr->name == "vertices")
				vertexPos = itr->startpos;
			else if (itr->name == "texCoords")
				texcoordPos = itr->startpos;
			else if (itr->name == "normals")
				NormalPos = itr->startpos;
		}
		kfloat texcoordscale = mTexCoordsScale;
		for (int index = 0; index < vertexCount; index++)
		{
			// write vertex pos (3 float)
			memcpy(writer + vertexPos, reader, 3 * sizeof(float));
			// write texcoord (2 float) (use pos x and pos y)
			if (mVertexArrayMask & ModuleRenderer::TEXCOORD_ARRAY_MASK)
			{
				float* tcWriter = (float*)(writer + texcoordPos);
				tcWriter[0] = reader[0] * texcoordscale;
				tcWriter[1] = reader[1] * texcoordscale;
			}
			
			// write normal (3 unsigned char + 1 char unused)
			if (mVertexArrayMask & ModuleRenderer::NORMAL_ARRAY_MASK)
			{
				nArray[index].Normalize();
				writer[NormalPos] = (signed char)(nArray[index].x*127.5f);
				writer[NormalPos+1] = (signed char)(nArray[index].y*127.5f);
				writer[NormalPos+2] = (signed char)(nArray[index].z*127.5f);
			}
			writer += mCurrentVertexSize;
			reader += 3;
		}

		// set vertex array
		result->mVertexBufferArray.SetBuffer(buff, vertexCount*mCurrentVertexSize);

		// use unsigned short indices
		if (vertexCount < 65536)
			result->mTriangleBuffer.SetBuffer(index, indexCount * sizeof(unsigned short));
		else // unsigned int indices 
			result->mTriangleBuffer.SetBuffer(index, indexCount * sizeof(unsigned int));
		result->mTriangleCount = tricount;

		result->Init();
	}

	if (nArray)
		delete[] nArray;

	mTriangles.Clear();
	mVertexArray.Clear();
	mGroupBuilding = false;
	return result;
}

SP<ModernMeshItemGroup>	ModernMeshBuilder::EndGroup(bool optimize)
{
	char tmp[32];
	sprintf(tmp, "group%d", mGroupCount++);

	SP<ModernMeshItemGroup>	result(nullptr);
	if (mCurrentVertexBuilderSize)
	{
		if (mGenerateNormals)
			GenerateNormals();
		
		if (mGenerateTangents)
			GenerateTangents();

		SnapToGridAndMerge();
		
		if (optimize)
			OptimiseForCache();

		result = KigsCore::GetInstanceOf(tmp, "ModernMeshItemGroup");
		result->mVertexBufferArray.SetBuffer(mVertexArray.getArray(), mVertexArray.size()*mCurrentVertexSize);
		result->mVertexCount = mVertexArray.size();
		result->mVertexSize = mCurrentVertexSize;
		result->mVertexArrayMask = mVertexArrayMask;
		result->mVertexDesc = mVertexDesc;
		result->setValue("TexCoordsScale", mTexCoordsScale);

		auto s1 = result->mVertexBufferArray.const_ref()->size();
		auto s2 = result->mVertexCount * result->mVertexSize;
		KIGS_ASSERT(s1 == s2);

		if (mIndicesMergeBarriers.size())
		{
			result->SetIndexBoundaries(mIndicesMergeBarriers);
		}

		// use unsigned short indices
		if (result->mVertexCount < 65536)
		{
			ModernMesh::Triangle<unsigned int>*   	triangleArray = reinterpret_cast< ModernMesh::Triangle<unsigned int>*>(mTriangles.getArray());
			ModernMesh::Triangle<unsigned short>*	shorttriangleArray = new ModernMesh::Triangle<unsigned short>[mTriangles.size()];

			int copyTriangle;
			int tricount = mTriangles.size();
			for (copyTriangle = 0; copyTriangle < tricount; copyTriangle++)
			{
				shorttriangleArray[copyTriangle].indices[0] = (unsigned short)triangleArray[copyTriangle].indices[0];
				shorttriangleArray[copyTriangle].indices[1] = (unsigned short)triangleArray[copyTriangle].indices[1];
				shorttriangleArray[copyTriangle].indices[2] = (unsigned short)triangleArray[copyTriangle].indices[2];
			}

			result->mTriangleBuffer.SetBuffer(shorttriangleArray, mTriangles.size() * sizeof(ModernMesh::Triangle<unsigned short>));
			delete[] triangleArray;
		}
		else // unsigned int indices 
		{
			result->mTriangleBuffer.SetBuffer(reinterpret_cast< ModernMesh::Triangle<unsigned int>*>(mTriangles.getArray()), mTriangles.size() * sizeof(ModernMesh::Triangle<unsigned int>));
		}
		result->mTriangleCount = mTriangles.size();

		result->Init();
	}

	mTriangles.Clear();
	mVertexArray.Clear();
	mGroupBuilding = false;
	return result;
}

bool			ModernMeshBuilder::areEqual(void* v1, void* v2)
{
	return (memcmp(v1, v2, mCurrentVertexSize) == 0);
}

// convert from full float format in mEntry to compressed output
void* ModernMeshBuilder::convert(void* v)
{
	unsigned char* result = mOneVertexData;
	unsigned char*	read = (unsigned char*)v;

	for(auto& current : mVertexDesc)
	{
		if (current.type == ModernMesh::VertexElem::Type::Position3D)
		{
			v3f pos = *GetVertexComp<v3f>(read, current.inStartPos);
			if (!mBBoxInit)
			{
				mCurrentBBox.Init(pos);
				
				mBBoxInit = true;
			}
			else
			{
				mCurrentBBox.Update(pos);
			}
				
			if (mSectionInit)
			{
				mCurrentSectionBBox.Init(pos);
				mSectionInit = false;
			}
			else
			{
				mCurrentSectionBBox.Update(pos);
			}


		}

		if (current.inSize == 0) // not provided
		{
			memset(result + current.startpos, 0, current.size);
		}
		else if (current.size == current.inSize) // copy
		{
			memcpy(result + current.startpos, read + current.inStartPos, current.size);
		}
		else // convert
		{
			memset(result + current.startpos, 0, current.size);

			unsigned int i;
			for (i = 0; i < current.elemCount; i++)
			{
				float flin = *((float*)(read + current.inStartPos + 4 * i));
				if (current.type == ModernMesh::VertexElem::Type::ColorRGBA)
				{
					if (flin < 0.0f)
					{
						flin = 0.0f;
					}
					if (flin > 1.0f)
					{
						flin = 1.0f;
					}
					unsigned char* component = (result + current.startpos + i);
					*component = (unsigned char)(flin*255.0f);
				}
				else if(current.type == ModernMesh::VertexElem::Type::Normal3D || current.type == ModernMesh::VertexElem::Type::Tangent3D)
				{
					if (flin < -1.0f)
					{
						flin = -1.0f;
					}
					if (flin > 1.0f)
					{
						flin = 1.0f;
					}
					signed char* component = ((signed  char*)result + current.startpos + i);
					*component = (signed  char)((flin*255.0f) / 2.0f);
				}
			}
		}
	}
	return result;
}


unsigned int	ModernMeshBuilder::addVertex(void* v1)
{
	void* v = convert(v1);
	mVertexArray.push_back(v);
	return mCurrentVertexBuilderSize++;
}

void	ModernMeshBuilder::AddTriangle(void* v1, void* v2, void* v3)
{
	ModernMesh::Triangle<unsigned int>	toAdd;

	toAdd.indices[0] = addVertex(v1);
	toAdd.indices[1] = addVertex(v2);
	toAdd.indices[2] = addVertex(v3);

	mTriangles.push_back(&toAdd);
}

unsigned int	ModernMeshBuilder::getID(void* v)
{
	return fastGetID((const char*)v, mCurrentVertexSize);
}

void ModernMeshBuilder::PlaceMergeBarrier()
{
	mVertexMergeBarriers.push_back(mVertexArray.size());
	mIndicesMergeBarriers.push_back(mTriangles.size());
	mSectionsBBox.push_back(mCurrentSectionBBox);
	mSectionInit = true;
}

void ModernMeshBuilder::SnapToGridAndMerge()
{
	int startvertexpos = -1;
	int starttexcoordpos = -1;
	int startnormalpos = -1;
	for (auto& desc : mVertexDesc)
	{
		if (desc.type == ModernMesh::VertexElem::Type::Position3D)
			startvertexpos = desc.startpos;
		else if (desc.type == ModernMesh::VertexElem::Type::TextureCoordinate2D)
			starttexcoordpos = desc.startpos;
		else if (desc.type == ModernMesh::VertexElem::Type::Normal3D)
			startnormalpos = desc.startpos;
	}

	if (mVertexMergeBarriers.size() == 0 || mVertexMergeBarriers.back() != mVertexArray.size())
	{
		PlaceMergeBarrier();
	}

	struct GridSize
	{
		float pos;
		float uv;
	};
	std::vector<GridSize> sections_grid_sizes;


	GridSize old_gs;
	old_gs.pos = pow(10, (int)(log10(Norm(mCurrentBBox.m_Min - mCurrentBBox.m_Max) / 10000) - 0.5f));

	for (auto bbox : mSectionsBBox)
	{
		GridSize gs;
		gs.pos = pow(10, (int)(log10(Norm(bbox.m_Min - bbox.m_Max) / 10000) - 0.5f));
		gs.uv = 0.001f;
		sections_grid_sizes.push_back(gs);
	}

	int count_smoothed = 0;
	if (mSmoothNormalsThreshold != 1 && startnormalpos != -1)
	{
		const float threshold = mSmoothNormalsThreshold;
		std::vector<unsigned char> flags(mVertexArray.size(), 0);
		struct SharedVertex
		{
			unsigned int index;
			v3f normal;
			bool merge = false;
			unsigned int merge_mask;
		};
		std::vector<SharedVertex> shared;

		
		unsigned int currentBarrierIndex = 0;
		int currentBarrier = mVertexMergeBarriers[0];
		
		GridSize current_grid_size = sections_grid_sizes[currentBarrierIndex];

		for (unsigned int i = 0; i < mVertexArray.size(); ++i)
		{
			if ((flags[i] & 1) != 0) continue;

			if (i >= currentBarrier)
			{
				++currentBarrierIndex;
				current_grid_size = sections_grid_sizes[currentBarrierIndex];
				currentBarrier = mVertexMergeBarriers[currentBarrierIndex];
			}

			shared.clear();
			v3f pi = *GetVertexComp<v3f>(mVertexArray[i], startvertexpos);
			auto cni = GetVertexComp<signed char>(mVertexArray[i], startnormalpos);
			v3f ni{ *cni / 127.5f, *(cni + 1) / 127.5f, *(cni + 2) / 127.5f };
			shared.push_back({ i, ni, false, 1 });

			v3f average_n = ni;

			for (unsigned int j = i+1; j < currentBarrier; ++j)
			{
				if ((flags[j] & 1) == 0)
				{
					v3f pj = *GetVertexComp<v3f>(mVertexArray[j], startvertexpos);
					//if (NormSquare(pi - pj) <= current_grid_size.pos)
					if (NormSquare(pi - pj) <= FLT_EPSILON)
					{
						auto cnj = GetVertexComp<signed char>(mVertexArray[j], startnormalpos);
						v3f nj{ *cnj / 127.5f, *(cnj + 1) / 127.5f, *(cnj + 2) / 127.5f };

						shared.push_back({ j, nj, false, (1u<<shared.size()) });
						average_n += nj;
					}
				}
			}

			average_n.Normalize();

			for (int s1 = 0; s1 < shared.size(); ++s1)
			{
				for (int s2 = s1+1; s2 < shared.size(); ++s2)
				{
					if (Dot(shared[s1].normal, shared[s2].normal) > threshold)
					{
						shared[s1].merge_mask |= (1 << s2);
						shared[s2].merge_mask |= (1 << s1);
					}	
				}
			}
			
			for (int s1 = 0; s1 < shared.size(); ++s1)
			{
				if (!shared[s1].merge)
				{
					v3f true_average = shared[s1].normal;
					unsigned int mask = shared[s1].merge_mask;
					int count = 1;
					for (int s2 = s1+1; s2 < shared.size(); ++s2)
					{
						if (mask == shared[s2].merge_mask)
						{
							true_average += shared[s2].normal;
							count++;
						}
					}
					if (count >= 2)
					{
						true_average.Normalize();
						for (int s2 = s1; s2 < shared.size(); ++s2)
						{
							if (mask == shared[s2].merge_mask)
							{
								flags[shared[s2].index] |= 1;
								auto v = mVertexArray[shared[s2].index];
								auto cn = GetVertexComp<signed char>(v, startnormalpos);
								*(cn + 0) = true_average.x*127.5f;
								*(cn + 1) = true_average.y*127.5f;
								*(cn + 2) = true_average.z*127.5f;
								shared[s2].merge = true;
								count_smoothed++;
							}
						}
					}
				}
			}
		}
	}

	int count_merged = 0;
	if (!mNoMerge)
	{
		unsigned int currentBarrierIndex = 0;
		int currentBarrier = mVertexMergeBarriers[0];
		int lastBarrier = 0;

		GridSize current_grid_size = sections_grid_sizes[currentBarrierIndex];

		
		std::vector<int> indices_offsets;

		int wi = 0;

		int current_section_offset = 0;
		int next_section_offset = 0;

		for (int i = 0; i < mVertexArray.size(); ++i, ++wi)
		{
			if (i >= currentBarrier)
			{
				lastBarrier = wi;
				++currentBarrierIndex;
				current_grid_size = sections_grid_sizes[currentBarrierIndex];

				indices_offsets.push_back(current_section_offset);

				current_section_offset += next_section_offset;
				next_section_offset = 0;
				currentBarrier = mVertexMergeBarriers[currentBarrierIndex];
			}
			auto v = mVertexArray[i];
			v3f* p = GetVertexComp<v3f>(v, startvertexpos);
			*p /= current_grid_size.pos;
			p->x = (int)(p->x + 0.5f) * current_grid_size.pos;
			p->y = (int)(p->y + 0.5f) * current_grid_size.pos;
			p->z = (int)(p->z + 0.5f) * current_grid_size.pos;

			if (starttexcoordpos != -1)
			{
				v2f* uv = GetVertexComp<v2f>(v, starttexcoordpos);
				*uv /= current_grid_size.uv;
				uv->x = (int)(uv->x + 0.5f) * current_grid_size.uv;
				uv->y = (int)(uv->y + 0.5f) * current_grid_size.uv;
			}

			if(i!=wi)
				memcpy(mVertexArray[wi], v, mCurrentVertexSize);

			v = mVertexArray[wi];
			
			int indices_start = currentBarrierIndex > 0 ? mIndicesMergeBarriers[currentBarrierIndex - 1] : 0;
			int indices_end = mIndicesMergeBarriers[currentBarrierIndex];

			for (int j = wi - 1; j >= lastBarrier; --j)
			{
				if (memcmp(v, mVertexArray[j], mCurrentVertexSize) == 0)
				{
					count_merged++;
					
					
					for (int k = indices_start; k < indices_end; ++k)
					{
						auto t = (ModernMesh::Triangle<unsigned int>*)mTriangles[k];
						if (t->indices[0] == wi + current_section_offset) t->indices[0] = j + current_section_offset;
						if (t->indices[1] == wi + current_section_offset) t->indices[1] = j + current_section_offset;
						if (t->indices[2] == wi + current_section_offset) t->indices[2] = j + current_section_offset;

						if (t->indices[0] > wi + current_section_offset) t->indices[0]--;
						if (t->indices[1] > wi + current_section_offset) t->indices[1]--;
						if (t->indices[2] > wi + current_section_offset) t->indices[2]--;
					}
					next_section_offset++;
					wi--;
					break;
				}
			}
		}

		if (indices_offsets.size())
		{
			indices_offsets.push_back(current_section_offset);
			int n = 0;
			int k = 0;
			for (; n < mIndicesMergeBarriers.size(); ++n)
			{
				int next_section = mIndicesMergeBarriers[n];
				int section_offset = indices_offsets[n];
				//mIndicesMergeBarriers[n] -= section_offset/3;
				if (section_offset == 0)
				{
					k = next_section;
				}
				else
				{
					for (; k < next_section; ++k)
					{
						auto t = (ModernMesh::Triangle<unsigned int>*)mTriangles[k];
						t->indices[0] -= section_offset;
						t->indices[1] -= section_offset;
						t->indices[2] -= section_offset;
					}
				}
			}
		}
		mVertexArray.resize(wi);
	}
	
	/*
	if(count_smoothed || count_merged)
		printf("%d smoothed, %d merged\n", count_smoothed, count_merged);
	*/
}

void ModernMeshBuilder::GenerateNormals()
{
	int startvertexpos = 0;
	int startnormalpos = 0;
	for (auto& desc : mVertexDesc)
	{
		if (desc.type == ModernMesh::VertexElem::Type::Position3D)
			startvertexpos = desc.startpos;
		else if (desc.type == ModernMesh::VertexElem::Type::Normal3D)
			startnormalpos = desc.startpos;
	}

	std::vector<v3f> normals(mVertexArray.size(), v3f{ 0,0,0 });

	for (auto i = 0; i < mTriangles.size(); ++i)
	{
		auto tri = (ModernMesh::Triangle<unsigned int>*)mTriangles[i];

		unsigned char* startvertex0 = (unsigned char*)mVertexArray[tri->indices[0]];
		unsigned char* startvertex1 = (unsigned char*)mVertexArray[tri->indices[1]];
		unsigned char* startvertex2 = (unsigned char*)mVertexArray[tri->indices[2]];

		v3f* v0 = (v3f*)(startvertex0 + startvertexpos);
		v3f* v1 = (v3f*)(startvertex1 + startvertexpos);
		v3f* v2 = (v3f*)(startvertex2 + startvertexpos);

		v3f N;
		N.CrossProduct(*v1 - *v0, *v2 - *v0);
		N.Normalize();

		normals[tri->indices[0]] += N;
		normals[tri->indices[1]] += N;
		normals[tri->indices[2]] += N;
	}

	for (auto i = 0; i < normals.size(); ++i)
	{
		auto n = normals[i].Normalized();

		*((signed char*)mVertexArray[i] + startnormalpos + 0) = (signed char)(n.x * 127.5f);
		*((signed char*)mVertexArray[i] + startnormalpos + 1) = (signed char)(n.y * 127.5f);
		*((signed char*)mVertexArray[i] + startnormalpos + 2) = (signed char)(n.z * 127.5f);
	}
}


static v3f CalcTangent(void* v1, void* v2, void* v3, unsigned int offsetPos, unsigned int offsetUV)
{
	auto p1 = *GetVertexComp<v3f>(v1, offsetPos);
	auto p2 = *GetVertexComp<v3f>(v2, offsetPos);
	auto p3 = *GetVertexComp<v3f>(v3, offsetPos);
	
	auto uv1 = *GetVertexComp<v2f>(v1, offsetUV);
	auto uv2 = *GetVertexComp<v2f>(v2, offsetUV);
	auto uv3 = *GetVertexComp<v2f>(v3, offsetUV);

	float x1 = p2.x - p1.x;
	float x2 = p3.x - p1.x;
	float y1 = p2.y - p1.y;
	float y2 = p3.y - p1.y;
	float z1 = p2.z - p1.z;
	float z2 = p3.z - p1.z;

	float s1 = uv2.x - uv1.x;
	float s2 = uv3.x - uv1.x;
	float t1 = uv2.y - uv1.y;
	float t2 = uv3.y - uv1.y;

	float r = 1.0f / (s1 * t2 - s2 * t1);
	auto result = v3f((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);

	return result;
}


void ModernMeshBuilder::GenerateTangents()
{
	if (((mVertexArrayMask & ModuleRenderer::TEXCOORD_ARRAY_MASK) == 0) || ((mVertexArrayMask & ModuleRenderer::NORMAL_ARRAY_MASK) == 0))
	{
		KIGS_ERROR("Cannot compute tangents without normals and texture coordinates", 3);
	}

	int startvertexpos = 0;
	int startnormalpos = 0;
	int starttexcoordpos = 0;
	int starttangentpos = 0;
	for (auto& desc : mVertexDesc)
	{
		if (desc.type == ModernMesh::VertexElem::Type::Position3D)
			startvertexpos = desc.startpos;
		else if (desc.type == ModernMesh::VertexElem::Type::TextureCoordinate2D)
			starttexcoordpos = desc.startpos;
		else if (desc.type == ModernMesh::VertexElem::Type::Normal3D)
			startnormalpos = desc.startpos;
		else if (desc.type == ModernMesh::VertexElem::Type::Tangent3D)
			starttangentpos = desc.startpos;
	}

	std::vector<v3f> tangents(mVertexArray.size(), v3f{ 0,0,0 });

	for (int i = 0; i < mTriangles.size(); i++)
	{
		auto t = (ModernMesh::Triangle<unsigned int>*)mTriangles[i];
		
		auto tangent = CalcTangent(mVertexArray[t->indices[0]], mVertexArray[t->indices[1]], mVertexArray[t->indices[2]], startvertexpos, starttexcoordpos);
		
		tangents[t->indices[0]] += tangent;
		tangents[t->indices[1]] += tangent;
		tangents[t->indices[2]] += tangent;
	}


	for (int i = 0; i < tangents.size(); ++i)
	{
		auto v = mVertexArray[i];
		auto cn = GetVertexComp<unsigned char>(v, startnormalpos);
		Vector3D n(((float)(*cn))*(2.0f / 255.0f), ((float)(*(cn + 1)))*(2.0f / 255.0f), ((float)(*(cn + 2)))*(2.0f / 255.0f));

		auto ct = GetVertexComp<unsigned char>(v, starttangentpos);

		auto t = tangents[i].Normalized();

		v3f t1 = (n ^ t).Normalized();
		t = (t1 ^ n).Normalized();
		
		*(signed char*)(ct + 0) = (signed char)(t.x * 127.5f);
		*(signed char*)(ct + 1) = (signed char)(t.y * 127.5f);
		*(signed char*)(ct + 2) = (signed char)(t.z * 127.5f);
	}
}
