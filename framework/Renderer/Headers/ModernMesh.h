#pragma once


#include "HDrawable.h"
#include "TecLibs/Tec3D.h"
#include "TecLibs/3D/3DObject/BBox.h"
#include "BufferedFile.h"
#include "maBuffer.h"
#include "CoreMap.h"

#include <atomic>

namespace Kigs
{
	namespace Core
	{
		class AbstractDynamicGrowingBuffer;
	}
	namespace Utils
	{
		class ModernMeshBuilder;
	}
	namespace Draw
	{
		class Material;
		class ModernMeshItemGroup;


		// ****************************************
		// * ModernMesh class
		// * --------------------------------------
		/**
		* \file	ModernMesh.h
		* \class	ModernMesh
		* \ingroup Renderer
		* \brief	Optimized mesh class.
		*
		*/
		// ****************************************
		class ModernMesh : public HDrawable
		{
		public:
			friend class ModernMeshItemGroup;

			DECLARE_CLASS_INFO(ModernMesh, HDrawable, Renderer)
				DECLARE_CONSTRUCTOR(ModernMesh);

			SmartPointer<ModernMesh> CreateClonedMesh(const std::string& name, bool reuse_materials);

			bool BBoxUpdate(double/* time */) override { return true; }

			void GetNodeBoundingBox(v3f& pmin, v3f& pmax) const override
			{
				pmin = mBoundingBox.m_Min;
				pmax = mBoundingBox.m_Max;
			}

			void ComputeTangents(bool useTextureCoords);

			void GetItemGroup(std::vector<ModernMeshItemGroup*>& list);
			void GetVertex(s32& VCount, float*& VArray);
			void GetTriangles(s32& VCount, float*& VArray, s32& ICount, void*& IArray);

			void ComputeNormals();

			void ApplyScaleFactor(float scaleFactor);
			void FlipAxis(s32 axisX, s32 axisY, s32 axisZ);

			void SetCanFree();


			class TriangleBase // Nothing there
			{
			public:
			};

			template<typename T>
			class Triangle : public TriangleBase
			{
			public:
				//! first indice
				T indices[3];
			};

			// describe one element in the vertex
			class VertexElem
			{
			public:

				std::string	name;
				u32	size;
				u32	startpos;
				u32	mask;
				u32	elemCount;
				u32	inSize;
				u32	inStartPos;
				enum class Type : unsigned char
				{
					Position3D,
					Position2D,
					TextureCoordinate3D,
					TextureCoordinate2D,
					Normal3D,
					Tangent3D,
					ColorRGB,
					ColorRGBA,
					BoneWeights,
					BoneIndexes
				} type;
			};


			void StartMeshBuilder();
			void StartMeshGroup(CoreMap<std::string>* description, s32 hs32VertexBufferSize = 256, s32 hs32TriangleBufferSize = 256);
			void AddTriangle(void* v1, void* v2, void* v3);
			void PlaceMergeBarrier();
			SP<ModernMeshItemGroup> EndMeshGroup();
			SP<ModernMeshItemGroup> EndMeshGroup(void* vertex, s32 vertexCount, void* index, s32 indexCount);

			SP<ModernMeshItemGroup> EndMeshGroup(s32 vertex_count, v3f* vertices, v3f* normals, v4f* colors, v2f* texCoords, s32 face_count, v3u* faces, v3f offset = { 0,0,0 }, SP<ModernMeshItemGroup> reuse_group = nullptr);

			void EndMeshBuilder();
			virtual ~ModernMesh();

		protected:
#ifdef KEEP_NAME_AS_STRING
			// give a chance to the object to add dynamic attribute to its export
			void PrepareExport(ExportSettings* settings) override;


			// called after the object was exported (remove dynamics added before export)
			void EndExport(ExportSettings* settings) override;
#endif



			void InitModifiable() override;



			virtual void  InitBoundingBox();

			Utils::ModernMeshBuilder* mCurrentMeshBuilder = nullptr;
			BBox mBoundingBox = BBox{ v3f(0.0f, 0.0f, 0.0f), v3f(-1.0f, -1.0f, -1.0f) };
			bool mWasBuild = false;

			//! name of the file to read in load method
			maString mFileName = BASE_ATTRIBUTE(FileName, "");

#ifdef _DEBUG
			maBool mShowVertex = BASE_ATTRIBUTE(ShowVertex, false);
			maBool mOptimize = BASE_ATTRIBUTE(Optimize, true);
#else
			maBool mOptimize = BASE_ATTRIBUTE(Optimize, true);
#endif

