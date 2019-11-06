#include "PrecompiledHeaders.h"
#include "Plane.h"
#include <stdlib.h>

IMPLEMENT_CLASS_INFO(Plane)
IMPLEMENT_CLASS_INFO(Panel)


IMPLEMENT_CONSTRUCTOR(Plane)
, myPosition(*this, false, LABEL_AND_ID(Position), 0, 0, 0)
, myNormal(*this, false, LABEL_AND_ID(Normal), 0, 0, 1)
{
	myIsDynamic = false;
}

Plane::~Plane()
{
}

void Plane::InitModifiable()
{
	CollisionBaseNode::InitModifiable();
	myBoundingBox.Update(Point3D(-1000, -1000, -1000));
	myBoundingBox.Update(Point3D(1000, 1000, 1000));
}

bool Plane::TestHit(Hit& hit, v3f local_origin, v3f local_direction)
{
	if (Intersection::IntersectRayPlane(local_origin, local_direction, (v3f)myPosition, (v3f)myNormal, hit.HitDistance))
	{
		hit.HitNormal = -(v3f)myNormal;
		return true;
	}
	return false;
}


#ifdef KIGS_TOOLS
#include <GLSLDebugDraw.h>
#include <Timer.h>
void Plane::DrawDebug(const Point3D& pos, const  Matrix3x4* mat, Timer *timer)
{
	Vector3D n(myNormal[0], myNormal[1], myNormal[2]);
	double d = timer->GetTime();
	while (d > 1) d -= 1;

	// draw center
	Point3D localPos(0, 0, 0);
	mat->TransformPoint(&localPos);
	dd::plane(localPos, n, debugColor, Vector3D(abs(n.x), abs(n.y), abs(n.z)) * 255, 10, 1);

	// draw hit
	dd::circle(pos, n, debugColor, 2 * d, 50);
}

void Panel::DrawDebug(const Point3D& pos, const  Matrix3x4* mat, Timer *timer)
{
	Point3D p[5];

	double d = timer->GetTime();
	while (d > 1) d -= 1;

	Vector3D n(myNormal[0], myNormal[1], myNormal[2]);
	Vector3D up(myUp[0], myUp[1], myUp[2]);
	Vector3D left = up^Vector3D(myNormal[0], myNormal[1], myNormal[2]);

	up.Normalize();
	left.Normalize();

	left *= mySize[0] / 2;
	up *= mySize[1] / 2;

	p[0] = myPosition;
	p[1] = myPosition - left - up;
	p[2] = myPosition - left + up;
	p[3] = myPosition + left + up;
	p[4] = myPosition + left - up;
	mat->TransformPoints(p, 5);


	// draw quad
	dd::line(p[1], p[2], debugColor);
	dd::line(p[2], p[3], debugColor);
	dd::line(p[3], p[4], debugColor);
	dd::line(p[4], p[1], debugColor);

	dd::line(p[1], p[3], debugColor);
	dd::line(p[2], p[4], debugColor);

	// draw up
	up.Normalize();
	mat->TransformVector(&up); up.Normalize();
	dd::line(p[0], p[0] + up*0.1f, Vector3D(abs(up.x), abs(up.y), abs(up.z)) * 255);

	// draw normal
	mat->TransformVector(&n); n.Normalize();
	dd::line(p[0], p[0] + n*0.1f, Vector3D(abs(n.x), abs(n.y), abs(n.z)) * 255);

	// draw hit
	dd::circle(pos, n, debugColor, 2 * d, 50);
}
#endif

IMPLEMENT_CONSTRUCTOR(Panel)
, mySize(*this, false, LABEL_AND_ID(Size), 0, 0)
, myHitPos(*this, false, LABEL_AND_ID(HitPos), -1, -1)
, myUp(*this, false, LABEL_AND_ID(Up), 0, 1, 0)
{
}

void Panel::InitModifiable()
{
	CollisionBaseNode::InitModifiable();

	Vector3D left = (Vector3D)(myUp) ^ (Vector3D)(myNormal);

	Point3D p[2];
	p[0].Set(-mySize[0] * 0.5f, -mySize[1] * 0.5f, 0);
	p[1].Set(mySize[0] * 0.5f, mySize[1] * 0.5f, 0);

	Matrix3x3 mat;
	mat.Set(left, (Vector3D)myUp, (Vector3D)myNormal);
	mat.TransformPoints(p, 2);

	myBoundingBox.Update(p[0]);
	myBoundingBox.Update(p[1]);

}


Point2D Panel::ConvertHit(const Vector3D& hitPos)
{
	Vector3D up(myUp[0], myUp[1], myUp[2]);
	Vector3D left = up^Vector3D(myNormal[0], myNormal[1], myNormal[2]);

	return Point2D((Dot(left, hitPos) / mySize[0]) + 0.5f, (Dot(up, hitPos) / mySize[1]) + 0.5f);
}

bool Panel::GetHit(float &X, float& Y)
{
	X = myHitPos[0];
	Y = myHitPos[1];

	return myHit;
}

bool Panel::ValidHit(const Point3D& pos)
{
	Vector3D up(myUp[0], myUp[1], myUp[2]);
	Vector3D left = up^Vector3D(myNormal[0], myNormal[1], myNormal[2]);

	myHitPos[0] = ((Dot(left, pos - myPosition) / mySize[0]) + 0.5f);
	myHitPos[1] = ((Dot(up, pos - myPosition) / mySize[1]) + 0.5f);

	if (myHitPos[0] > 1.0f ||
		myHitPos[1] > 1.0f ||
		myHitPos[0] < 0.0f ||
		myHitPos[1] < 0.0f)
		return false;

	return true;
}

bool Panel::TestHit(Hit& hit, v3f local_origin, v3f local_direction)
{
	if (ParentClassType::TestHit(hit, local_origin, local_direction))
		return ValidHit(local_origin + local_direction * hit.HitDistance);
	return false;
}
