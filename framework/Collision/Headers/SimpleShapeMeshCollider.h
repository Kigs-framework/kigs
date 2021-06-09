#pragma once
#include "TecLibs/3D/3DObject/BBox.h"
#include "CollisionBaseObject.h"
#include "CoreModifiable.h"
#include "CoreItem.h"


enum class SimpleShapeType
{
	ST_Uninit = -1,
	ST_AABBox = 0,
	ST_AACylinder = 1,

};

class SimpleShapeBase : public CollisionBaseObject
{
public:

	virtual SimpleShapeType getType() const = 0;
	virtual CoreItemSP	getCoreItemDesc() const = 0;

	static std::shared_ptr<SimpleShapeBase> createFromDesc(CoreItemSP desc);

#ifdef KIGS_TOOLS
	// draw debug mInfo using GLSLDrawDebug
	void DrawDebug(const Point3D& pos, const  Matrix3x4* mat, Timer* timer)
	{
		// not for now
	}
#endif


private:

};

class AABBoxSimpleShape : public SimpleShapeBase
{
public:
	SimpleShapeType getType() const override
	{
		return SimpleShapeType::ST_AABBox;
	}

	template<typename PacketStream>
	bool Serialize(PacketStream& stream)
	{
		SERIALIZE_VERSION(stream, 1);
		CHECK_SERIALIZE(serialize_object(stream, mBBox.m_Min));
		CHECK_SERIALIZE(serialize_object(stream, mBBox.m_Max));
	}

	CoreItemSP	getCoreItemDesc() const override;

	virtual bool CallLocalRayIntersection(Hit& hit, const Point3D& start, const Vector3D& dir)  const override;
	virtual bool CallLocalRayIntersection(std::vector<Hit>& hit, const Point3D& start, const Vector3D& dir)  const override;

	BBox	mBBox;
};

class AACylinderSimpleShape : public SimpleShapeBase
{
public:
	SimpleShapeType getType() const override
	{
		return SimpleShapeType::ST_AACylinder;
	}

	template<typename PacketStream>
	bool Serialize(PacketStream& stream)
	{
		SERIALIZE_VERSION(stream, 1);
		CHECK_SERIALIZE(serialize_object(stream, mAxis[0]));
		CHECK_SERIALIZE(serialize_object(stream, mAxis[1]));
		CHECK_SERIALIZE(serialize_object(stream, mRadius));
	}

	CoreItemSP	getCoreItemDesc() const override;

	virtual bool CallLocalRayIntersection(Hit& hit, const Point3D& start, const Vector3D& dir)  const override;
	virtual bool CallLocalRayIntersection(std::vector<Hit>& hit, const Point3D& start, const Vector3D& dir)  const override;


	v3f			mAxis[2];
	float		mRadius;
};
