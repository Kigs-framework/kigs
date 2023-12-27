#pragma once

#include "TecLibs/Tec3D.h"
#include "coreTypes.h"

namespace Kigs
{
	namespace Core
	{
		class CoreModifiable;
	}
	namespace Scene
	{
		class Node3D;
	}
	namespace Collide
	{
		class CollisionBaseObject;
	}

	namespace Maths
	{
	
		struct Hit
		{
			double							HitDistance = DBL_MAX;
			p3f							HitPosition;
			v3f						HitNormal;
			Core::CoreModifiable*			HitActor = nullptr;
			Scene::Node3D*					HitNode = nullptr;
			Core::CoreModifiable*			HitFlagNode = nullptr;
			unsigned int					HitFlag = 0u;
			Collide::CollisionBaseObject*	HitCollisionObject = nullptr;
			s32								HitTriangleVertexIndices[3] = { -1,-1,-1 };
			s32								HitFaceIndex = -1;
			void Clear()
			{
				*this = Hit{};
			}
		};



		/*!
		test if a ray, in fact a segment (Origin, direction, length) intersect a BBox (min and max points)
		*/
		inline bool IntersectionFastRayBBoxTest(const p3f& RayOrigin,
			const v3f& RayDirection, float MaxDist,
			const p3f& BBoxMin, const p3f& BBoxMax)
		{
			//! bbox center
			v3f center(BBoxMin);
			center += BBoxMax;
			center *= 0.5f;

			//! bbox diag
			v3f diag(BBoxMax);
			diag -= BBoxMin;
			diag *= 0.5f;

			float r = glm::length2(diag);

			v3f Q(center);
			Q -= RayOrigin;
			float c = glm::length2(Q);
			float v = dot(Q, RayDirection);

			if ((v < 0.0f) && (c > r))
			{
				return false;
			}
			else if ((v > 0.0f) && ((c - MaxDist * MaxDist) > r))
			{
				return false;
			}

			float d = r - (c - v * v);
			// If there is no intersection, return false
			if (d < -0.05f) return false;
			else return true;
		}

		/*!
		test if a ray(Origin, direction) intersect a Sphere (center, radius) and return minimal distance of intersection
		*/
		inline bool IntersectionRaySphere(const p3f& RayOrigin,
			const v3f& RayDirection,
			const p3f& SphereCenter,
			const float SphereRadius,
			float& FirstIntersectionDistance)
		{
			v3f Q = SphereCenter - RayOrigin;

			float c = length(Q);
			float v = dot(Q, RayDirection);

			float d = SphereRadius * SphereRadius - (c * c - v * v);

			// If there was no intersection, return false
			if (d < 0.0f) return false;

			// Compute the first distance of intersection
			FirstIntersectionDistance = v - sqrtf(d);
			return true;
		}

		/*!
		test if a ray(Origin, direction) intersect a Sphere (center, radius) and return both intersection distance
		both distances can be equal if ray is tangent to the sphere
		*/
		inline bool MultiIntersectionRaySphere(const p3f& RayOrigin,
			const v3f& RayDirection,
			const p3f& SphereCenter,
			const float SphereRadius,
			float& FirstIntersectionDistance,
			float& SecondIntersectionDistance)
		{

			v3f Q(RayOrigin- SphereCenter);
			float a = length2(RayDirection);
			float b = dot(Q, RayDirection);
			float c = length2(Q) - SphereRadius * SphereRadius;
			float d = b * b - a * c;
			if (d < 0.0f) return false;
			d = sqrtf(d);
			FirstIntersectionDistance = (-b - d) / a;
			SecondIntersectionDistance = (-b + d) / a;
			return true;
		}

