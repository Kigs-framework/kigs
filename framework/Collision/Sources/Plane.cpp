#include "PrecompiledHeaders.h"
#include "Plane.h"
#include <stdlib.h>

using namespace Kigs::Collide;

IMPLEMENT_CLASS_INFO(Plane)
IMPLEMENT_CLASS_INFO(Panel)


IMPLEMENT_CONSTRUCTOR(Plane)
, mPosition(*this, false, "Position", 0, 0, 0)
, mNormal(*this, false, "Normal", 0, 0, 1)
{
	mIsDynamic = false;
}

Plane::~Plane()
{
}

void Plane::InitModifiable()
{
	CollisionBaseNode::InitModifiable();
	mBoundingBox.Update(Point3D(-1000, -1000, -1000));
	mBoundingBox.Update(Point3D(1000, 1000, 1000));
}

bool Plane::TestHit(Hit& hit, v3f local_origin, v3f local_direction)
{
	if (IntersectRayPlane(local_origin, local_direction, (v3f)mPosition, (v3f)mNormal, hit.HitDistance))
	{
		hit.HitNormal = -(v3f)mNormal;
		return true;
	}
	return false;
}


#ifdef KIGS_TOOLS
#include "GLSLDebugDraw.h"
#include "Timer.h"
void Plane::DrawDebug(const Hit& h, const Matrix3x4& mat)
{
	Vector3D n(mNormal[0], mNormal[1], mNormal[2]);
	
	auto t = Time::TimePoint::clock::now();
	auto d = t.time_since_epoch().count() / 1'000'000'000.0;
	d = d - (s64)d;

	// draw center
	Point3D localPos(0, 0, 0);
	mat.TransformPoint(&localPos);
	dd::plane(localPos, n, mDebugColor, Vector3D(abs(n.x), abs(n.y), abs(n.z)) * 255, 10, 1);

	// draw hit
	dd::circle(h.HitPosition, n, mDebugColor, 2 * d, 50);
}

void Panel::DrawDebug(const Hit& h, const Matrix3x4& mat)
{
	Point3D p[5];

	auto t = Time::TimePoint::clock::now();
	auto d = t.time_since_epoch().count() / 1'000'000'000.0;
	d = d - (s64)d;

	Vector3D n(mNormal[0], mNormal[1], mNormal[2]);
	Vector3D up(mUp[0], mUp[1], mUp[2]);
	Vector3D left = up^Vector3D(mNormal[0], mNormal[1], mNormal[2]);

	up.Normalize();
	left.Normalize();

	left *= mSize[0] / 2;
	up *= mSize[1] / 2;

	p[0] = mPosition;
	p[1] = mPosition - left - up;
	p[2] = mPosition - left + up;
	p[3] = mPosition + left + up;
	p[4] = mPosition + left - up;
	mat.TransformPoints(p, 5);

	// draw quad
	dd::line(p[1], p[2], mDebugColor);
	dd::line(p[2], p[3], mDebugColor);
	dd::line(p[3], p[4], mDebugColor);
	dd::line(p[4], p[1], mDebugColor);

	dd::line(p[1], p[3], mDebugColor);
	dd::line(p[2], p[4], mDebugColor);

	// draw up
	up.Normalize();
	mat.TransformVector(&up); up.Normalize();
	dd::line(p[0], p[0] + up*0.1f, Vector3D(abs(up.x), abs(up.y), abs(up.z)) * 255);

	// draw normal
	mat.TransformVector(&n); n.Normalize();
	dd::line(p[0], p[0] + n*0.1f, Vector3D(abs(n.x), abs(n.y), abs(n.z)) * 255);

	// draw hit
	dd::circle(h.HitPosition, n, mDebugColor, 2 * d, 50);
}
#endif

IMPLEMENT_CONSTRUCTOR(Panel)
, mSize(*this, false, "Size", 0, 0)
, mHitPos(*this, false, "HitPos", -1, -1)
, mUp(*this, false, "Up", 0, 1, 0)
{
}

void Panel::InitModifiable()
{
	CollisionBaseNode::InitModifiable();

	Vector3D left = (Vector3D)(mUp) ^ (Vector3D)(mNormal);

	Point3D p[2];
	p[0].Set(-mSize[0] * 0.5f, -mSize[1] * 0.5f, 0);
	p[1].Set(mSize[0] * 0.5f, mSize[1] * 0.5f, 0);

	Matrix3x3 mat;
	mat.Set(left, (Vector3D)mUp, (Vector3D)mNormal);
	mat.TransformPoints(p, 2);

	mBoundingBox.Update(p[0]);
	mBoundingBox.Update(p[1]);

}


Point2D Panel::ConvertHit(const Vector3D& hitPos)
{
	Vector3D up(mUp[0], mUp[1], mUp[2]);
	Vector3D left = up^Vector3D(mNormal[0], mNormal[1], mNormal[2]);

	return Point2D((Dot(left, hitPos) / mSize[0]) + 0.5f, (Dot(up, hitPos) / mSize[1]) + 0.5f);
}

bool Panel::GetHit(float &X, float& Y)
{
	X = mHitPos[0];
	Y = mHitPos[1];

	return mHit;
}

bool Panel::ValidHit(const Point3D& pos)
{
	Vector3D up(mUp[0], mUp[1], mUp[2]);
	Vector3D left = up^Vector3D(mNormal[0], mNormal[1], mNormal[2]);

	mHitPos[0] = ((Dot(left, pos - mPosition) / mSize[0]) + 0.5f);
	mHitPos[1] = ((Dot(up, pos - mPosition) / mSize[1]) + 0.5f);

	if (mHitPos[0] > 1.0f ||
		mHitPos[1] > 1.0f ||
		mHitPos[0] < 0.0f ||
		mHitPos[1] < 0.0f)
		return false;

	return true;
}

bool Panel::TestHit(Hit& hit, v3f local_origin, v3f local_direction)
{
	if (ParentClassType::TestHit(hit, local_origin, local_direction))
		return ValidHit(local_origin + local_direction * hit.HitDistance);
	return false;
}
