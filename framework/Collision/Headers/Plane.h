#ifndef _PLANE_H_
#define _PLANE_H_

#include "TecLibs/2D/BBox2D.h"

#include "CollisionBaseNode.h"

// ****************************************
// * Plane class
// * --------------------------------------
/**
* \file	Plane.h
* \class	Plane
* \ingroup Collision
* \brief  Plane structure for collision purpose.
*/
// ****************************************
class Plane : public CollisionBaseNode
{
public:
	DECLARE_CLASS_INFO(Plane, CollisionBaseNode, Collision)

	/*! \brief constructor 
	*/
	DECLARE_CONSTRUCTOR(Plane);

	/*! \brief destructor 
	*/
	virtual ~Plane();

#ifdef KIGS_TOOLS
	// draw debug info using GLSLDrawDebug
	virtual void DrawDebug(const Point3D& pos, const  Matrix3x4* mat, Timer *timer);
#endif
	
	void SetIsDynamic(bool b)	{mIsDynamic = b;}
	bool IsDynamic() { return mIsDynamic; }

	bool TestHit(Hit& hit, v3f local_origin, v3f local_direction) override;

	void SetPlane(Point3D &pos, Vector3D &norm)
	{
		mPosition[0] = pos.x;
		mPosition[1] = pos.y;
		mPosition[2] = pos.z;

		mNormal.setValue(norm);
	}
	
	void GetPlane(Point3D &pos, Vector3D &norm)
	{
		pos.x = mPosition[0];
		pos.y = mPosition[1];
		pos.z = mPosition[2];

		norm.x = mNormal[0];
		norm.y = mNormal[1];
		norm.z = mNormal[2];
	}
	
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

	bool		mIsDynamic;

	maVect3DF	mPosition;
	maVect3DF	mNormal;
};

// ****************************************
// * Panel class
// * --------------------------------------
/**
* \file	Plane.h
* \class	Panel
* \ingroup Collision
* \brief  Rectangular surface on a plane where collision is detected.
*/
// ****************************************
class Panel :public Plane
{
public:
	DECLARE_CLASS_INFO(Panel, Plane, Collision)

	/*! \brief constructor
	*/
	DECLARE_CONSTRUCTOR(Panel);


	Point2D ConvertHit(const Vector3D& hitPos);
	bool GetHit(float &X, float& Y);

#ifdef KIGS_TOOLS
	// draw debug info using GLSLDrawDebug
	virtual void DrawDebug(const Point3D& pos, const  Matrix3x4* mat, Timer *timer);
#endif
		
	bool TestHit(Hit& hit, v3f local_origin, v3f local_direction) override;
	bool ValidHit(const Point3D& pos) override;

protected:
	void InitModifiable() override;
	

	maVect2DF mSize;
	maVect2DF mHitPos;
	maVect3DF mUp;

};


#endif