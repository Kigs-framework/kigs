#pragma once

#include "CoreModifiable.h"
#include "TecLibs/Tec3D.h"
#include "TecLibs/3D/3DObject/BBox.h"
#include "KMesh.h"


#include "AABBTree.h"

namespace Kigs
{
	namespace Collide
	{
		class BSphere;
		class Plane;


		// ****************************************
		// * Collision class
		// * --------------------------------------
		/**
		* \file	Collision.h
		* \class	Collision
		* \ingroup Collision
		* \brief  Encapsulate useful collision calculation methods.
		*/
		// ****************************************
		class Collision
		{
		public:
			/*! \brief compute collision time between two moving spheres
				given 2 spheres radius, position, and velocity, return true if the sphere collide
				or false if not.
				If the sphere collide, return the intersection time
			*/
			static bool CollideSphereSphere(const v3f& Sphere1Center,
				const Maths::Vector3D& Sphere1Velocity,
				const float Sphere1Radius,
				const v3f& Sphere2Center,
				const Maths::Vector3D& Sphere2Velocity,
				const float Sphere2Radius,
				float& IntersectionTime);

			/*! \brief compute collision between a moving sphere and a single triangle
				given the sphere origin, radius and velocity, and 3 points, return if there is
				a collision, and if yes, give the intersection distance (time?), the intersecting
				point and normal
			*/
			static bool CollideSphereTriangle(const v3f& SphereOrigin,
				const Maths::Vector3D& SphereVelocity,
				const float SphereRadius,
				const v3f& P1,
				const v3f& P2,
				const v3f& P3,
				double& IntersectionDistance,
				Maths::Vector3D& InteresectionNormal, v3f& IntersectionPoint);

			/*! \brief compute collision between a moving sphere and a mesh using an already existing AABBTree
				given the sphere origin, radius and velocity, the mesh and its AABBtree, return if there is
				a collision, and if yes, give the intersection distance (time?), the intersecting
				point and normal
			*/
			static bool CollideSphereAABBTreeNode(const v3f& SphereOrigin,
				const Maths::Vector3D& SphereVelocity,
				const float& SphereRadius,
				AABBTreeNode& pAABB, Draw::Mesh* pMesh,
				double& IntersectionDistance,
				Maths::Vector3D& InteresectionNormal, v3f& IntersectionPoint);

			/*! \brief compute collision between a ray and a sphere given the ray origin, ray direction in sphere local coords, and sphere radius
				return if there is a collision, and if yes, give the intersection distance (time?), the intersecting
				point and normal
			*/
			static bool CollideRaySphere(
				const v3f& RayStartingPoint,
				const Maths::Vector3D& Raydirection,
				const float& SphereRadius,
				double& IntersectionDistance,
				Maths::Vector3D& IntersectionNormal);

			static bool CollideRayPlane(
				const v3f& RayStartingPoint,
				const Maths::Vector3D& Raydirection,
				const v3f& PlanePos,
				const Maths::Vector3D& PlaneNorm,
				double& IntersectionDistance,
				Maths::Vector3D& IntersectionNormal);

			static bool CollideRayPlane(
				const v3f& RayStartingPoint,
				const Maths::Vector3D& Raydirection,
				Plane* Plane,
				double& IntersectionDistance,
				Maths::Vector3D& IntersectionNormal);

			static bool CollideRayCylinder(
				const v3f& RayStartingPoint,
				const Maths::Vector3D& RayDirection,
				const Maths::Vector3D& CylinderDirection,
				const float& CylinderHeight,
				const float& CylinderRadius,
				double& IntersectionDistance,
				Maths::Vector3D& IntersectionNormal);

#ifdef COUNTCOLLISION
			static unsigned int	mCollisionTestCount;
#endif

		protected:
			/*! \brief return true if point is in triangle given by A,B and C
			*/
			static bool CheckPointInTriangle(const v3f& point, const v3f& A, const v3f& B, const v3f& C);

			/*! \brief return the closest point to P on the line (A,B)
			*/
			static v3f closestPointOnLine(const v3f& A, const v3f& B, const v3f& P);

			/*! \brief return the closest point to P on the triangle (A,B,C)
			*/
			static v3f closestPointOnTriangle(const v3f& A, const v3f& B, const v3f& C, const v3f& P);

			/*! \brief internal collide computation used by the public CollideSphereAABBTreeNode method
			*/
			static bool CollideSphereAABBTreeNode(const v3f& SphereOrigin,
				const Maths::Vector3D& SphereVelocity,
				const float& SphereRadius,
				AABBTreeNode& pAABB, Draw::Mesh* pMesh,
				const Maths::BBox& MovingSphereBBox,
				double& IntersectionDistance,
				Maths::Vector3D& InteresectionNormal, v3f& IntersectionPoint);

		};


		// ****************************************
		// * IPlane class
		// * --------------------------------------
		/**
		* \file	Collision.h
		* \class	IPlane
		* \ingroup Collision
		* \brief  Plane structure for collision calculations.
		*/
		// ****************************************

		class IPlane
		{
		public:
			enum eSideOfPlane
			{
				FRONT,
				BACK,
				ON
			} SideOfPlane;
			void SetFromTriangle(const v3f& A, const v3f& B, const v3f& C)
			{
				Maths::Vector3D v1, v2;
				mOrigin = A;
				v1 = B - A;
				v2 = C - A;
				mNormal.CrossProduct(v1, v2);
				mNormal.Normalize();
			}
			eSideOfPlane Side(const v3f& P)
			{
				Maths::Vector3D dir = mOrigin - P;
				float d = Dot(dir, mNormal);

				if (d < -0.001f)
					return FRONT;
				else
					if (d > 0.001f)
						return BACK;

				return ON;
			}
			v3f mOrigin;
			Maths::Vector3D mNormal;
		};

	}
}