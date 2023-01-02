#pragma once
#include <vector>
#include <map>
#include "CoreSTL.h"
#include "TecLibs/Tec3D.h"
#include "TecLibs/3D/3DObject/BBox.h"
#include "OctreeBase.h"

extern bool operator<(const v3f& U, const v3f& V);
extern bool operator<(const v3i& U, const v3i& V);

namespace Kigs
{
	namespace Utils
	{
		class MeshSimplificationOctree;
		class MeshSimplification;


		// store plane information
		struct MSPlaneStruct
		{
			v3f			mNormal;
			float		mDist;
			static v3f	mAxisNormal[3];
		};

		// store surface information
		struct MSSurfaceStruct
		{
			MSPlaneStruct	mPlane;
			float		mSurface;
			v3f			mInOctreePlaneP0;
		};

		struct MSTriangleInfo
		{
			v3f			mNormal;
			float		mSurface;
		};

		// store a vertice and it's associated surface index
		struct MSTriangleVertex
		{
			v3f		mVertex;
			u32		mSurfaceIndex;
			u32		mTriangleIndex;
		};

		inline bool operator ==(const MSTriangleVertex& first, const MSTriangleVertex& second)
		{
			if (first.mSurfaceIndex != second.mSurfaceIndex)
				return false;

			return first.mVertex == second.mVertex;
		}

		// OctreeNode content type for MeshSimplification
		class MSOctreeContent
		{
		public:

			MSOctreeContent() {}
			~MSOctreeContent() {
				if (mData)
					delete mData;
			}

			void	setContent(const v3f& p1, u32 surfaceIndex, u32 triangleindex)
			{
				if (!mData)
				{
					mData = new ContentData;
				}
				mData->mVertices.push_back({ p1, surfaceIndex,triangleindex });
			}

			const std::vector<MSTriangleVertex>* getContent() const
			{
				if (mData)
				{
					return &(mData->mVertices);
				}
				return nullptr;
			}

			static MSOctreeContent canCollapse(OctreeNodeBase** children, bool& doCollapse)
			{
				// collapse only if all nodes are empty
				MSOctreeContent returnval;
				doCollapse = false;

				return returnval;
			}

			bool	canOnlyBeSetOnLeaf() const
			{
				return true;
			}

			bool	isEmpty() const
			{
				if (mData)
				{
					return (mData->mVertices.size() == 0);
				}
				return true;
			}

			bool operator ==(const MSOctreeContent& other) const
			{
				if (other.isEmpty() && isEmpty())
					return true;

				if (other.isEmpty() || isEmpty())
					return false;

				if (other.getContent()->size() != getContent()->size())
					return false;

				if (other.getContent() == getContent())
					return true;


				return false;
			}

			friend class MeshSimplificationOctreeNode;
			friend class MeshSimplification;
			friend class BuildMeshFromEnveloppe;
		protected:
			// additionnal data only for enveloppe nodes
			class ContentData
			{
			protected:

			public:

				std::vector<std::pair<u32, float>>	getPrincipalSurfacesForDirection(const nodeInfo& n, const v3f& dir, const std::vector<MSSurfaceStruct>& Surfaces);

				void	initEnvelopeData();

				~ContentData()
				{
					if (mEnvelopeData)
						delete mEnvelopeData;

					mEnvelopeData = nullptr;
				}

				MeshSimplificationOctreeNode* getNeighbor(u32 pos)
				{
					if (!mEnvelopeData)
						return nullptr;
					for (auto& n : mEnvelopeData->mNeighbors)
					{
						if (n.first == pos)
						{
							return n.second;
						}
					}
					return nullptr;
				}

				std::pair<u32, u8>* getVertexForFreeFace(u8 freefaceMask)
				{
					if (!mEnvelopeData)
						return nullptr;
					for (auto& p : mEnvelopeData->mGoodIntersectionPoint)
					{
						if (freefaceMask & p.second)
						{
							return &p;
						}
					}
					return nullptr;
				}

