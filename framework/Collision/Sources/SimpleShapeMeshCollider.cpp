#include "SimpleShapeMeshCollider.h"

#include "AABBTree.h"

#ifdef KIGS_TOOLS
#include "GLSLDebugDraw.h"
#endif

SimpleShapeBase* SimpleShapeBase::createFromDesc(CoreItemSP desc)
{

	SimpleShapeBase*	result=nullptr;

	switch ((int)desc["Type"])
	{
	case (int)SimpleShapeType::ST_AABBox:
		result = new AABBoxSimpleShape();
		break;
	case (int)SimpleShapeType::ST_AACylinder:
		result = new AACylinderSimpleShape();
		break;
	case (int)SimpleShapeType::ST_Compound:
		result = new CompoundShape();
		break;
	case (int)SimpleShapeType::ST_AABBTree:
		result = new AABBTree();
		break;
	case (int)SimpleShapeType::ST_Transform:
		result = new TransformShape();
		break;
	}

	if (result)
	{
		result->initFromCoreItemDesc(desc); 
		auto face_index = desc["FaceIndex"];
		if (face_index)
		{
			result->mFaceIndex = face_index;
		}
	}
	return result;
}




CoreItemSP	CompoundShape::getCoreItemDesc() const
{
	CoreItemSP map = getBaseCoreItemDesc();
	CoreItemSP vec = MakeCoreVector();
	for (auto& shape : mShapes)
	{
		vec->set("", shape->getCoreItemDesc());
	}
	map->set("Shapes", vec);
	return map;
}
void CompoundShape::initFromCoreItemDesc(CoreItemSP init)
{
	CoreItemSP vec = init["Shapes"];
	if (vec)
	{
		for (auto shape : vec)
		{
			mShapes.push_back(SP<SimpleShapeBase>(createFromDesc(shape)));
		}
	}
}
CompoundShape::~CompoundShape()
{
}
bool CompoundShape::CallLocalRayIntersection(Hit& hit, const Point3D& start, const Vector3D& dir) const
{
	bool one_hit = false;
	for (auto& shape : mShapes)
	{
		Hit hit_shape;
		if (shape->CallLocalRayIntersection(hit_shape, start, dir))
		{
			if (hit_shape.HitDistance < hit.HitDistance)
			{
				hit = hit_shape;
				one_hit = true;
			}
		}
	}
	return one_hit;
}
bool CompoundShape::CallLocalRayIntersection(std::vector<Hit>& hit, const Point3D& start, const Vector3D& dir) const
{
	bool one_hit = false;
	for (auto& shape : mShapes)
	{
		if (shape->CallLocalRayIntersection(hit, start, dir))
		{
			one_hit = true;
		}
	}
	return one_hit;
}
#ifdef KIGS_TOOLS
void CompoundShape::DrawDebug(const Hit& h, const Matrix3x4& mat)
{
	for (auto ss : mShapes)
	{
		if(ss->getType() == SimpleShapeType::ST_Compound || ss->getType() == SimpleShapeType::ST_Transform || ss->mFaceIndex == h.HitFaceIndex)
			ss->DrawDebug(h, mat);
	}
}
#endif






