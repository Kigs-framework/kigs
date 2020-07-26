#include "PrecompiledHeaders.h"
#include "Collision.h"
#include "Intersection.h"
#include "BSphere.h"
#include "Plane.h"

int gCollisionDrawLevel=0;
#ifdef KIGS_TOOLS
#include<GLSLDebugDraw.h>
extern const Matrix3x4 * currentNodeMatrix;
#endif

#ifdef COUNTCOLLISION
unsigned int	Collision::CollisionTestCount;
#endif
/*!
	check if a collision occurs between two moving spheres
	Two usages are possibles :
		- giving each sphere center/radius and velocity in meter per seconds
		  -> just check for collision in the next second, return intersection time between 0 and 1
		- giving each sphere center/radius and movement vector (end position = center + movement vector)
		  -> check collision between start and end position, collision position is sphereXCenter + returned IntersectionTime * SphereXVelocity
*/
bool Collision::CollideSphereSphere(const Point3D &Sphere1Center,
	const Vector3D &Sphere1Velocity,
	const kfloat Sphere1Radius,
	const Point3D &Sphere2Center,
	const Vector3D &Sphere2Velocity,
	const kfloat Sphere2Radius,
	kfloat &IntersectionTime)
{
	//! first compute sphere1 to sphere 2 vector
	Vector3D diffC = Sphere2Center - Sphere1Center;
	//! then compute sphere2 relative velocity to sphere1   
	Vector3D diffV = Sphere2Velocity - Sphere1Velocity;
	//! distance equation between sphere1 and sphere2 
	kfloat a = Dot(diffV, diffV);
	kfloat b = Dot(diffC, diffV);
	kfloat c = Sphere1Radius + Sphere2Radius;
	c = Dot(diffC, diffC) - c*c;
	kfloat delta = b*b - a*c;
	if (delta < KFLOAT_ZERO) return false;
	delta = sqrtf(delta);

	IntersectionTime = (-b - delta) / a;
	//! return true if collision occur in the next second (or between start and end position)
	if (IntersectionTime >= KFLOAT_CONST(0.0f) && IntersectionTime <= KFLOAT_CONST(1.0f)) return true;

	return false;
}


/*!
	check if a point (Point, P) is inside a triangle defined by points (A,B,C)
	the given point is in the triangle plane.
	Use the angles sum : APB + BPC + CPA. If sum is near 2*PI then the point is in the triangle else
	it is out.
	This method use 3 vector normalize, 3 acos, 3 dot product and one abs, perhaps other faster method is possible
*/
/*
bool Collision::CheckPointInTriangle(const Point3D& point, const Point3D& A, const Point3D& B, const Point3D& C)
{
	Vector3D	AB(A,B);
	Vector3D	BC(B,C);
	Vector3D	CA(C,A);

	// first get cross ABxAC to get reference vector

	Vector3D	referenceV;
	referenceV.CrossProduct(AB,-CA);

	Vector3D	cross1;
	cross1.CrossProduct(AB,Vector3D(A,point));

	kfloat sign1=Dot(cross1,referenceV);

	cross1.CrossProduct(BC,Vector3D(B,point));

	kfloat sign2=Dot(cross1,referenceV);

	cross1.CrossProduct(CA,Vector3D(C,point));

	kfloat sign3=Dot(cross1,referenceV);

	if((sign1<=KFLOAT_ZERO)&&(sign2<=KFLOAT_ZERO)&&(sign3<=KFLOAT_ZERO))
	{
		return true;
	}

	if((sign1>=KFLOAT_ZERO)&&(sign2>=KFLOAT_ZERO)&&(sign3>=KFLOAT_ZERO))
	{
		return true;
	}

  return(false);
}*/
bool Collision::CheckPointInTriangle(const Point3D& point, const Point3D& A, const Point3D& B, const Point3D& C)
{
	// Compute vectors        
	Vector3D v0(A, C, asVector{});
	Vector3D v1(A, B, asVector{});
	Vector3D v2(A, point, asVector{});

	// Compute dot products
	kfloat dot00 = Dot(v0, v0);
	kfloat dot01 = Dot(v0, v1);
	kfloat dot02 = Dot(v0, v2);
	kfloat dot11 = Dot(v1, v1);
	kfloat dot12 = Dot(v1, v2);

	// Compute barycentric coordinates
	//invDenom = 1 / (dot00 * dot11 - dot01 * dot01)
	kfloat u = (dot11 * dot02 - dot01 * dot12)/* * invDenom*/;
	kfloat v = (dot00 * dot12 - dot01 * dot02)/* * invDenom*/;

	kfloat denom = (dot00 * dot11 - dot01 * dot01);
	// Check if point is in triangle
	return (u >= KFLOAT_ZERO) && (v >= KFLOAT_ZERO) && ((u + v) <= denom);

}