				u32	countOppositeFreeFaces()
				{
					u32 result = 0;
					for (u32 i = 0; i < 3; i++)
					{
						u32 mask = (0x3 << (i << 1));
						if ((mEmptyNeighborsFlag & mask) == mask)
						{
							result++;
						}
					}

					return result;
				}

				std::vector<MSTriangleVertex>					mVertices;
				u8	mEmptyNeighborsFlag = 0;
				u8	mFreeFaceCount = 0;
				u8  mOtherFlags = 0;
				class additionnalEnvelopeData
				{
				public:
					std::vector<std::pair<u32, MeshSimplificationOctreeNode*>>	mNeighbors;
					// pair.first = index in BuildMeshFromEnveloppe mVertices  and pair.second is freeface mask
					std::vector<std::pair<u32, u8>>								mGoodIntersectionPoint;


					~additionnalEnvelopeData()
					{
						mNeighbors.clear();
						mGoodIntersectionPoint.clear();
					}
				};
				additionnalEnvelopeData* mEnvelopeData = nullptr;
			};

			ContentData* mData = nullptr;
		private:

		};


		class MeshSimplificationOctree;

		// specific octree node 
		class MeshSimplificationOctreeNode : public OctreeNode<MSOctreeContent>
		{
		public:
			MeshSimplificationOctreeNode() : OctreeNode<MSOctreeContent>()
			{

			}

			MeshSimplificationOctreeNode(const v3f& p1, u32 surfaceIndex, u32 triangleindex) : OctreeNode<MSOctreeContent>()
			{
				setContent(p1, surfaceIndex, triangleindex);
			}


			void	setContent(const v3f& p1, u32 surfaceIndex, u32 triangleindex)
			{
				mContentType.setContent(p1, surfaceIndex, triangleindex);
			}

			const std::vector<MSTriangleVertex>* getVertices() const
			{
				return mContentType.getContent();
			}

			void checkEmptyNeighbors(MeshSimplificationOctree& octree, nodeInfo& node);

			u8	getFreeFaceCount()
			{
				if (mContentType.mData)
					return mContentType.mData->mFreeFaceCount;

				return 0;
			}

			u8	getFreeFaceFlag()
			{
				if (mContentType.mData)
					return mContentType.mData->mEmptyNeighborsFlag;

				return 0;
			}

			std::set<u32>	getSurfaceIndexes()
			{
				std::set<u32> result;
				const std::vector<MSTriangleVertex>* packed = mContentType.getContent();
				if (packed)
				{
					for (const auto& p : *packed)
					{
						result.insert(p.mSurfaceIndex);
					}
				}
				return result;
			}

			friend class MeshSimplificationOctree;

			// second parameter is precomputed sum(absf(surface normal)*0.5);   
			bool	intersectSurface(const v3f& minp, const float& r, float dist, const v3f& n)
			{
				v3f e(0.5f, 0.5f, 0.5f);
				v3f c = minp + e;

				float s = Dot(n, c) - dist;

				return fabsf(s) < r;
			}

		protected:



		private:

		};

		// octree class specialized for mesh simplification
		class MeshSimplificationOctree : public OctreeBase<CoreModifiable>
		{
		public:

			friend class MeshSimplification;

			DECLARE_CLASS_INFO(MeshSimplificationOctree, OctreeBase<CoreModifiable>, Core);
			DECLARE_CONSTRUCTOR(MeshSimplificationOctree);

			virtual ~MeshSimplificationOctree()
			{

			}

			// set vertex list and transform it in octree coordinate
			void			setVertexList(const std::vector<v3f>& vlist);
			// transform given list from octree coords to world coords
			void			transformBackVertexList(std::vector<v3f>& vlist);

			void			setVoxelContent(u32 P1, u32 P2, u32 P3, u32 surfaceIndex);

			inline void		TransformInOctreeCoord(v3f& p)
			{
				p -= mBBox.m_Min;
				p *= mBBoxCoef;
				p += mTranslate;
			}

