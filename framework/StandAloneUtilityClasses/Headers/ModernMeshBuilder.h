#ifndef _MODERNMESHBUILDER_H_
#define _MODERNMESHBUILDER_H_
	
#include "ModernMesh.h"
#include "DynamicGrowingBuffer.h"


// ****************************************
// * ModernMeshBuilder class
// * --------------------------------------
/**
* \file	ModernMeshBuilder.h
* \class	ModernMeshBuilder
* \ingroup Renderer
* \brief Utility class to create a ModernMesh instance from triangle lists. 
*
*/
// ****************************************
class	ModernMeshBuilder
{
public:

	ModernMeshBuilder();
	~ModernMeshBuilder();
	void	StartGroup(CoreVector* description, int hintVertexBufferSize, int hintTriangleBufferSize);
	SP<ModernMeshItemGroup>	EndGroup(bool optimize = true);
	SP<ModernMeshItemGroup>	EndGroup(void * vertex, int vertexCount, void * index, int indexCount);
	SP<ModernMeshItemGroup>	EndGroup(int vertex_count, v3f* vertices, v3f* normals, v4f* colors, v2f* texCoords, int face_count, v3u* faces, v3f offset);
	void	AddTriangle(void* v1, void* v2, void* v3);
	void PlaceMergeBarrier();

protected:

	struct	SortTriangle
	{
	public:
		unsigned int	Score;
		unsigned int	Index;

		bool	operator < (const SortTriangle& str) const
		{
			return Score<str.Score;
		}
	};

	
	void	OptimiseForCache();
	
	void	GenerateNormals();
	void	GenerateTangents();

	void SnapToGridAndMerge();
	// convert from full float format in entry to compressed output
	void* convert(void* v);

	unsigned int	getID(void* v);
	unsigned int	addVertex(void* v);

	bool			areEqual(void* v1, void* v2);

	struct BuildVertexStruct
	{
		unsigned int	index;
		void*			data;
	};

	struct OptimiseBuildVertexStruct
	{
		unsigned int	useCount;
		int	newIndex;
	};

	class	OptimiseTriangle
	{
	public:
		unsigned int	Score1;	// 8 bits => already processed points + 8 bits 255-min used count 
								//  + 8 bits max delta between max used and min used count

		void	ComputeScore(OptimiseBuildVertexStruct* varray, int currentVertexIndex);
		ModernMesh::Triangle<unsigned int>*	triangleStruct;

		bool	operator < (const OptimiseTriangle& str) const
		{
			return Score1<str.Score1;
		}
	};




	kstl::vector<int>											mVertexMergeBarriers;
	kstl::vector<int>											mIndicesMergeBarriers;
	kstl::vector<BBox>											mSectionsBBox;
	kigs::unordered_map<unsigned int, kstl::vector<BuildVertexStruct> >	mVertexBuilder;
	AbstractDynamicGrowingBuffer								mTriangles; 
	AbstractDynamicGrowingBuffer								mVertexArray;
	unsigned int												mCurrentVertexSize;
	unsigned int												mCurrentVertexInSize;
	unsigned int												mCurrentVertexBuilderSize;
	unsigned int												mVertexArrayMask;
	kstl::vector<ModernMesh::VertexElem>						mVertexDesc;
	bool														mGroupBuilding = false;

	unsigned int												mGroupCount;

	unsigned int												mTriangleChunkSize;

	kfloat														mTexCoordsScale;

	unsigned char*												mOneVertexData = nullptr;
	bool														mGenerateNormals = false;
	bool														mGenerateTangents = false;
	float														mSmoothNormalsThreshold = 1;
	bool														mNoMerge = false;

	bool	mBBoxInit = false;
	BBox	mCurrentBBox;
	bool	mSectionInit = true;
	BBox	mCurrentSectionBBox;
};
		
#endif //_MODERNMESHBUILDER_H_
