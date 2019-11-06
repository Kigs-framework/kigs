// **********************************************************************
// * FILE  : Vector3D.h
// * GROUP : TecLibs/3D
// *---------------------------------------------------------------------
// * PURPOSE : Class for 3D Vector
// * COMMENT : Avoid using it unless you really need the * with Matrix3x4 to NOT apply a translation
// *---------------------------------------------------------------------
// **********************************************************************
#ifndef _Vector3D_h_
#define _Vector3D_h_

// ----------------------------------------------------------------------
// **************************************************
// * Vector3D
// *-------------------------------------------------
// * - Definition of 3D vector
// * - 
// **************************************************

struct Vector3D : Point3D
{
	Vector3D() = default;
	using Point3D::Point3D;
	inline Vector3D(const Point3D& pt);
};

#endif //_Vector3D_h_


