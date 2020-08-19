#ifndef _BCylinder_H_
#define _BCylinder_H_

#include "CollisionBaseNode.h"

// ****************************************
// * BCylinder class
// * --------------------------------------
/**
* \file	BCylinder.h
* \class	BCylinder
* \ingroup Collision
* \brief Bounding cylinder, for collision calculation purpose.
*/
// ****************************************
class BCylinder : public CollisionBaseNode
{
public:
	DECLARE_CLASS_INFO(BCylinder, CollisionBaseNode, Collision)

	/*! \brief constructor
	*/
	DECLARE_CONSTRUCTOR(BCylinder)

	float GetRadius() const { return mRadius.const_ref(); }
	float GetHeight() const { return mHeight.const_ref(); }
	void GetAxle(Point3D &Axle);

protected:
	void	InitModifiable() override;

	void	GetNodeBoundingBox(Point3D& pmin, Point3D& pmax) const override { pmin.Set(mBoundingBox.m_Min); pmax.Set(mBoundingBox.m_Max); }

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

	//! \brief cylinder radius
	maFloat mRadius;
	//! \brief cylinder height
	maFloat mHeight;
	//! \brief cylinder alignement axis
	maEnum<3> mAxle;

#ifdef KIGS_TOOLS
	void	Update(const Timer&  timer, void* addParam)override;
	// draw debug mInfo using GLSLDrawDebug
	virtual void DrawDebug(const Point3D& pos, const  Matrix3x4* mat, Timer *timer);
#endif
};

#endif //_BCylinder_H_