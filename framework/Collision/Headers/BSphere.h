#ifndef _BSPHERE_H_
#define _BSPHERE_H_

#include "CollisionBaseNode.h"

// ****************************************
// * BSphere class
// * --------------------------------------
/**
* \file	BSphere.h
* \class	BSphere
* \ingroup Collision
* \brief Sphere structure used for collision calculation.
*/
// ****************************************

class BSphere : public CollisionBaseNode
{
public:
	DECLARE_CLASS_INFO(BSphere, CollisionBaseNode, Collision)

	/*! \brief constructor
	*/
	DECLARE_CONSTRUCTOR(BSphere)

	float GetRadius() const { return mRadius.const_ref(); }

	bool TestHit(Hit& hit, v3f local_origin, v3f local_direction) override;

protected:

	virtual bool CallLocalRayIntersection(Hit &hit, const Point3D& start, const Vector3D& dir) const override
	{
		// TODO
		return false;
	}
	virtual bool CallLocalRayIntersection(std::vector<Hit> &hit, const Point3D& start, const Vector3D& dir) const override
	{
		// TODO
		return false;
	}
	void	InitModifiable() override;
	
	/*! \brief the sphere radius
	*/
	maFloat mRadius;

#ifdef KIGS_TOOLS
	// draw debug mInfo using GLSLDrawDebug
	virtual void DrawDebug(const Point3D& pos, const  Matrix3x4* mat, Timer *timer);
#endif
};

#endif //_BSPHERE_H_