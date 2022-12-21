#include "PrecompiledHeaders.h"
#include "BSphere.h"
#include "Collision.h"

IMPLEMENT_CLASS_INFO(BSphere)

IMPLEMENT_CONSTRUCTOR(BSphere)
, mRadius(*this, false, "Radius", 0.0f)
{
}

void BSphere::InitModifiable()
{
	CollisionBaseNode::InitModifiable();

	mBoundingBox.Update(Point3D(-mRadius, -mRadius, -mRadius));
	mBoundingBox.Update(Point3D(mRadius, mRadius, mRadius));
}

bool BSphere::TestHit(Hit& hit, v3f local_origin, v3f local_direction)
{
	//Intersection::IntersectionRaySphere(local_origin, local_direction, v3f(0,0,0), myRadius,)
	return Collision::CollideRaySphere(local_origin, local_direction, mRadius, hit.HitDistance, hit.HitNormal);
}

#ifdef KIGS_TOOLS
#include <GLSLDebugDraw.h>
void BSphere::DrawDebug(const Hit& h, const Matrix3x4& mat)
{
	Point3D p(0,0,0);
	mat.TransformPoint(&p);
	Vector3D r{ 0,0,mRadius };
	mat.TransformVector(&r);
	dd::sphere(p, mDebugColor, Norm(r));
}
#endif