/*!
	search for the lowest positive root of the a*t*t + b*t + c = 0 function
	return true if one positive root is found, t parameter is used to return the founded root
*/
bool LowestPositiveRoot_SimplifiedQuadratic(const kfloat &a, const kfloat &b, const kfloat &c, kdouble &t)
{
	kfloat delta = b*b - a*c;
	if (delta >= KFLOAT_CONST(0.0f))
	{
		delta = sqrtf(delta);
		t = (-b - delta) / a;
		if (t >= KFLOAT_CONST(0.0f))
		{
			kfloat t2 = (-b + delta) / a;
			if (t2 >= KFLOAT_CONST(0.0f) && t2 < t) t = t2;
			return true;
		}
		else
		{
			t = (-b + delta) / a;
			return t >= KFLOAT_CONST(0.0f);
		}
	}
	return false;
}

/*!
	check collision between a moving sphere (position, radius, movement vector) and a triangle (P1,P2,P3).
	<b>the method return false if the sphere is already intersecting the triangle</b>
*/

bool Collision::CollideSphereTriangle(const Point3D &SphereOrigin,
	const Vector3D &SphereVelocity,
	const kfloat SphereRadius,
	const Point3D &P1,
	const Point3D &P2,
	const Point3D &P3,
	kdouble &IntersectionTime,
	Vector3D &Normal,
	Point3D &IntersectionPoint)
{
#ifdef COUNTCOLLISION
	CollisionTestCount++;
#endif
	kfloat t0, t1;
	kfloat a;

	//Normal.CrossProduct(Vector3D(P1,P2),Vector3D(P1,P3));
//	Normal.Normalize();	
	//! compute the "normal velocity", ie the velocity of the sphere on the plane's normal axis
	kfloat nv = Dot(Normal, SphereVelocity);

	//! first return false if sphere is moving far away from the triangle (nv>KFLOAT_CONST(0.0)) 
	if (nv > KFLOAT_CONST(0.0f))
	{
		return false;		// moving away
	}

	//! compute distance from sphere origin to plane 
	kfloat dnAO = Dot(Normal, Vector3D(P1, SphereOrigin, asVector{}));	// distance to plane
	if (nv == KFLOAT_CONST(0.0f))	// mouvement dans le plan du triangle
	{
		return false;
	}
	//! compute intersection start/end times  
	t0 = (SphereRadius - dnAO) / nv;
	t1 = (-SphereRadius - dnAO) / nv;

	// swap so t0 is min, t1 is max
	if (t0 > t1)
	{
		kfloat temp = t1;
		t1 = t0;
		t0 = temp;
	}

	/*! if collision has already occured or there's no collision between sphere start/end pos
		then return false
	*/
	if (t0 > KFLOAT_CONST(1.0f) || t1 < KFLOAT_CONST(0.0f))
	{
		return false;
	}

	if (t0 < KFLOAT_CONST(0.0)) t0 = KFLOAT_CONST(0.0);
	if (t1 < KFLOAT_CONST(0.0)) t1 = KFLOAT_CONST(0.0);
	if (t0 > KFLOAT_CONST(1.0)) t0 = KFLOAT_CONST(1.0);
	if (t1 > KFLOAT_CONST(1.0)) t1 = KFLOAT_CONST(1.0);


	IntersectionTime = t0;

	Point3D PointOnPlane;

	//! compute projected intersection point on plane
	PointOnPlane.x = SphereOrigin.x - Normal.x*SphereRadius + t0*SphereVelocity.x;
	PointOnPlane.y = SphereOrigin.y - Normal.y*SphereRadius + t0*SphereVelocity.y;
	PointOnPlane.z = SphereOrigin.z - Normal.z*SphereRadius + t0*SphereVelocity.z;

	//! and return true if it is inside the triangle 
	if (CheckPointInTriangle(PointOnPlane, P1, P2, P3))
	{
		IntersectionPoint = PointOnPlane;
		return true;
	}

	// TODO : redo everything here

	/*! if intersection occured with the plane but outside the triangle, compute intersection with
		nearest of P1, P2 or P3
	*/
	IntersectionTime = 1000.0;
	bool Found = false;
	kdouble t;
	Vector3D P1O(P1, SphereOrigin, asVector{});
	Vector3D P2O(P2, SphereOrigin, asVector{});
	Vector3D P3O(P3, SphereOrigin, asVector{});
	kfloat SquareRadius = SphereRadius*SphereRadius;
	kfloat SquareVelocity = NormSquare(SphereVelocity);

	kfloat b = Dot(P1O, SphereVelocity);
	kfloat c = NormSquare(P1O) - SquareRadius;
	if (LowestPositiveRoot_SimplifiedQuadratic(SquareVelocity, b, c, IntersectionTime))
	{
		IntersectionPoint = P1;
		Found = true;
	}
	b = Dot(P2O, SphereVelocity);
	c = NormSquare(P2O) - SquareRadius;
	if (LowestPositiveRoot_SimplifiedQuadratic(SquareVelocity, b, c, t))
	{
		if (t < IntersectionTime)
		{
			IntersectionTime = t;
			IntersectionPoint = P2;
			Found = true;
		}
	}
	b = Dot(P3O, SphereVelocity);
	c = NormSquare(P3O) - SquareRadius;
	if (LowestPositiveRoot_SimplifiedQuadratic(SquareVelocity, b, c, t))
	{
		if (t < IntersectionTime)
		{
			IntersectionTime = t;
			IntersectionPoint = P3;
			Found = true;
		}
	}

	/*! then compute intersection with
		nearest edge (P1,P2), (P2,P3), (P3,P1)
	*/
	Vector3D e(P1, P2, asVector{});
	kfloat SquareEdge = NormSquare(e);
	kfloat ev = Dot(e, SphereVelocity);
	kfloat ePO = Dot(e, P1O);
	a = SquareEdge*-SquareVelocity + ev*ev;
	b = -SquareEdge*Dot(P1O, SphereVelocity) + ev*ePO;
	c = SquareEdge*(SquareRadius - NormSquare(P1O)) + ePO*ePO;
	if (LowestPositiveRoot_SimplifiedQuadratic(a, b, c, t))
	{
		if (t < IntersectionTime)
		{
			kfloat f0 = (ev*(kfloat)t + Dot(e, P1O)) / SquareEdge;
			if (f0 >= KFLOAT_CONST(0.0f) && f0 <= KFLOAT_CONST(1.0f))
			{
				IntersectionTime = t;
				IntersectionPoint = P1 + (Point3D)e*f0;
				Found = true;
			}
		}
	}

	e = P3 - P2;
	SquareEdge = NormSquare(e);
	ev = Dot(e, SphereVelocity);
	ePO = Dot(e, P2O);
	a = SquareEdge*-SquareVelocity + ev*ev;
	b = -SquareEdge*Dot(P2O, SphereVelocity) + ev*ePO;
	c = SquareEdge*(SquareRadius - NormSquare(P2O)) + ePO*ePO;
	if (LowestPositiveRoot_SimplifiedQuadratic(a, b, c, t))
	{
		if (t < IntersectionTime)
		{
			kfloat f0 = (ev*(kfloat)t + Dot(e, P2O)) / SquareEdge;
			if (f0 >= KFLOAT_CONST(0.0f) && f0 <= KFLOAT_CONST(1.0f))
			{
				IntersectionTime = t;
				IntersectionPoint = P2 + (Point3D)e*f0;
				Found = true;
			}
		}
	}

	e = P1 - P3;
	SquareEdge = NormSquare(e);
	ev = Dot(e, SphereVelocity);
	ePO = Dot(e, P3O);
	a = SquareEdge*-SquareVelocity + ev*ev;
	b = -SquareEdge*Dot(P3O, SphereVelocity) + ev*ePO;
	c = SquareEdge*(SquareRadius - NormSquare(P3O)) + ePO*ePO;
	if (LowestPositiveRoot_SimplifiedQuadratic(a, b, c, t))
	{
		if (t < IntersectionTime)
		{
			kfloat f0 = (ev*(kfloat)t + Dot(e, P3O)) / SquareEdge;
			if (f0 >= KFLOAT_CONST(0.0f) && f0 <= KFLOAT_CONST(1.0f))
			{
				IntersectionTime = t;
				IntersectionPoint = P3 + (Point3D)e*f0;
				Found = true;
			}
		}
	}

	/*! finally keep the nearest intersection with point or edge if any, and return
	*/
	if (Found)
	{
		Normal = (SphereOrigin + (Point3D)SphereVelocity*(kfloat)IntersectionTime) - IntersectionPoint;
		Normal.Normalize();
		return true;
	}

	return false;
}

