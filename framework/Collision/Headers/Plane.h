#ifndef _PLANE_H_
#define _PLANE_H_

#include "TecLibs/2D/BBox2D.h"

#include "CollisionBaseNode.h"

// ****************************************
// * Plane class
// * --------------------------------------
/*!  \class Plane
     a node in a Axis Aligned Bounding box tree, used to optimise intersection calculation 
	 \ingroup Collision
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
	
	void SetIsDynamic(bool b)	{myIsDynamic = b;}
	bool IsDynamic() { return myIsDynamic; }

	bool TestHit(Hit& hit, v3f local_origin, v3f local_direction) override;

	void SetPlane(Point3D &pos, Vector3D &norm)
	{
		myPosition[0] = pos.x;
		myPosition[1] = pos.y;
		myPosition[2] = pos.z;

		myNormal.setValue(norm);
	}
	
	void GetPlane(Point3D &pos, Vector3D &norm)
	{
		pos.x = myPosition[0];
		pos.y = myPosition[1];
		pos.z = myPosition[2];

		norm.x = myNormal[0];
		norm.y = myNormal[1];
		norm.z = myNormal[2];
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

	bool		myIsDynamic;

	maVect3DF	myPosition;
	maVect3DF	myNormal;
};


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
	

	maVect2DF mySize;
	maVect2DF myHitPos;
	maVect3DF myUp;

};


#endif