		/*!
		test if a ray(Origin, direction) intersect a BBox (min and max points)
		return nearest intersection point and distance
		*/
		inline bool IntersectionRayBBox(const p3f& RayOrigin, const v3f& RayDirection,
			const p3f& BBoxMin, const p3f& BBoxMax,
			p3f& IntersectionPoint, v3f& IntersectionNormal,
			double& IntersectionDistance)
		{
			enum IRBB_Side
			{
				IRBB_NEGATIVE = 0,
				IRBB_POSITIVE = 1,
				IRBB_MIDDLE = 2,
			};


			IRBB_Side SideX, SideY, SideZ;
			p3f candidatePlane;
			double MinT = 1000.0;
			bool Inside = true;
			bool IntersectionFound = false;
			p3f CandidatePoint;

			//! first test along X axis
			if (RayOrigin.x < BBoxMin.x)
			{
				SideX = IRBB_NEGATIVE;
				candidatePlane.x = BBoxMin.x;
				Inside = false;
			}
			else
			{
				if (RayOrigin.x > BBoxMax.x)
				{
					SideX = IRBB_POSITIVE;
					candidatePlane.x = BBoxMax.x;
				}
				else
				{
					SideX = IRBB_MIDDLE;
					if (RayDirection.x < 0.0f)
					{
						candidatePlane.x = BBoxMin.x;
					}
					else
					{
						candidatePlane.x = BBoxMax.x;
					}
				}
			}

			//! then test along Y axis
			if (RayOrigin.y < BBoxMin.y)
			{
				SideY = IRBB_NEGATIVE;
				candidatePlane.y = BBoxMin.y;
				Inside = false;
			}
			else
			{
				if (RayOrigin.y > BBoxMax.y)
				{
					SideY = IRBB_POSITIVE;
					candidatePlane.y = BBoxMax.y;
					Inside = false;
				}
				else
				{
					SideY = IRBB_MIDDLE;
					if (RayDirection.y < 0.0f)
					{
						candidatePlane.y = BBoxMin.y;
					}
					else
					{
						candidatePlane.y = BBoxMax.y;
					}
				}
			}

			//! and finally test along Z axis
			if (RayOrigin.z < BBoxMin.z)
			{
				SideZ = IRBB_NEGATIVE;
				candidatePlane.z = BBoxMin.z;
				Inside = false;
			}
			else
			{
				if (RayOrigin.z > BBoxMax.z)
				{
					SideZ = IRBB_POSITIVE;
					candidatePlane.z = BBoxMax.z;
					Inside = false;
				}
				else
				{
					SideZ = IRBB_MIDDLE;
					if (RayDirection.z < 0.0f)
					{
						candidatePlane.z = BBoxMin.z;
					}
					else
					{
						candidatePlane.z = BBoxMax.z;
					}
				}
			}

			//! then search for minimal ray to plane distance 
			if ((SideX != IRBB_MIDDLE || Inside) && RayDirection.x != 0.0f)
			{
				IntersectionDistance = (candidatePlane.x - RayOrigin.x) / RayDirection.x;
				if (IntersectionDistance >= 0.0f)
				{
					CandidatePoint.y = RayOrigin.y + (float)IntersectionDistance * RayDirection.y;
					if (CandidatePoint.y >= BBoxMin.y && CandidatePoint.y <= BBoxMax.y)
					{
						CandidatePoint.z = RayOrigin.z + (float)IntersectionDistance * RayDirection.z;
						if (CandidatePoint.z >= BBoxMin.z && CandidatePoint.z <= BBoxMax.z)
						{
							CandidatePoint.x = candidatePlane.x;
							MinT = IntersectionDistance;
							IntersectionFound = true;
							IntersectionPoint = CandidatePoint;
							IntersectionNormal = v3f((SideX == IRBB_POSITIVE) ? 1.0f : -1.0f, 0.0f, 0.0f); // side X
						}
					}
				}
			}


			if ((SideY != IRBB_MIDDLE || Inside) && RayDirection.y != 0.0f)
			{
				IntersectionDistance = (candidatePlane.y - RayOrigin.y) / RayDirection.y;
				if (IntersectionDistance >= 0.0f && IntersectionDistance < MinT)
				{
					CandidatePoint.x = RayOrigin.x + (float)IntersectionDistance * RayDirection.x;
					if (CandidatePoint.x >= BBoxMin.x && CandidatePoint.x <= BBoxMax.x)
					{
						CandidatePoint.z = RayOrigin.z + (float)IntersectionDistance * RayDirection.z;
						if (CandidatePoint.z >= BBoxMin.z && CandidatePoint.z <= BBoxMax.z)
						{
							CandidatePoint.y = candidatePlane.y;
							MinT = IntersectionDistance;
							IntersectionFound = true;
							IntersectionPoint = CandidatePoint;
							IntersectionNormal = v3f(0.0f, (SideY == IRBB_POSITIVE) ? 1.0f : -1.0f, 0.0f); // side Y
						}
					}
				}
			}


			if ((SideZ != IRBB_MIDDLE || Inside) && RayDirection.z != 0.0f)
			{
				IntersectionDistance = (candidatePlane.z - RayOrigin.z) / RayDirection.z;
				if (IntersectionDistance >= 0.0f && IntersectionDistance < MinT)
				{
					CandidatePoint.y = RayOrigin.y + (float)IntersectionDistance * RayDirection.y;
					if (CandidatePoint.y >= BBoxMin.y && CandidatePoint.y <= BBoxMax.y)
					{
						CandidatePoint.x = RayOrigin.x + (float)IntersectionDistance * RayDirection.x;
						if (CandidatePoint.x >= BBoxMin.x && CandidatePoint.x <= BBoxMax.x)
						{
							CandidatePoint.z = candidatePlane.z;
							MinT = IntersectionDistance;
							IntersectionFound = true;
							IntersectionPoint = CandidatePoint;
							IntersectionNormal = v3f(0.0f, 0.0f, (SideZ == IRBB_POSITIVE) ? 1.0f : -1.0f);  // side Z
						}
					}
				}
			}


			IntersectionDistance = MinT;
			return IntersectionFound;
		}