/*!
	check collision between a moving sphere (position, radius, movement vector) and mesh given its AABBTree.

*/
bool Collision::CollideSphereAABBTreeNode(
	const Point3D &SphereOrigin,
	const Vector3D &SphereVelocity,
	const kfloat &SphereRadius,
	AABBTreeNode &pAABB, Mesh* pMesh,
	kdouble &IntersectionDistance,
	Vector3D &IntersectionNormal, Point3D &IntersectionPoint)
{



	//! first compute the bounding box for the sphere along its trajectory
	BBox	MovingSphereBBox;

	if (SphereVelocity.x >= KFLOAT_CONST(0.0f))
	{
		MovingSphereBBox.m_Max.x = SphereOrigin.x + SphereVelocity.x + SphereRadius;
		MovingSphereBBox.m_Min.x = SphereOrigin.x - SphereRadius;
	}
	else
	{
		MovingSphereBBox.m_Max.x = SphereOrigin.x + SphereRadius;
		MovingSphereBBox.m_Min.x = SphereOrigin.x + SphereVelocity.x - SphereRadius;
	}

	if (SphereVelocity.y >= KFLOAT_CONST(0.0f))
	{
		MovingSphereBBox.m_Max.y = SphereOrigin.y + SphereVelocity.y + SphereRadius;
		MovingSphereBBox.m_Min.y = SphereOrigin.y - SphereRadius;
	}
	else
	{
		MovingSphereBBox.m_Max.y = SphereOrigin.y + SphereRadius;
		MovingSphereBBox.m_Min.y = SphereOrigin.y + SphereVelocity.y - SphereRadius;
	}
	if (SphereVelocity.z >= KFLOAT_CONST(0.0f))
	{
		MovingSphereBBox.m_Max.z = SphereOrigin.z + SphereVelocity.z + SphereRadius;
		MovingSphereBBox.m_Min.z = SphereOrigin.z - SphereRadius;
	}
	else
	{
		MovingSphereBBox.m_Max.z = SphereOrigin.z + SphereRadius;
		MovingSphereBBox.m_Min.z = SphereOrigin.z + SphereVelocity.z - SphereRadius;
	}


	//! then call the same method but with the moving sphere BBOx param
	return CollideSphereAABBTreeNode(SphereOrigin, SphereVelocity, SphereRadius, pAABB, pMesh, MovingSphereBBox,
		IntersectionDistance, IntersectionNormal, IntersectionPoint);
}