			BBox			getBoundingBox()
			{
				BBox	result;
				result.m_Min = mTranslate;
				result.m_Max = mBBox.m_Max - mBBox.m_Min;
				result.m_Max *= mBBoxCoef;
				result.m_Max += mTranslate;
				return result;
			}

			void			setBBox(const BBox& bbox)
			{

				v3f objectShift = mTranslate;
				objectShift *= mPrecision;

				BBox bboxTakingAccountOfTranslation(bbox);
				bboxTakingAccountOfTranslation.m_Max += objectShift;

				// compute cubic bounding box (octree is cubic)
				// so take the bigger edge

				v3f edges(bboxTakingAccountOfTranslation.m_Max);
				edges -= bboxTakingAccountOfTranslation.m_Min;

				float maxedge = edges.x;
				if (maxedge < edges.y)
					maxedge = edges.y;
				if (maxedge < edges.z)
					maxedge = edges.z;

				// add 2*mPrecision to have an empty border around bbox (1 on each side) (a bit more than 2 in fact, just in case)
				maxedge += 2.5f * mPrecision;

				// now search corresponding power of two

				float subdivision = maxedge / mPrecision;
				int poweroftwodecal = 0;
				while ((1 << poweroftwodecal) < subdivision)
				{
					poweroftwodecal++;
				}

				setValue("MaxDepth", poweroftwodecal);

				// so maxedge is now pow2 size of the octree
				maxedge = (float)(1 << poweroftwodecal);
				maxedge *= mPrecision;

				mBBoxCoef = 1.0f / mPrecision;

				// we want that bbox.m_Min - mBBox.m_Min = mPrecision
				// so mBBox.m_Min = bbox.m_Min - mPrecision

				mBBox.m_Min = bbox.m_Min;
				mBBox.m_Min -= mPrecision;

				mBBox.m_Max = mBBox.m_Min;
				mBBox.m_Max += maxedge;

			}

			void	setSurfaceList(const std::vector<MSSurfaceStruct>& surfs)
			{
				mAllSurfacesPtr = &surfs;
			}

			std::vector<nodeInfo>	floodFillEmpty(u32 setBrowsingFlag);

			template<typename F>
			void	floodFillWithCondition(const nodeInfo& startPos, F&& condition, u32 setBrowsingFlag);

			const std::vector<v3f>& getInOctreeCoordsVertices()
			{
				return mIOCVertices;
			}

		protected:

			static void		trianglePlaneIntersection(const v3f& triA, const v3f& triB, const v3f& triC, const MSPlaneStruct& p, std::set<v3f>& outSegTips);
			static void		segmentPlaneIntersection(const v3f& P1, const v3f& P2, const MSPlaneStruct& p, std::set<v3f>& outSegTips);
			static float	distFromPlane(const MSPlaneStruct& p, const v3f& P1);

			friend class MeshSimplificationOctreeNode;

			MeshSimplificationOctreeNode* getFinalNodeAtPos(MeshSimplificationOctreeNode* currentNode, const v3i& pos, int currentDepth);

			// precision in meter (so 0.01f = 1cm)
			maFloat		mPrecision = BASE_ATTRIBUTE(Precision, 0.01f);
			maVect3DF	mTranslate = BASE_ATTRIBUTE(Translate, 0.0f, 0.0f, 0.0f);
			const std::vector<MSSurfaceStruct>* mAllSurfacesPtr;
			BBox	mBBox;
			float	mBBoxCoef;

			u32 computeTriangleInfos(u32 P1, u32 P2, u32 P3);

			std::vector<MSTriangleInfo>		mTriangleInfos;
			std::vector<v3f>				mIOCVertices;


		};

		template<typename F>
		inline void	MeshSimplificationOctree::floodFillWithCondition(const nodeInfo& startPos, F&& condition, u32 setBrowsingFlag)
		{
			recursiveFloodFill t(*this, nullptr, setBrowsingFlag);
			t.run(startPos, condition);
		}
	}
}