		inline bool IntersectionRayBBox(const p3f& RayOrigin, const v3f& RayDirection,
			const p3f& BBoxMin, const p3f& BBoxMax)
		{
			auto getLimit = [&](int sign) -> auto&
			{
				if (sign == 0)return BBoxMin;
				return BBoxMax;
			};

			int sign[3];
			v3f invdir = 1.0f / RayDirection;
			sign[0] = (invdir.x < 0);
			sign[1] = (invdir.y < 0);
			sign[2] = (invdir.z < 0);


			float tmin = (getLimit(sign[0]).x - RayOrigin.x) * (invdir.x);
			float tmax = (getLimit(1 - sign[0]).x - RayOrigin.x) * (invdir.x);
			float tymin = (getLimit(sign[1]).y - RayOrigin.y) * (invdir.y);
			float tymax = (getLimit(1 - sign[1]).y - RayOrigin.y) * (invdir.y);
			if ((tmin > tymax) || (tymin > tmax))
				return false;
			if (tymin > tmin)
				tmin = tymin;
			if (tymax < tmax)
				tmax = tymax;
			float tzmin = (getLimit(sign[2]).z - RayOrigin.z) * (invdir.z);
			float tzmax = (getLimit(1 - sign[2]).z - RayOrigin.z) * (invdir.z);
			if ((tmin > tzmax) || (tzmin > tmax))
				return false;
			if (tzmin > tmin)
				tmin = tzmin;
			if (tzmax < tmax)
				tmax = tzmax;
			return true;
		}

		inline bool IntersectionSegmentBBox(const p3f& p1, const p3f& p2, const p3f& BBoxMin, const p3f& BBoxMax)
		{
			auto d = (p2 - p1) * 0.5f;
			auto e = (BBoxMax - BBoxMin) * 0.5f;
			auto c = p1 + d - (BBoxMin + BBoxMax) * 0.5f;
			auto ad = v3f(fabs(d.x), fabs(d.y), fabs(d.z));

			if (fabsf(c[0]) > e[0] + ad[0])
				return false;
			if (fabsf(c[1]) > e[1] + ad[1])
				return false;
			if (fabsf(c[2]) > e[2] + ad[2])
				return false;

			if (fabsf(d[1] * c[2] - d[2] * c[1]) > e[1] * ad[2] + e[2] * ad[1] + FLT_EPSILON)
				return false;
			if (fabsf(d[2] * c[0] - d[0] * c[2]) > e[2] * ad[0] + e[0] * ad[2] + FLT_EPSILON)
				return false;
			if (fabsf(d[0] * c[1] - d[1] * c[0]) > e[0] * ad[1] + e[1] * ad[0] + FLT_EPSILON)
				return false;

			return true;
		}


		/*! Intersection between a ray (origin, direction) and a triangle (ABC)
		use : http://www.acm.org/jgt/papers/MollerTrumbore97/code.html algorithm
		Return : IntersectionDistance, u,v coords of intersection in triangle coordinates
		global coordinate system intersection point can be computed by : intersection==A*u + B*v + C*(1-u-v)
		*/
		inline bool IntersectionRayTriangle(const p3f& RayOrigin, const v3f& RayDirection,
			const p3f& A, const p3f& B, const p3f& C,
			double& IntersectionDistance,
			float& TriangleCoord_u,
			float& TriangleCoord_v,
			v3f& triangle_normal
		)
		{
			v3f edge1(B-A);
			v3f edge2(C-A);
			v3f pvec, qvec;
			float det, inv_det;

			pvec  = cross(RayDirection, edge2);
			det = dot(edge1, pvec);

			//! if ray and triangle plane are parallel, no intersection
			if (det > -FLT_EPSILON && det < FLT_EPSILON)
				return false;

			inv_det = 1.0f / det;

			//! calculate distance from A to ray origin 
			v3f tvec(RayOrigin-A);

			//! calculate u parameter and test if u is between 0 and 1, else no intersection
			TriangleCoord_u = dot(tvec, pvec) * inv_det;
			if (TriangleCoord_u < 0.0f || TriangleCoord_u > 1.0f)
				return false;

			qvec = cross(tvec, edge1);

			//! calculate v parameter and test if v is between 0 and 1, else no intersection
			TriangleCoord_v = dot(RayDirection, qvec) * inv_det;
			if (TriangleCoord_v < 0.0f || TriangleCoord_u + TriangleCoord_v > 1.0f)
				return false;

			//! at the end we are sure ray intersect triangle so compute intersection distance
			IntersectionDistance = dot(edge2, qvec) * inv_det;

			if (IntersectionDistance >= 0.0f)
			{
				triangle_normal = cross(edge1, edge2);
				normalize(triangle_normal);
				return true;
			}
			return false;
		}