/*!
	check collision between a moving sphere (position, radius, movement vector, bbox along trajectory) and mesh given its AABBTree.
	this method is recursive
*/
bool Collision::CollideSphereAABBTreeNode(const Point3D &SphereOrigin,
	const Vector3D &SphereVelocity,
	const kfloat &SphereRadius,
	AABBTreeNode &pAABB, Mesh* pMesh,
	const BBox& MovingSphereBBox,
	kdouble &IntersectionDistance,
	Vector3D &IntersectionNormal, Point3D &IntersectionPoint)
{
	//kfloat LocalDistance;
	kdouble TempDist = IntersectionDistance;
	Vector3D TempNormal;
	Point3D  tempPoint;

	//! first test current node bounding box against moving sphere bounding box
	if (Intersection::IntersectionAABBAABB(pAABB.m_BBox.m_Min, pAABB.m_BBox.m_Max, MovingSphereBBox.m_Min, MovingSphereBBox.m_Max))
	{
		//! if the bounding box are intersecting
		if (pAABB.Son1 == NULL && pAABB.Son2 == NULL)
		{
			//! if this node is a leaf then check each triangles and return 
			bool found = false;
			/*for (int i = 0; i < pAABB.TriangleCount; i++)
			{
				TempNormal = pAABB.TriangleArray[i].m_normal;
				if (CollideSphereTriangle(SphereOrigin, SphereVelocity, SphereRadius,
					pMesh->VertexArray[pAABB.TriangleArray[i].m_Original->a],
					pMesh->VertexArray[pAABB.TriangleArray[i].m_Original->b],
					pMesh->VertexArray[pAABB.TriangleArray[i].m_Original->c],
					TempDist, TempNormal, tempPoint))
				{
					if (TempDist < IntersectionDistance)
					{
						found = true;
						IntersectionDistance = TempDist;
						IntersectionNormal = TempNormal;
						IntersectionPoint = tempPoint;
					}
				}
			}*/
			return found;
		}

		//! if this node is not a leaf then recurse to sons
		if (CollideSphereAABBTreeNode(SphereOrigin, SphereVelocity, SphereRadius, *(pAABB.Son1), pMesh, MovingSphereBBox, TempDist, TempNormal, tempPoint))
		{
			if (TempDist < IntersectionDistance)
			{
				IntersectionDistance = TempDist;
				IntersectionNormal = TempNormal;
				IntersectionPoint = tempPoint;
			}
			if (CollideSphereAABBTreeNode(SphereOrigin, SphereVelocity, SphereRadius, *(pAABB.Son2), pMesh, MovingSphereBBox, TempDist, TempNormal, tempPoint))
			{
				if (TempDist < IntersectionDistance)
				{
					IntersectionDistance = TempDist;
					IntersectionNormal = TempNormal;
					IntersectionPoint = tempPoint;
				}
			}
			return true;
		}
		else
		{
			return CollideSphereAABBTreeNode(SphereOrigin, SphereVelocity, SphereRadius, *(pAABB.Son2), pMesh, MovingSphereBBox, IntersectionDistance, IntersectionNormal, IntersectionPoint);
		}
	}

	return false;
}

