#include "SimpleShapeMeshCollider.h"

SimpleShapeBase* SimpleShapeBase::createFromDesc(CoreItemSP desc)
{

	SimpleShapeBase*	result=nullptr;

	switch ((int)desc["Type"])
	{
	case (int)SimpleShapeType::ST_AABBox:
		result =new AABBoxSimpleShape();
		break;
	case (int)SimpleShapeType::ST_AACylinder:
		result = new AACylinderSimpleShape();
		break;
	}

	if (result)
	{
		result->initFromCoreItemDesc(desc);
	}

	return result;
}

CoreItemSP	AABBoxSimpleShape::getCoreItemDesc() const
{
	CoreItemSP map = MakeCoreMap();
	map->set("Type", (int)SimpleShapeType::ST_AABBox);

	CoreItemSP desc = MakeCoreMap();
	desc->set("BBoxMin", mBBox.m_Min);
	desc->set("BBoxMax", mBBox.m_Max);

	map->set("Desc", desc);
	return map;
}

void AABBoxSimpleShape::initFromCoreItemDesc(CoreItemSP init)
{
	CoreItemSP desc = init["Desc"];
	if (desc)
	{
		mBBox.m_Min = (v3f)desc["BBoxMin"];
		mBBox.m_Max = (v3f)desc["BBoxMax"];
	}
}

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

bool AABBoxSimpleShape::CallLocalRayIntersection(Hit& hit, const Point3D& start, const Vector3D& dir)  const
{
	return Intersection::IntersectionRayBBox(start, dir, mBBox.m_Min, mBBox.m_Max, hit.HitPosition, hit.HitNormal, hit.HitDistance);
}
bool AABBoxSimpleShape::CallLocalRayIntersection(std::vector<Hit>& hit, const Point3D& start, const Vector3D& dir)  const
{
	Hit	tst;
	hit.clear();

	if (CallLocalRayIntersection(tst, start, dir))
	{
		hit.push_back(tst);
		return true;
	}

	return false;
}


CoreItemSP	AACylinderSimpleShape::getCoreItemDesc() const
{
	CoreItemSP map = MakeCoreMap();
	map->set("Type", (int)SimpleShapeType::ST_AACylinder);

	CoreItemSP desc = MakeCoreMap();
	desc->set("AxisPos1", mAxis[0]);
	desc->set("AxisPos2", mAxis[1]);
	desc->set("Radius", mRadius);

	map->set("Desc", desc);
	return map;
}
void AACylinderSimpleShape::initFromCoreItemDesc(CoreItemSP init)
{
	CoreItemSP desc = init["Desc"];
	if (desc)
	{
		mAxis[0] = (v3f)desc["AxisPos1"];
		mAxis[1] = (v3f)desc["AxisPos2"];
		mRadius = desc["Radius"];
	}
}

bool AACylinderSimpleShape::CallLocalRayIntersection(Hit& hit, const Point3D& start, const Vector3D& dir)  const
{

	/*Capped cylinder
		live example
		// cylinder defined by extremes pa and pb, and radious ra
		vec4 cylIntersect(in vec3 ro, in vec3 rd, in vec3 pa, in vec3 pb, float ra)
	{*/
		v3f ca(mAxis[1]- mAxis[0]);
		v3f oc(start - mAxis[0]);
		float caca = Dot(ca, ca);
		float card = Dot(ca, dir);
		float caoc = Dot(ca, oc);
		float a = caca - card * card;
		float b = caca * Dot(oc, dir) - caoc * card;
		float c = caca * Dot(oc, oc) - caoc * caoc - mRadius * mRadius * caca;
		float h = b * b - a * c;
		if (h < 0.0) return false; //no intersection
		h = sqrt(h);
		float t = (-b - h) / a;
		// body
		float y = caoc + t * card;
		if (y > 0.0 && y < caca)
		{
			hit.HitDistance = t;
			hit.HitNormal = (oc + t * dir - ca * y / caca) / mRadius;
			hit.HitPosition = start + t * dir;
			return true;
		}
		// caps
		t = (((y < 0.0) ? 0.0 : caca) - caoc) / card;
		if (abs(b + a * t) < h)
		{
			hit.HitDistance = t;
			hit.HitNormal = ca * (y<0.0f?-1.0f:1.0f) / caca;
			hit.HitPosition = start + t * dir;
			return true;
		}
		return false; //no intersection
	//}
}
bool AACylinderSimpleShape::CallLocalRayIntersection(std::vector<Hit>& hit, const Point3D& start, const Vector3D& dir)  const
{
	Hit	tst;
	hit.clear();

	if (CallLocalRayIntersection(tst, start, dir))
	{
		hit.push_back(tst);
		return true;
	}

	return false;
}
