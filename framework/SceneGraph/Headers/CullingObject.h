#ifndef _CULLINGOBJECT_H_
#define _CULLINGOBJECT_H_

#include "CoreModifiable.h"
#include "TecLibs/Tec3D.h"

class Node3D;

/*! \defgroup InternalCulling Culling Internal
 * \ingroup SceneGraph
*/

// ****************************************
// * CullingObject class
// * --------------------------------------
/**
 * \file	CullingObject.h
 * \class	CullingObject
 * \ingroup SceneGraph
 * \brief	A culling object is a list of planes (each defined by a point and a normal vector) used to determine if an object is inside or outside the culling object.
 * \author	ukn
 * \version ukn
 * \date	ukn
 */
// ****************************************
class CullingObject : public CoreModifiable
{
public:
	//! a culling object is a CoreModifiable
	DECLARE_CLASS_INFO(CullingObject,CoreModifiable,SceneGraph)
	DECLARE_INLINE_CONSTRUCTOR(CullingObject) {}

	//! an object can be totally outside, totally inside or partially inside
	enum  CULLING_RESULT
	{
	all_out=1,
	all_in=2,
	partially_in=3
	};

	//! "classic" culling planes when culling object is created by a camera  
	enum CULLING_PLANE
	{
		plane_near=0,
		plane_far,
		plane_bottom,
		plane_top,
		plane_left,
		plane_right
	};

	//! test the given node 
	virtual CULLING_RESULT Cull(Node3D*,unsigned int& cullingMask);

	template<bool isScaled>
	CULLING_RESULT SubCull(Node3D*, unsigned int& cullingMask);

	// ****************************************
	// * CullPlane class
	// * --------------------------------------
	/**
	 * \file	CullPlane.h
	 * \class	CullPlane
	 * \ingroup InternalCulling
	 * \brief structure to store a plane definition : a Origin Point3D and a Normal Vector3D	
	 * \author	ukn
	 * \version ukn
	 * \date	ukn
	 */
	// ****************************************
	class CullPlane
	{
	public:
		//! normal
		Vector3D  myNormal;
		//! position
		Point3D   myOrigin;

		//! fourth term of plane equation
		float	myD;
	};

	/**
	 * \brief	return current plane count in this object
	 * \fn 		int GetPlaneCount()
	 * \return	name : the current plane count in this object
	 */
	int GetPlaneCount(){return (int)myCullPlaneList.size();}

	/**
	 * \brief	initialise plane with index i in the list with given origin and normal
	 * \fn 		void  InitPlane(int i,const Vector3D&, const Point3D&);
	 * \param	i : index
	 * \param	Vector3D : normal
	 * \param	Point3D : position
	 * \return	the current plane count in this object
	 */
	void  InitPlane(int i,const Vector3D&, const Point3D&);

	/**
	 * \brief	insert a new plane in the liste defined with given origin and normal
	 * \fn 		void  AddPlane(const Vector3D&, const Point3D&);
	 * \param	Vector3D : normal
	 * \param	Point3D : position
	 */
	void  AddPlane(const Vector3D&, const Point3D&);

	/**
	 * \brief	remove plane with index i from the list
	 * \fn 		void  RemovePlane(int i);
	 * \param	i : index
	 */
	void  RemovePlane(int i);

	/**
	 * \brief	return the plane list 
	 * \fn 		const kstl::vector<CullPlane*>& GetCullPlaneList()
	 * \return	the plane list 
	 */
	std::vector<CullPlane>& GetCullPlaneList(){return myCullPlaneList;}

	// return intersection point of p1,p2,p3
	static Point3D	getIntersection(const CullPlane& p1, const CullPlane& p2, const CullPlane& p3);

protected:
	
	//! the vector used to store the list of plane
	kstl::vector<CullPlane>   myCullPlaneList;
};

#endif //_CULLINGOBJECT_H_
