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
/*!  \class CollisionBaseObject
	 Base object for collision object (sphere, box, mesh...)
	 \ingroup Collision
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

	virtual CoreModifiable* GetLinkedItem() { return LinkedItem; }
	void SetLinkedItem(CoreModifiable * item) { LinkedItem = item; }
	
#ifdef KIGS_TOOLS
	// debug color used for this node
	Point3D debugColor;

	// draw debug info using GLSLDrawDebug
	virtual void DrawDebug(const Point3D& pos, const Matrix3x4* mat, Timer *timer) = 0;
#endif

	bool IsCoreModifiable = false;
	bool IsActive = true;

	// false static cast for two main collision type
	virtual AABBTree* getAABBTree()
	{
		return nullptr;
	}
	virtual SpacialMeshBVH* getSpacialMeshBVH()
	{
		return nullptr;
	}


protected:
	CoreModifiable* LinkedItem = nullptr;
};
#endif //_COLLISIONBASEOBJECT_H_