bool Collision::CollideRayCylinder(
	const Point3D &RayStartingPoint,
	const Vector3D &RayDirection,
	const Vector3D &CylinderDirection,
	const kfloat &CylinderHeight,
	const kfloat &CylinderRadius,
	kdouble &IntersectionDistance,
	Vector3D &IntersectionNormal)
{
	/*auto dist3D_Line_to_Line =[](Point3D p0, Vector3D v0, Point3D p1, Vector3D v1, Point3D & out0, Point3D & out1)
	{
		//Vector   u = L1.P1 - L1.P0;
		//Vector   v = L2.P1 - L2.P0;
		Vector3D  w = p0 - p1;
		float    a = Dot(v0, v0);         // always >= 0
		float    b = Dot(v0, v1);
		float    c = Dot(v1, v1);         // always >= 0
		float    d = Dot(v0, w);
		float    e = Dot(v1, w);
		float    D = a*c - b*b;        // always >= 0
		float    sc, tc;

		// compute the line parameters of the two closest points
		if (D < SMALL_NUM) {          // the lines are almost parallel
			sc = 0.0;
			tc = (b>c ? d / b : e / c);    // use the largest denominator
		}
		else {
			sc = (b*e - c*d) / D;
			tc = (a*e - b*d) / D;
		}

		out0 = sc * v0;
		out1 = tc * v1;
		out1 = - w + out1;

		// get the difference of the two closest points
		//Vector3D   dP = w + out0 - out1;  // =  L1(sc) - L2(tc)
		Vector3D   dP = out0 - out1;  // =  L1(sc) - L2(tc)

		return Norm(dP);   // return the closest distance
	};


	// Initialize the result as if there is no intersection.  If we discover
	// an intersection, these values will be modified accordingly.
	double dist = IntersectionDistance;
	Vector3D N(0,0,0);
	unsigned int numIntersections = 0;

	// Create a coordinate system for the cylinder.  In this system, the
	// cylinder segment center C is the origin and the cylinder axis direction
	// W is the z-axis.  U and V are the other coordinate axis directions.
	// If P = x*U+y*V+z*W, the cylinder is x^2 + y^2 = r^2, where r is the
	// cylinder radius.  The end caps are |z| = h/2, where h is the cylinder
	// height.
	float halfHeight = (0.5f) * CylinderHeight;
	float rSqr = CylinderRadius * CylinderRadius;


	// Convert incoming line origin to capsule coordinates.
	Point3D P{ Dot(CylinderDirection, RayStartingPoint), 
		Dot(Point3D(CylinderDirection.y,CylinderDirection.z,CylinderDirection.x), RayStartingPoint), 
		Dot(Point3D(CylinderDirection.z,CylinderDirection.x,CylinderDirection.y), RayStartingPoint) };

	// Get the z-value, in cylinder coordinates, of the incoming line's
	// unit-length direction.
	float dz = Dot(CylinderDirection, RayDirection);

	// The line is parallel to the cylinder axis. 
	// Determine whether the line intersects the cylinder end disks.
	if (fabsf(dz) > 0.999f)
	{
		float radialSqrDist = rSqr - P[1] * P[1] - P[2] * P[2];
		if (radialSqrDist >= 0.0001f)
		{
			dist = fabsf(P.x) - halfHeight;
			N = (dz > 0.0f) ?-CylinderDirection: CylinderDirection;
		}
		// else:  The line is outside the cylinder, no intersection.
		numIntersections = 2;
	}
	// The line is NOT parallel to the cylinder axis.
	else
	{
		// find the shortest direction
		Vector3D shortestDir = CylinderDirection^RayDirection;
		//Vector3D shortestPlaneDir = CylinderDirection^shortestDir;
		//kigsprintf("%0.3f %0.3f %0.3f\n", shortestDir.x, shortestDir.y, shortestDir.z);
		
		Point3D ori(0, 0, 0), out0, out1;
		float xx = dist3D_Line_to_Line(ori, CylinderDirection, RayStartingPoint, RayDirection, out0, out1);

		dd::sphere(out0, Point3D(0.5, 0, 1), 0.03f);
		if (xx == CylinderRadius) // one single hit (tangent)
		{
			dist = Norm(RayStartingPoint - out1);
			N = out1 - out0;
			N.Normalize();
			numIntersections = 2;

		}
		else if (xx < CylinderRadius)// two hits (arc)
		{
			dd::sphere(out1, Point3D(0.5, 0.0, .5), 0.03f);
		}
		else
		{
			dd::sphere(out1, Point3D(1, 0.0, 1), 0.03f);
			return false;
		}
	}

	


	if (numIntersections < 2)
	{
		// Convert the incoming line unit-length direction to cylinder
		// coordinates.
		Point3D D{
			Dot(Point3D(CylinderDirection.y,CylinderDirection.z,CylinderDirection.x),RayDirection),
			Dot(Point3D(CylinderDirection.z,CylinderDirection.x,CylinderDirection.y),RayDirection),
			dz };

		float a0, a1, a2, discr, root, inv, tValue;

		// The line is perpendicular to the cylinder axis.
		if (dz < 0.0001f && dz > -0.0001f)
		{
			if (std::abs(P[2]) <= halfHeight)
			{
				// Test intersection of line P+t*D with infinite cylinder
				// x^2+y^2 = r^2.  This reduces to computing the roots of a
				// quadratic equation.  If P = (px,py,pz) and D = (dx,dy,dz),
				// then the quadratic equation is
				//   (dx^2+dy^2)*t^2 + 2*(px*dx+py*dy)*t + (px^2+py^2-r^2) = 0

				a0 = P[0] * P[0] + P[1] * P[1] - rSqr;
				a1 = P[0] * D[0] + P[1] * D[1];
				a2 = D[0] * D[0] + D[1] * D[1];
				discr = a1 * a1 - a0 * a2;
				if (discr > 0.0f)
				{
					// The line intersects the cylinder in two places.
					root = sqrt(discr);
					inv = 1.0f / a2;
					dist = (-a1 - root) * inv;
				}
				else if (discr == 0.0f)
				{
					// The line is tangent to the cylinder.
					dist = -a1 / a2;
				}
				numIntersections = 2;
				// else: The line does not intersect the cylinder.
			}
			// else: The line is outside the planes of the cylinder end disks.
		}

		if (numIntersections < 2)
		{
			// Test for intersections with the planes of the end disks.
			inv = 1.0f / D[2];

			float t0 = (-halfHeight - P[2]) * inv;
			float xTmp = P[0] + t0 * D[0];
			float yTmp = P[1] + t0 * D[1];
			if (xTmp * xTmp + yTmp * yTmp <= rSqr)
			{
				// Plane intersection inside the top cylinder end disk.
				if (dist > t0)
					dist = t0;
				numIntersections++;
			}

			float t1 = (+halfHeight - P[2]) * inv;
			xTmp = P[0] + t1 * D[0];
			yTmp = P[1] + t1 * D[1];
			if (xTmp * xTmp + yTmp * yTmp <= rSqr)
			{
				// Plane intersection inside the bottom cylinder end disk.
				if (dist > t1)
					dist = t1;
				numIntersections++;
			}

			if (numIntersections < 2)
			{
				// Test for intersection with the cylinder wall.
				a0 = P[0] * P[0] + P[1] * P[1] - rSqr;
				a1 = P[0] * D[0] + P[1] * D[1];
				a2 = D[0] * D[0] + D[1] * D[1];
				discr = a1 * a1 - a0 * a2;
				if (discr > 0.0f)
				{
					root = sqrt(discr);
					inv = 1.0f / a2;
					tValue = (-a1 - root) * inv;
					if (t0 <= t1)
					{
						if (t0 <= tValue && tValue <= t1)
						{
							if (dist > tValue)
								dist = tValue;
							numIntersections++;
						}
					}
					else
					{
						if (t1 <= tValue && tValue <= t0)
						{
							if (dist > tValue)
								dist = tValue;
							numIntersections++;
						}
					}

					if (numIntersections < 2)
					{
						tValue = (-a1 + root) * inv;
						if (t0 <= t1)
						{
							if (t0 <= tValue && tValue <= t1)
							{
								if (dist > tValue)
									dist = tValue;
								numIntersections++;
							}
						}
						else
						{
							if (t1 <= tValue && tValue <= t0)
							{
								if (dist > tValue)
									dist = tValue;
								numIntersections++;
							}
						}
					}
					// else: Line intersects end disk and cylinder wall.
				}
				else if (discr == (float)0)
				{
					tValue = -a1 / a2;
					if (t0 <= t1)
					{
						if (t0 <= tValue && tValue <= t1)
						{
							if (dist > tValue)
								dist = tValue;
							numIntersections++;
						}
					}
					else
					{
						if (t1 <= tValue && tValue <= t0)
						{
							if (dist > tValue)
								dist = tValue;
							numIntersections++;
						}
					}
				}
			}
		}
	}

	if (numIntersections > 0)
	{
		if (dist < IntersectionDistance)
		{
			IntersectionDistance = dist;
			IntersectionNormal = N;
			return true;
		}

		return false;
	}*/
	return false;
}