		/*!	test intersection between a Ray (origin and direction) and a Plane (origin and normal)
		return intersection distance
		*/
		inline bool IntersectRayPlane(const p3f& rOrigin, const v3f& rVector,
			const p3f& pOrigin, const v3f& pNormal,
			double& IntersectionDistance)
		{

			float d = -dot(pNormal, pOrigin);
			float numer = dot(pNormal, rOrigin) + d;
			float denom = dot(pNormal, rVector);

			if (denom == 0.0f)  // normal is orthogonal to vector, cant intersect
				return false;

			double dist = -(numer / denom);
			if (dist < 0 || dist >= IntersectionDistance)
				return false;

			IntersectionDistance = dist;
			return true;
		}


		/*!	test intersection between a Sphere (center, radius) and a BBOX (min and max points)
		just return true or false
		*/
		inline bool IntersectionSphereBBox(const p3f& SphereCenter, float SphereRadius,
			const p3f& BBoxMin, const p3f& BBoxMax)
		{
			float dmin = 0.0f;
			if (SphereCenter.x < BBoxMin.x)
			{
				dmin += (SphereCenter.x - BBoxMin.x) * (SphereCenter.x - BBoxMin.x);
			}
			else
			{
				if (SphereCenter.x > BBoxMax.x) dmin += (SphereCenter.x - BBoxMax.x) * (SphereCenter.x - BBoxMax.x);
			}

			if (SphereCenter.y < BBoxMin.y)
			{
				dmin += (SphereCenter.y - BBoxMin.y) * (SphereCenter.y - BBoxMin.y);
			}
			else
			{
				if (SphereCenter.y > BBoxMax.y) dmin += (SphereCenter.y - BBoxMax.y) * (SphereCenter.y - BBoxMax.y);
			}

			if (SphereCenter.z < BBoxMin.z)
			{
				dmin += (SphereCenter.z - BBoxMin.z) * (SphereCenter.z - BBoxMin.z);
			}
			else
			{
				if (SphereCenter.z > BBoxMax.z) dmin += (SphereCenter.z - BBoxMax.z) * (SphereCenter.z - BBoxMax.z);
			}


			return (dmin <= SphereRadius * SphereRadius);

		}


		/*!	return true if two AABBox intersect
		*/
		inline bool IntersectionAABBAABB(const p3f& BMin1, const p3f& BMax1, const p3f& BMin2, const p3f& BMax2)
		{
			if (BMin1.x > BMax2.x) return false;
			if (BMin2.x > BMax1.x) return false;
			if (BMin1.y > BMax2.y) return false;
			if (BMin2.y > BMax1.y) return false;
			if (BMin1.z > BMax2.z) return false;
			if (BMin2.z > BMax1.z) return false;
			return true;
		}

		/*!	test intersection between a ray (origin, direction) and a cylinder with axis on world Z axis (radius)
		return both intersections distance
		*/
		inline bool IntersectionRayCylinder(const p3f& RayOrigin,
			const v3f& RayDirection,
			const float CylinderRadius,
			float& FirstIntersectionDistance,
			float& SecondIntersectionDistance)
		{
			//! project ray on (X,Y) plane and test intersection with a circle centered on origin 
			float a = RayDirection.x * RayDirection.x + RayDirection.y * RayDirection.y;
			float b = RayOrigin.x * RayDirection.x + RayOrigin.y * RayDirection.y;
			float c = RayOrigin.x * RayOrigin.x + RayOrigin.y * RayOrigin.y - CylinderRadius * CylinderRadius;

			float d = b * b - a * c;

			if (d < 0.0f) return false;

			d = sqrtf(d);
			FirstIntersectionDistance = (-b - d) / a;
			SecondIntersectionDistance = (-b + d) / a;

			if (FirstIntersectionDistance > SecondIntersectionDistance)
			{
				a = FirstIntersectionDistance;
				FirstIntersectionDistance = SecondIntersectionDistance;
				SecondIntersectionDistance = a;
			}

			return true;

		}


		/*!   \brief return true if ray intersect sphere
		*/
		inline bool TestIntersectionRaySphere(const p3f& RayOrigin,
			const v3f& RayDirection,
			const p3f& SphereCenter,
			const float SphereRadius)
		{
			v3f Q = SphereCenter - RayOrigin;
			float c = length2(Q);
			float v = dot(Q, RayDirection);
			float d = SphereRadius * SphereRadius - (c - v * v);
			// If there is no intersection, return false
			if (d < 0.0f) return false;
			else return true;
		}


	}
}