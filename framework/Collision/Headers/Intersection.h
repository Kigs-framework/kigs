#pragma once

#include "CoreModifiable.h"
#include "TecLibs/Tec3D.h"
#include "TecLibs/3D/3DObject/BBox.h"
#include "AABBTree.h"

#include "TecLibs/Math/IntersectionAlgorithms.h"

namespace Kigs
{
	namespace Maths
	{
		struct Hit;
	}
	namespace Collide
	{
		class Plane;

		bool IntersectRayPlane(const Point3D& rOrigin, const Vector3D& rVector,
			const Point3D& pOrigin, const Vector3D& pNormal,
			double& IntersectionDistance, CollisionBaseObject* pCollisionObject);

		bool IntersectRayPlane(const Point3D& rOrigin, const Vector3D& rVector,
			Plane* plane, double& IntersectionDistance);

		/*!   \brief return true if ray intersect mesh
			the intersected triangle, distance and uv on triangle are returned
		*/
		bool IntersectionRayAABBTree(const Point3D* vertex_list, const Point3D& RayOrigin, const Vector3D& RayDirection,
			const AABBTreeNode& pAABB, double& IntersectionDistance, Vector3D& IntersectionNormal, float& CoordTri_u, float& CoordTri_v, s32* outTriangleIndices = nullptr);

		bool IntersectionRayAABBTree(const Point3D* vertex_list, const Point3D& RayOrigin, const Vector3D& RayDirection,
			const AABBTreeNode& pAABB, std::vector<Maths::Hit>&);

		template<int plane>
		bool IntersectionAxisAlignedPlaneAABBTree(const Point3D* vertex_list, float planeCoordinate, const AABBTreeNode& pAABB, std::vector< Segment3D >& result, const std::vector< u8 >& side)
		{
			bool bresult = false;

			int plane_1 = (plane + 1) % 3;
			int plane_2 = (plane + 2) % 3;


			// check if plane intersect aabb bbox

			if ((pAABB.GetBBox().m_Min[plane] > planeCoordinate) || (pAABB.GetBBox().m_Max[plane] < planeCoordinate))
			{
				return false;
			}

			if (!pAABB.GetSon1()) // leaf
			{
				bool found = false;
				auto triangle_array = pAABB.GetTriangleArray();

				for (int i = 0; i < pAABB.GetTriangleCount(); i++)
				{

					unsigned int indexes[3] = { triangle_array[i].A(),triangle_array[i].B(),triangle_array[i].C() };
					unsigned int intersect = side[indexes[0]] | side[indexes[1]] | side[indexes[2]];

					if (intersect == 3)
					{
						int pointIndexInSegment = 0;
						int swapOffset = 0;

						if (side[indexes[0]] == 2)
						{
							swapOffset = 1;
						}

						result.push_back(Segment3D());

						Segment3D& ToAdd = result.back();

						// test each segment
						for (unsigned int j = 0; j < 3; j++)
						{
							unsigned int j1 = (j + 1) % 3;

							int pointIndexInSegmentMod2 = (pointIndexInSegment + swapOffset) & 1;

							if ((side[indexes[j]] | side[indexes[j1]]) == 3)
							{
								// always cut in same direction to avoid precision mismatch
								int cutj1 = indexes[j];
								int cutj2 = indexes[j1];
								if (side[indexes[j]] == 2)
								{
									int cutj1 = indexes[j1];
									int cutj2 = indexes[j];
								}

								float c = (planeCoordinate - vertex_list[cutj1][plane]) / (vertex_list[cutj2][plane] - vertex_list[cutj1][plane]);

								ToAdd.pts[pointIndexInSegmentMod2][plane] = planeCoordinate;
								ToAdd.pts[pointIndexInSegmentMod2][plane_1] = vertex_list[cutj1][plane_1] + c * ((vertex_list[cutj2][plane_1] - vertex_list[cutj1][plane_1]));
								ToAdd.pts[pointIndexInSegmentMod2][plane_2] = vertex_list[cutj1][plane_2] + c * ((vertex_list[cutj2][plane_2] - vertex_list[cutj1][plane_2]));

								pointIndexInSegment++;

							}
							else if (side[indexes[j]] == 0)
							{
								ToAdd.pts[pointIndexInSegmentMod2] = vertex_list[indexes[j]];
								pointIndexInSegment++;
							}

						}

						// check for too small segment
						if (DistSquare(ToAdd.p1, ToAdd.p2) < 0.000001f)
						{
							result.pop_back();
						}
						else
						{
							found = true;
						}

#ifdef _DEBUG
						if (pointIndexInSegment != 2)
						{
							KIGS_ERROR("Bad Triangle plane intersection\n", 1);
						}
#endif

					}
				}
				return found;
			}

			bresult |= IntersectionAxisAlignedPlaneAABBTree<plane>(vertex_list, planeCoordinate, *pAABB.GetSon1(), result, side);

			bresult |= IntersectionAxisAlignedPlaneAABBTree<plane>(vertex_list, planeCoordinate, *pAABB.GetSon2(), result, side);


			return bresult;
		}

