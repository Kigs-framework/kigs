#pragma once

#include "CollisionBaseNode.h"

class BoxCollider : public CollisionBaseNode
{
public:
	DECLARE_CLASS_INFO(BoxCollider, CollisionBaseNode, Collision)
	DECLARE_CONSTRUCTOR(BoxCollider)

	bool TestHit(Hit& hit, v3f local_origin, v3f local_direction) override;

protected:
	void	InitModifiable() override;
	maVect3DF mSize = BASE_ATTRIBUTE(Size, 1, 1, 1);

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
	// draw debug info using GLSLDrawDebug
	virtual void DrawDebug(const Point3D& pos, const  Matrix3x4* mat, Timer *timer);
#endif
};