			maBool mWireMode = BASE_ATTRIBUTE(WireMode, false);
#ifdef KIGS_TOOLS
			maBool mDrawNormals = BASE_ATTRIBUTE(DrawNormals, false);
			maBool mDrawUVs = BASE_ATTRIBUTE(DrawUVs, false);
#endif

		};
		// ****************************************
		// * ModernMeshItemGroup class
		// * --------------------------------------
		/**
		* \file	ModernMesh.h
		* \class	ModernMeshItemGroup
		* \ingroup Renderer
		* \brief	Group of triangles with the same characteristics.
		*
		*/
		// ****************************************

		class ModernMeshItemGroup : public Drawable
		{
		public:
			friend class ModernMesh;
			DECLARE_CLASS_INFO(ModernMeshItemGroup, Drawable, Renderer)
				DECLARE_CONSTRUCTOR(ModernMeshItemGroup);
			virtual ~ModernMeshItemGroup();

			bool Draw(TravState* travstate) override;
			bool PreDraw(TravState* travstate) override;
			bool PostDraw(TravState* travstate) override;

			virtual void SetupClonedMesh(ModernMeshItemGroup* cloned_from);

			void SetIndexBoundaries(std::vector<s32> boundaries)
			{
				mBoundaries = std::move(boundaries);
			}

			void SetColliderIndexBoundaries(std::vector<s32> boundaries)
			{
				mColliderBoundaries = std::move(boundaries);
			}

			std::vector<s32> mBoundaries;
			std::vector<s32> mColliderBoundaries;

			maULong mBoundariesMask = BASE_ATTRIBUTE(BoundariesMask, UINT64_MAX);


			u32 GetSelfDrawingNeeds()  override
			{
				return ((u32)Need_Postdraw) | ((u32)Need_Predraw) | ((u32)Need_Draw);
			}

			s32 getTriangleCount() { return mTriangleCount; }
			s32 getVertexCount() { return mVertexCount; }
			s32 getVertexSize() { return mVertexSize; }


			bool	BBoxUpdate(double /* time*/)  override { return true; }

			std::atomic_int mCanFreeBuffers = { 0 };
			//! List of Triangles
			maBuffer mTriangleBuffer = BASE_ATTRIBUTE(TriangleBuffer, "");
			maBuffer mVertexBufferArray = BASE_ATTRIBUTE(VertexBufferArray, "");
			//! Triangle Count
			s32	mTriangleCount = 0;
			s32	mVertexCount = 0;
			s32	mVertexSize = 0;

			bool mOwnedBuffer = true;
			u32 mVertexBuffer = 0xFFFFFFFF;
			u32 mIndexBuffer = 0xFFFFFFFF;
			u32 mIndexType;

			maInt						mCullMode = BASE_ATTRIBUTE(CullMode, 1);
			maFloat						mTexCoordsScale = BASE_ATTRIBUTE(TexCoordsScale, 0.01f);;
			maBool						mInstanced = BASE_ATTRIBUTE(Instanced, false);
			maBool						mNoLight = BASE_ATTRIBUTE(NoLight, false);


			maBool mTestOcclusion = BASE_ATTRIBUTE(TestOcclusion, false);
			u64 mOcclusionQueryId = -1;
			u64 mLastOcclusionResult = 1;
			int mLastFrameOcclusionTested = 0;

			u32							mVertexArrayMask = 0;	// give available data (should be set in material ?)
			std::vector<ModernMesh::VertexElem>	mVertexDesc;
			void ComputeTangents(bool useTextureCoords);
			void ComputeNormals();
			void ApplyScaleFactor(float scaleFactor);
			void FlipAxis(s32 axisX, s32 axisY, s32 axisZ);

			void* GetVertexBuffer(size_t& size) { size = mVertexBufferArray.length(); return mVertexBufferArray.buffer(); }
			void* GetIndexBuffer(size_t& size) { size = mTriangleBuffer.length(); return mTriangleBuffer.buffer(); }



		protected:
			std::vector<ModernMesh::VertexElem>* GetVertexDesc() { return &mVertexDesc; }

			const ModernMesh::VertexElem* GetVertexDesc(const std::string& descname)
			{
				auto itr = mVertexDesc.begin();
				for (; itr != mVertexDesc.end(); ++itr)
				{
					if (itr->name == descname)
						return &(*itr);
				}
				return 0;

			}
			void GetTangent(unsigned char* v1, unsigned char* v2, unsigned char* v3, u32 tcpos, Vector3D& sdir);
			/**
			* \brief	initialize modifiable
			* \fn 		void InitModifiable() override
			*/
			void InitModifiable() override;

			void ImportFromCData(const std::string& imported) override;

			void DoFirstDrawInit();

#ifdef KEEP_NAME_AS_STRING
			// give a chance to the object to add dynamic attribute to its export
			void PrepareExport(ExportSettings* settings) override;

			// called after the object was exported (remove dynamics added before export)
			void EndExport(ExportSettings* settings) override;
#endif
		};

	}
}


