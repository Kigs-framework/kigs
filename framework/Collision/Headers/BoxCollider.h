#pragma once

#include "CollisionBaseNode.h"

// ****************************************
// * BoxCollider class
// * --------------------------------------
/**
* \file	BoxCollider.h
* \class	BoxCollider
* \ingroup Collision
* \brief Box collision object.
*/
// ****************************************

class BoxCollider : public CollisionBaseNode
{
public:
	DECLARE_CLASS_INFO(BoxCollider, CollisionBaseNode, Collision)
	DECLARE_CONSTRUCTOR(BoxCollider)

	bool TestHit(Hit& hit, v3f local_origin, v3f local_direction) override;

protected:
	void InitModifiable() override;
	void NotifyUpdate(const u32 labelid) override;

	maVect3DF mSize = BASE_ATTRIBUTE(Size, 1, 1, 1);
	maVect3DF mOffset = BASE_ATTRIBUTE(Offset, 0, 0, 0);

	virtual bool CallLocalRayIntersection(Hit &hit, const Point3D& start, const Vector3D& dir)  const override
	{
		// TODO
		return false;
	}
	virtual bool CallLocalRayIntersection(std::vector<Hit> &hit, const Point3D& start, const Vector3D& dir) const override
	{
		// TODO
		return false;
	}

#ifdef KIGS_TOOLS
	void DrawDebug(const Hit& h, const Matrix3x4& mat) override;
#endif
};
