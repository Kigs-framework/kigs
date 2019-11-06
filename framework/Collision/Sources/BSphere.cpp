#include "PrecompiledHeaders.h"
#include "BSphere.h"
#include "Collision.h"

IMPLEMENT_CLASS_INFO(BSphere)

IMPLEMENT_CONSTRUCTOR(BSphere)
, myRadius(*this, false, LABEL_AND_ID(Radius), KFLOAT_CONST(0.0))
{
}

void BSphere::InitModifiable()
{
	CollisionBaseNode::InitModifiable();

	myBoundingBox.Update(Point3D(-myRadius, -myRadius, -myRadius));
	myBoundingBox.Update(Point3D(myRadius, myRadius, myRadius));
}

bool BSphere::TestHit(Hit& hit, v3f local_origin, v3f local_direction)
{
	//Intersection::IntersectionRaySphere(local_origin, local_direction, v3f(0,0,0), myRadius,)
	return Collision::CollideRaySphere(local_origin, local_direction, myRadius, hit.HitDistance, hit.HitNormal);
}

#ifdef KIGS_TOOLS
#include <GLSLDebugDraw.h>
void BSphere::DrawDebug(const Point3D& pos, const  Matrix3x4* mat, Timer *timer)
{
	Point3D p(0,0,0);
	mat->TransformPoint(&p);
	Vector3D r{ 0,0,myRadius };
	mat->TransformVector(&r);
	dd::sphere(p, debugColor, Norm(r));
}
#endif