CoreItemSP	TransformShape::getCoreItemDesc() const
{
	CoreItemSP map = getBaseCoreItemDesc();
	map->set("Shape", mShape->getCoreItemDesc());
	map->set("TransformX", mTransform.XAxis);
	map->set("TransformY", mTransform.YAxis);
	map->set("TransformZ", mTransform.ZAxis);
	map->set("TransformPos", mTransform.Pos);
	return map;
}
void TransformShape::initFromCoreItemDesc(CoreItemSP init)
{
	CoreItemSP shape = init["Shape"];
	mShape = SP<SimpleShapeBase>(SimpleShapeBase::createFromDesc(shape));

	CoreItemSP TransformX = init["TransformX"];
	CoreItemSP TransformY = init["TransformY"];
	CoreItemSP TransformZ = init["TransformZ"];
	CoreItemSP TransformPos = init["TransformPos"];

	mTransform.XAxis = (v3f)TransformX;
	mTransform.YAxis = (v3f)TransformY;
	mTransform.ZAxis = (v3f)TransformZ;
	mTransform.Pos = (v3f)TransformPos;

	mInvertTransform = Inv(mTransform);
}
TransformShape::~TransformShape()
{
}
bool TransformShape::CallLocalRayIntersection(Hit& hit, const Point3D& start, const Vector3D& dir) const
{
	if (!mShape) return false;

	auto local_start = mInvertTransform * start;
	auto local_dir = mInvertTransform * dir;
	local_dir.Normalize();

	if (mShape->CallLocalRayIntersection(hit, local_start, local_dir))
	{
		hit.HitNormal = mTransform * hit.HitNormal;
		hit.HitDistance = Norm(mTransform * Vector3D(hit.HitDistance * local_dir));
		hit.HitPosition = start + dir * hit.HitDistance;
		return true;
	}
	return false;
}
bool TransformShape::CallLocalRayIntersection(std::vector<Hit>& hit, const Point3D& start, const Vector3D& dir) const
{
	if (!mShape) return false;
	int count_before = hit.size();
	
	auto local_start = mInvertTransform * start;
	auto local_dir = mInvertTransform * dir;
	local_dir.Normalize();

	auto result = mShape->CallLocalRayIntersection(hit, local_start, local_dir);

	for (int i = count_before; i < hit.size(); ++i)
	{
		auto& h = hit[i];
		h.HitNormal = mTransform * h.HitNormal;
		h.HitDistance = Norm(mTransform * Vector3D(h.HitDistance * local_dir));
		h.HitPosition = start + dir * h.HitDistance;
	}

	return result;
}
#ifdef KIGS_TOOLS
void TransformShape::DrawDebug(const Hit& h, const Matrix3x4& mat)
{
	if (mShape && mShape->mFaceIndex == h.HitFaceIndex)
	{
		mat3x4 new_mat = mat * mTransform;
		mShape->DrawDebug(h, new_mat);
	}
}
#endif


CoreItemSP	AABBoxSimpleShape::getCoreItemDesc() const
{
	CoreItemSP map = getBaseCoreItemDesc();
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
	if (Intersection::IntersectionRayBBox(start, dir, mBBox.m_Min, mBBox.m_Max, hit.HitPosition, hit.HitNormal, hit.HitDistance))
	{
		hit.HitFaceIndex = mFaceIndex;
		return true;
	}
	return false;
}
bool AABBoxSimpleShape::CallLocalRayIntersection(std::vector<Hit>& hit, const Point3D& start, const Vector3D& dir)  const
{
	Hit	tst;
	if (CallLocalRayIntersection(tst, start, dir))
	{
		hit.push_back(tst);
		return true;
	}

	return false;
}
#ifdef KIGS_TOOLS
void AABBoxSimpleShape::DrawDebug(const Hit& h, const Matrix3x4& mat)
{
	dd::local_bbox(mat, mBBox, mDebugColor, 0, false);
}
#endif



CoreItemSP	AACylinderSimpleShape::getCoreItemDesc() const
{
	CoreItemSP map = getBaseCoreItemDesc();

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
			hit.HitFaceIndex = mFaceIndex;
			return true;
		}
		// caps
		t = (((y < 0.0) ? 0.0 : caca) - caoc) / card;
		if (abs(b + a * t) < h)
		{
			hit.HitDistance = t;
			hit.HitNormal = ca * (y<0.0f?-1.0f:1.0f) / caca;
			hit.HitPosition = start + t * dir;
			hit.HitFaceIndex = mFaceIndex;
			return true;
		}
		return false; //no intersection
	//}
}
bool AACylinderSimpleShape::CallLocalRayIntersection(std::vector<Hit>& hit, const Point3D& start, const Vector3D& dir)  const
{
	Hit	tst;
	if (CallLocalRayIntersection(tst, start, dir))
	{
		hit.push_back(tst);
		return true;
	}

	return false;
}
#ifdef KIGS_TOOLS
void AACylinderSimpleShape::DrawDebug(const Hit& h, const Matrix3x4& mat)
{
	auto p1 = mat * mAxis[0];
	auto p2 = mat * mAxis[1];
	auto dir = (p1 - p2).Normalized();
	auto r = Norm(mat * Vector3D(mRadius, 0, 0));
	dd::circle(p1, dir, mDebugColor, r, 12);
	dd::circle(p2, -dir, mDebugColor, r, 12);
}
#endif