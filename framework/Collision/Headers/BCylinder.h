#ifndef _BCylinder_H_
#define _BCylinder_H_

#include "CollisionBaseNode.h"

// ****************************************
// * BCylinder class
// * --------------------------------------
/*!  \class BCylinder
	 cylinder used for collision calculation
	 \ingroup Collision
*/
// ****************************************

class BCylinder : public CollisionBaseNode
{
public:
	DECLARE_CLASS_INFO(BCylinder, CollisionBaseNode, Collision)

	/*! \brief constructor
	*/
	DECLARE_CONSTRUCTOR(BCylinder)

	float GetRadius() const { return myRadius.const_ref(); }
	float GetHeight() const { return myHeight.const_ref(); }
	void GetAxle(Point3D &Axle);

protected:
	void	InitModifiable() override;

	void	GetNodeBoundingBox(Point3D& pmin, Point3D& pmax) const override { pmin.Set(myBoundingBox.m_Min); pmax.Set(myBoundingBox.m_Max); }

	virtual bool CallLocalRayIntersection(Hit &hit, const Point3D& start, const Vector3D& dir)  const override
	{
		// TODO
		return false;
	}
	virtual bool CallLocalRayIntersection(std::vector<Hit> &hit, const Point3D& start, const Vector3D& dir)  const override
	{
		// TODO
		return false;
	}

	/*! \brief the sphere radius
	*/
	maFloat myRadius;
	maFloat myHeight;
	maEnum<3> myAxle;

#ifdef KIGS_TOOLS
	void	Update(const Timer&  timer, void* addParam)override;
	// draw debug info using GLSLDrawDebug
	virtual void DrawDebug(const Point3D& pos, const  Matrix3x4* mat, Timer *timer);
#endif
};

#endif //_BCylinder_H_