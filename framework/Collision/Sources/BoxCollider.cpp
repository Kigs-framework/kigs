#include "PrecompiledHeaders.h"
#include "BoxCollider.h"
#include "Collision.h"

#include "TecLibs/Math/Algorithm.h"

IMPLEMENT_CLASS_INFO(BoxCollider)

IMPLEMENT_CONSTRUCTOR(BoxCollider)
{
}

void BoxCollider::InitModifiable()
{
	ParentClassType::InitModifiable();
	mBoundingBox.Update(v3f(mOffset) - (v3f)mSize / 2);
	mBoundingBox.Update(v3f(mOffset) + (v3f)mSize / 2);

	mOffset.changeNotificationLevel(Owner);
	mSize.changeNotificationLevel(Owner);
}

void BoxCollider::NotifyUpdate(const u32 labelid)
{
	mBoundingBox = BBox::PreInit{};
	mBoundingBox.Update(v3f(mOffset) - (v3f)mSize / 2);
	mBoundingBox.Update(v3f(mOffset) + (v3f)mSize / 2);
}

namespace
{
	v3f normalAt(const BBox& bbox,const v3f& point)
	{
		v3f normal;
		v3f localPoint = point - bbox.Center();
		auto size = bbox.Size();
		float min = std::numeric_limits<float>::max();
		float distance = fabsf(size.x - fabsf(localPoint.x));
		if (distance < min)
		{
			min = distance;
			normal.Set(1, 0, 0);
			normal *= sign(localPoint.x);
		}
		distance = fabsf(size.y - fabsf(localPoint.y));
		if (distance < min)
		{
			min = distance;
			normal.Set(0, 1, 0);
			normal *= sign(localPoint.y);
		}
		distance = fabsf(size.z - fabsf(localPoint.z));
		if (distance < min)
		{
			min = distance;
			normal.Set(0, 0, 1);
			normal *= sign(localPoint.z);
		}
		return normal;
	}
}

bool BoxCollider::TestHit(Hit& hit, v3f local_origin, v3f local_direction)
{
	return Intersection::IntersectionRayBBox(local_origin, local_direction, mBoundingBox.m_Min, mBoundingBox.m_Max, hit.HitPosition, hit.HitNormal, hit.HitDistance);
}

#ifdef KIGS_TOOLS
#include <GLSLDebugDraw.h>
void BoxCollider::DrawDebug(const Point3D& pos, const  Matrix3x4* mat, Timer *timer)
{
	dd::local_bbox(*mat, mBoundingBox, mDebugColor);
}
#endif

