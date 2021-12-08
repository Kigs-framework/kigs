#ifndef _COLLISIONBASEOBJECT_H_
#define _COLLISIONBASEOBJECT_H_

#include <TecLibs\Tec3D.h>
#include <TecLibs\Math\IntersectionAlgorithms.h>
#include <vector>

class Timer;
class CoreModifiable;
class AABBTree;
class SpacialMeshBVH;


// ****************************************
// * CollisionBaseObject class
// * --------------------------------------
/**
* \file	CollisionBaseObject.h
* \class	CollisionBaseObject
* \ingroup Collision
* \brief  Base class for collision object (sphere, box, mesh...)
*/
// ****************************************

class CollisionBaseObject
{
public:
	CollisionBaseObject();

	virtual ~CollisionBaseObject() {}

	virtual bool ValidHit(const Point3D& pos) { return true; }

	virtual bool TestHit(Hit& hit, v3f local_origin, v3f local_direction) { return false; }

	virtual bool CallLocalRayIntersection(Hit &hit, const Point3D& start, const Vector3D& dir)  const  =0;
	virtual bool CallLocalRayIntersection(std::vector<Hit> &hit, const Point3D& start, const Vector3D& dir)  const =0;

#ifdef KIGS_TOOLS
	// debug color used for this node
	Point3D mDebugColor;

	// draw debug mInfo using GLSLDrawDebug
	virtual void DrawDebug(const Hit& h, const Matrix3x4& mat) = 0;
#endif

	bool mIsCoreModifiable = false;
	bool mIsActive = true;

	// false static cast for two main collision type
	virtual AABBTree* getAABBTree()
	{
		return nullptr;
	}
	virtual SpacialMeshBVH* getSpacialMeshBVH()
	{
		return nullptr;
	}
};
#endif //_COLLISIONBASEOBJECT_H_