bool Collision::CollideRaySphere(
	const Point3D &RayStartingPoint,
	const Vector3D &Raydirection,
	const kfloat &SphereRadius,
	kdouble &IntersectionDistance,
	Vector3D &IntersectionNormal)
{
	Vector3D L_AB_Vector = Vector3D(RayStartingPoint);

	if (Norm(L_AB_Vector) <= IntersectionDistance)
	{
		float a = Dot(Raydirection, Raydirection);
		float b = 2 * Dot(Raydirection, RayStartingPoint);
		float c = Dot(RayStartingPoint, RayStartingPoint) - (SphereRadius * SphereRadius);

		//Find discriminant
		float disc = b * b - 4 * a * c;

		// if discriminant is negative there are no float roots, so return 
		// false as ray misses sphere
		if (disc < 0)
			return false;

		// compute q as described above
		float distSqrt = sqrtf(disc);
		float q;
		if (b < 0)
			q = (-b - distSqrt)*0.5f;
		else
			q = (-b + distSqrt)*0.5f;

		// compute t0 and t1
		float t0 = q / a;
		float t1 = c / q;

		// make sure t0 is smaller than t1
		if (t0 > t1)
		{
			// if t0 is bigger than t1 swap them around
			float temp = t0;
			t0 = t1;
			t1 = temp;
		}

		// if t1 is less than zero, the object is in the ray's negative direction
		// and consequently the ray misses the sphere
		if (t1 < 0)
			return false;

		// if t0 is less than zero, the intersection point is at t1
		if (t0 < 0)
		{
#ifdef KIGS_TOOLS
			if (gCollisionDrawLevel > 1)
			{
				Point3D p(0, 0, 0);
				currentNodeMatrix->TransformPoint(&p);
				dd::sphere(p, Vector3D(255, 255, 0), SphereRadius);
			}
#endif
			IntersectionDistance = t1;
			IntersectionNormal = RayStartingPoint + (kfloat)IntersectionDistance*Raydirection;
			return true;
		}
		// else the intersection point is at t0
		else
		{
#ifdef KIGS_TOOLS
			if (gCollisionDrawLevel > 1)
			{
				Point3D p(0, 0, 0);
				currentNodeMatrix->TransformPoint(&p);
				dd::sphere(p, Vector3D(255, 255, 0), SphereRadius);
			}
#endif

			IntersectionDistance = t0;
			IntersectionNormal = RayStartingPoint + (kfloat)IntersectionDistance*Raydirection;
			return true;
		}
	}
	return false;
}


