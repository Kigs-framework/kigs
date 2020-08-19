#ifndef _DISTANCE_H_
#define _DISTANCE_H_

#include "CoreModifiable.h"
#include "TecLibs/Tec3D.h"
#include "TecLibs/3D/3DObject/BBox.h"
#include "KMesh.h"


// ****************************************
// * Distance class
// * --------------------------------------
/**
* \file	Distance.h
* \class	Distance
* \ingroup Collision
* \brief  Encapsulate useful distance calculation methods
*/
// ****************************************
class Distance
{
public:
	/*!   \brief return distance from point to triangle
		  triangle is given by points A,B,C
	*/	  
	static kfloat PointTriangle(const Point3D &P, const Point3D &A, const Point3D &B, const Point3D &C); 

	/*!   \brief return distance from point to mesh
		  the triangle from the mesh, where the distance is shortest is returned 
	*/
	static kfloat PointMesh(const Point3D &P, const Mesh* pMesh, Mesh::Triangle* &TrianglePtr); 

	/*!   \brief return distance from point to axis aligned box
		  the box is defined by a vector (diagonal), and centered on the origin point (0,0,0)
	*/
	static kfloat PointAABBAtOrigin(const Point3D &P, const Vector3D &BoxSize, Point3D &MinDistPoint);

	/*!   \brief return distance from point to axis aligned box
		  the box is defined by min and max points
	*/
	static kfloat PointAABB(const Point3D &P, const Point3D &BMin,const Point3D &BMax, Point3D &MinDistPoint);

	/*!   \brief return distance from point to oriented box
		  the box is defined a size (diagonal vector) and a transform matrix (rotation/scale and position)
	*/
	static kfloat PointOBB(const Point3D &P, const Matrix3x4 &OrientationMatrix, Vector3D &BoxSize, Point3D &MinDistPoint);

	/*!   \brief return distance from point to plane
		  plane is given by point PlaneOrigin and normal PlaneNormal
	*/
	static kfloat PointPlane(const Point3D &P, const Vector3D &PlaneNormal, const Point3D &PlaneOrigin);
};


#endif