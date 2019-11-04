#ifndef _BSPHERE_H_
#define _BSPHERE_H_

#include "CollisionBaseNode.h"

// ****************************************
// * BSphere class
// * --------------------------------------
/*!  \class BSphere
	 sphere used for collision calculation
	 \ingroup Collision
*/
// ****************************************

class BSphere : public CollisionBaseNode
{
public:
	DECLARE_CLASS_INFO(BSphere, CollisionBaseNode, Collision)

	/*! \brief constructor
	*/
	DECLARE_CONSTRUCTOR(BSphere)

	float GetRadius() const { return myRadius.const_ref(); }

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
	maFloat myRadius;

#ifdef KIGS_TOOLS
	// draw debug info using GLSLDrawDebug
	virtual void DrawDebug(const Point3D& pos, const  Matrix3x4* mat, Timer *timer);
#endif
};

#endif //_BSPHERE_H_