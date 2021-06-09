#include "SimpleShapeMeshCollider.h"

std::shared_ptr<SimpleShapeBase> SimpleShapeBase::createFromDesc(CoreItemSP desc)
{
	return nullptr;
}

CoreItemSP	AABBoxSimpleShape::getCoreItemDesc() const
{
	return nullptr;
}

bool AABBoxSimpleShape::CallLocalRayIntersection(Hit& hit, const Point3D& start, const Vector3D& dir)  const
{
	return false;
}
bool AABBoxSimpleShape::CallLocalRayIntersection(std::vector<Hit>& hit, const Point3D& start, const Vector3D& dir)  const
{
	return false;
}


CoreItemSP	AACylinderSimpleShape::getCoreItemDesc() const
{
	return nullptr;
}


bool AACylinderSimpleShape::CallLocalRayIntersection(Hit& hit, const Point3D& start, const Vector3D& dir)  const
{
	return false;
}
bool AACylinderSimpleShape::CallLocalRayIntersection(std::vector<Hit>& hit, const Point3D& start, const Vector3D& dir)  const
{
	return false;
}
