#ifndef _COLLISION_H_
#define _COLLISION_H_

#include "CoreModifiable.h"
#include "TecLibs/Tec3D.h"
#include "TecLibs/3D/3DObject/BBox.h"
#include "KMesh.h"


#include "AABBTree.h"

//#define COUNTCOLLISION
// ****************************************
// * Collision class
// * --------------------------------------
/*!  \class Collision
     encapsulate useful collision calculation methods
	 \ingroup Collision
*/
// ****************************************
class BSphere;
class Plane;
class Collision
{
public:
	/*! \brief compute collision time between two moving spheres
		given 2 spheres radius, position, and velocity, return true if the sphere collide
		or false if not.
		If the sphere collide, return the intersection time
	*/
	static bool CollideSphereSphere(const Point3D &Sphere1Center, 
							   const Vector3D &Sphere1Velocity, 
							   const kfloat Sphere1Radius, 
							   const Point3D &Sphere2Center, 
							   const Vector3D &Sphere2Velocity, 
							   const kfloat Sphere2Radius, 
							   kfloat &IntersectionTime);

	/*! \brief compute collision between a moving sphere and a single triangle
		given the sphere origin, radius and velocity, and 3 points, return if there is 
		a collision, and if yes, give the intersection distance (time?), the intersecting
		point and normal
	*/
	static bool CollideSphereTriangle(const Point3D &SphereOrigin,
							const Vector3D &SphereVelocity,
							const kfloat SphereRadius,
							const Point3D &P1,
							const Point3D &P2,
							const Point3D &P3,
							kdouble &IntersectionDistance,
							Vector3D &InteresectionNormal,Point3D &IntersectionPoint);

	/*! \brief compute collision between a moving sphere and a mesh using an already existing AABBTree 
		given the sphere origin, radius and velocity, the mesh and its AABBtree, return if there is 
		a collision, and if yes, give the intersection distance (time?), the intersecting
		point and normal
	*/
	static bool CollideSphereAABBTreeNode(const Point3D &SphereOrigin,
									const Vector3D &SphereVelocity,
									const kfloat &SphereRadius,
									AABBTreeNode &pAABB, Mesh* pMesh,
									kdouble &IntersectionDistance,
									Vector3D &InteresectionNormal,Point3D &IntersectionPoint);

	/*! \brief compute collision between a ray and a sphere given the ray origin, ray direction in sphere local coords, and sphere radius
		return if there is a collision, and if yes, give the intersection distance (time?), the intersecting
		point and normal
	*/
	static bool CollideRaySphere(
							const Point3D &RayStartingPoint, 
							const Vector3D &Raydirection,
							const kfloat &SphereRadius,
							kdouble &IntersectionDistance,
							Vector3D &IntersectionNormal);

	static bool CollideRayPlane(
		const Point3D &RayStartingPoint,
		const Vector3D &Raydirection,
		const Point3D &PlanePos,
		const Vector3D &PlaneNorm,
		kdouble &IntersectionDistance,
		Vector3D &IntersectionNormal);

	static bool CollideRayPlane(
		const Point3D &RayStartingPoint,
		const Vector3D &Raydirection,
		Plane *Plane,
		kdouble &IntersectionDistance,
		Vector3D &IntersectionNormal);

	static bool CollideRayCylinder(
		const Point3D &RayStartingPoint,
		const Vector3D &RayDirection,
		const Vector3D &CylinderDirection,
		const kfloat &CylinderHeight,
		const kfloat &CylinderRadius,
		kdouble &IntersectionDistance,
		Vector3D &IntersectionNormal);

#ifdef COUNTCOLLISION
static unsigned int	CollisionTestCount;
#endif

protected:
	/*! \brief return true if point is in triangle given by A,B and C
	*/
	static bool CheckPointInTriangle(const Point3D& point, const Point3D& A, const Point3D& B, const Point3D& C); 
	
	/*! \brief return the closest point to P on the line (A,B)
	*/
	static Point3D closestPointOnLine(const Point3D &A , const Point3D& B, const Point3D &P); 
	
	/*! \brief return the closest point to P on the triangle (A,B,C)
	*/
	static Point3D closestPointOnTriangle(const Point3D &A , const Point3D& B, const Point3D& C, const Point3D &P); 

	/*! \brief internal collide computation used by the public CollideSphereAABBTreeNode method
	*/
	static bool CollideSphereAABBTreeNode(const Point3D &SphereOrigin,
									const Vector3D &SphereVelocity,
									const kfloat &SphereRadius,
									AABBTreeNode &pAABB, Mesh* pMesh,
									const BBox& MovingSphereBBox,
									kdouble &IntersectionDistance,
									Vector3D &InteresectionNormal,Point3D &IntersectionPoint);

};

// ****************************************
// * IPlane class
// * --------------------------------------
/*!  \class IPlane
	 \ingroup Collision
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
	void SetFromTriangle(const Point3D &A, const Point3D &B, const Point3D &C)
	{
		Vector3D v1,v2;
		Origin = A;
		v1 = B-A;
		v2 = C-A;
		Normal.CrossProduct(v1, v2);
		Normal.Normalize();
	}
	eSideOfPlane Side(const Point3D &P)
	{
		 Vector3D dir = Origin - P;
		 kfloat d = Dot(dir, Normal);
 
		 if (d<KFLOAT_CONST(-0.001f))
		  return FRONT;	
		 else
		 if (d>KFLOAT_CONST(0.001f))
		  return BACK;	

		return ON;
	}
	Point3D Origin;
	Vector3D Normal;
};

#endif // _COLLISION_H_