#ifndef _MODERNMESHBUILDER_H_
#define _MODERNMESHBUILDER_H_
	
#include "ModernMesh.h"
#include "DynamicGrowingBuffer.h"

// mesh builder
// used when mesh is imported from a not optimized format
class	ModernMeshBuilder
{
public:

	ModernMeshBuilder();
	~ModernMeshBuilder();
	void	StartGroup(CoreVector* description, int hintVertexBufferSize, int hintTriangleBufferSize);
	ModernMeshItemGroup*	EndGroup(bool optimize = true);
	ModernMeshItemGroup*	EndGroup(void * vertex, int vertexCount, void * index, int indexCount);
	ModernMeshItemGroup*	EndGroup(int vertex_count, v3f* vertices, v3f* normals, v4f* colors, v2f* texCoords, int face_count, v3u* faces, v3f offset);
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




	kstl::vector<int>											myVertexMergeBarriers;
	kstl::vector<int>											myIndicesMergeBarriers;
	kstl::vector<BBox>											mySectionsBBox;
	kstl::unordered_map<unsigned int, kstl::vector<BuildVertexStruct> >	myVertexBuilder;
	AbstractDynamicGrowingBuffer								myTriangles; 
	AbstractDynamicGrowingBuffer								myVertexArray;
	unsigned int												myCurrentVertexSize;
	unsigned int												myCurrentVertexInSize;
	unsigned int												myCurrentVertexBuilderSize;
	unsigned int												myVertexArrayMask;
	kstl::vector<ModernMesh::VertexElem>						myVertexDesc;
	bool														myGroupBuilding = false;

	unsigned int												myGroupCount;

	unsigned int												myTriangleChunkSize;

	kfloat														myTexCoordsScale;

	unsigned char*												myOneVertexData = nullptr;
	bool														myGenerateNormals = false;
	bool														myGenerateTangents = false;
	float														mySmoothNormalsThreshold = 1;
	bool														myNoMerge = false;

	bool myBBoxInit = false;
	BBox	myCurrentBBox;
	bool	mySectionInit = true;
	BBox	myCurrentSectionBBox;
};
		
#endif //_MODERNMESHBUILDER_H_