		template<int plane>
		bool IntersectionAxisAlignedPlaneAABBTree(float planeCoordinate, const AABBTree& pAABB, std::vector< Segment3D >& result)
		{
			// check if plane intersect aabb bbox
			if ((pAABB.GetBBox().m_Min[plane] > planeCoordinate) || (pAABB.GetBBox().m_Max[plane] < planeCoordinate))
			{
				return false;
			}

			// check each vertex side

			const Point3D* vertex_list = pAABB.GetVertexList();
			unsigned int vertex_count = pAABB.GetVertexCount();

			std::vector< u8 > sides;
			sides.resize(vertex_count, 0);

			for (unsigned int i = 0; i < vertex_count; i++)
			{
				float coord = vertex_list[i][plane];
				if (coord > planeCoordinate)
				{
					sides[i] = 1;
				}
				else if (coord < planeCoordinate)
				{
					sides[i] = 2;
				}
			}

			return IntersectionAxisAlignedPlaneAABBTree<plane>(vertex_list, planeCoordinate, pAABB, result, sides);
		}

		bool IntersectionPlaneAABBTree(const Point3D* vertex_list, const Point3D& o, const Vector3D& n, const AABBTreeNode& pAABB, std::vector< Segment3D >& result, const std::vector< u8 >& side);

		bool IntersectionPlaneAABBTree(const Point3D& o, const Vector3D& n, const AABBTree& pAABB, std::vector< Segment3D >& result);


		/*!   \brief return list of intersection between AABBTree and segment // to an axis (x, y or z)
				for each intersection, return a Point2D containing coordinate of the intersection and direction (+ or -)
		*/
		void IntersectionAxisAABBTree(std::vector<Point2D>& coordinates, Point3D& RayOrigin, const int axisType, AABBTreeNode& pAABB, Point3D* VertexArray);


		bool IntersectionXAxisTriangle(Point3D& RayOrigin, const Point3D& A, const Point3D& B, const Point3D& C, float& x, float& dir);
		bool IntersectionYAxisTriangle(Point3D& RayOrigin, const Point3D& A, const Point3D& B, const Point3D& C, float& y, float& dir);
		bool IntersectionZAxisTriangle(Point3D& RayOrigin, const Point3D& A, const Point3D& B, const Point3D& C, float& z, float& dir);

		inline bool SortAxisIntersection(const Point2D& p1, const Point2D& p2)
		{
			return p1.x < p2.x;
		}

		inline bool IntersectionPlaneBBox(const Point3D& o, const Vector3D& n, const BBox& currentBBox)
		{
			int ix = signbit(n.x) ? 0 : 1;
			int iy = signbit(n.y) ? 0 : 1;
			int iz = signbit(n.z) ? 0 : 1;

			float d = o.x * n.x + o.y * n.y + o.z * n.z;

			float dot1 = currentBBox.m_MinMax[ix].x * n.x + currentBBox.m_MinMax[iy].y * n.y + currentBBox.m_MinMax[iz].z * n.z;

			if (dot1 <= d)
			{
				return false;
			}

			ix ^= 1; iy ^= 1; iz ^= 1;
			dot1 = currentBBox.m_MinMax[ix].x * n.x + currentBBox.m_MinMax[iy].y * n.y + currentBBox.m_MinMax[iz].z * n.z;

			if (dot1 >= d)
			{
				return false;
			}

			return true;
		}

	}
}