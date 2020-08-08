#ifndef _COLLISIONBASENODE_H_
#define _COLLISIONBASENODE_H_

#include<Drawable.h>
#include<CollisionBaseObject.h>
#include<maReference.h>
#include<TecLibs/3D/3DObject/BBox.h>

class CollisionBaseNode : public Drawable, public CollisionBaseObject
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(CollisionBaseNode, Drawable, Collision)

	/*! \brief constructor
	*/
	DECLARE_CONSTRUCTOR(CollisionBaseNode);

	bool BBoxUpdate(kdouble time) override { return true; }
	void	GetNodeBoundingBox(Point3D& pmin, Point3D& pmax) const override { pmin = myBoundingBox.m_Min; pmax = myBoundingBox.m_Max; }

protected:
	void	InitModifiable() override;


	//! link to the bounding box
	BBox        myBoundingBox;

	maBool		myHit;
};

#endif //_COLLISIONBASENODE_H_