bool Collision::CollideRayPlane(
	const Point3D &RayStartingPoint,
	const Vector3D &Raydirection,
	const Point3D &PlanePos,
	const Vector3D &PlaneNorm,
	kdouble &IntersectionDistance,
	Vector3D &IntersectionNormal)
{ 

	if (Dot(PlaneNorm, Raydirection) == 0) {
		return false; // No intersection, the line is parallel to the plane
	}

   // get d value
	float d = Dot(PlaneNorm, RayStartingPoint);
	// Compute the X value for the directed line ray intersecting the plane
	float x = -(Dot(RayStartingPoint, PlaneNorm)+d) / Dot(Raydirection, PlaneNorm);

	if (x > IntersectionDistance)
		return false;

	// output contact point
	//*contact = RayStartingPoint + normalize(Raydirection)*x; //Make sure your ray vector is normalized
	IntersectionDistance = x;
	IntersectionNormal = PlaneNorm;
	return true;
}

bool Collision::CollideRayPlane(
	const Point3D &RayStartingPoint,
	const Vector3D &Raydirection,
	Plane *plane,
	kdouble &IntersectionDistance,
	Vector3D &IntersectionNormal)
{
	Point3D pos;
	Vector3D norm;
	plane->GetPlane(pos, norm);

	if (Dot(norm, Raydirection) == 0) {
		return false; // No intersection, the line is parallel to the plane
	}

	// get d value
	float d = Dot(norm, RayStartingPoint);
	// Compute the X value for the directed line ray intersecting the plane
	float x = -(Dot(RayStartingPoint, norm) + d) / Dot(Raydirection, norm);

	if (x > IntersectionDistance)
		return false;

	// output contact point
	//*contact = RayStartingPoint + normalize(Raydirection)*x; //Make sure your ray vector is normalized
	IntersectionDistance = x;
	IntersectionNormal = norm;
	return true;
}