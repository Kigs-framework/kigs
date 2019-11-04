// **********************************************************************
// * FILE  : 3D.h
// * GROUP : TecLibs/3D
// *---------------------------------------------------------------------
// * PURPOSE : 3D classes
// * COMMENT : ...
// *---------------------------------------------------------------------
// **********************************************************************
#ifndef _3D_h_
#define _3D_h_



// ----------------------------------------------------------------------
// +---------
// | 3D section
// +---------

// General includes
#include "TecLibs.h"                // To be Tec
#include "TecMath.h"                // To use sqrt, cos, etc.

#ifdef TEC_USE_NAMESPACES
namespace Tec3D {
#endif
	
	// Forward declarations
	struct Vector3D;
	struct Point3D;
	struct Vector4D;
	struct Quaternion;
	
	struct Matrix3x3;
	struct Matrix3x4;
	struct Matrix4x4;
	class Basis3D;
	class Reference3D;
	
	// Classes definitions
	
#include "2D/Point2D.h"         // 
#include "2D/Point2DI.h"         //
	
#include "3D/Point3D.h"             // To use 3D point
#include "3D/Vector3D.h"            // To use 3D vector
#include "3D/Point3DI.h"             // To use 3D point
#include "3D/v3d.h"             	// To use 3D point

#include "3D/Vector4D.h"            // To use 4D vector (projective geometry)
#include "3D/Quaternion.h"          // To use quaternion
	
#include "2D/AffineTransform2D.h"
#include "3D/Matrix3x3.h"           // To use 3x3 matrix
#include "3D/Matrix3x4.h"           // To use 3x4 matrix
#include "3D/Matrix4x4.h"           // To use 4x4 matrix
#include "3D/Basis3D.h"             // To use 3D basis
#include "3D/Reference3D.h"         // To use 3D reference
	
	
	
	// Classes implementations
#include "3D/Vector3D.inl.h"
#include "3D/Point3D.inl.h"
#include "3D/v3d.inl.h"
#include "3D/Point3DI.inl.h"
#include "3D/Vector4D.inl.h"
#include "3D/Quaternion.inl.h"
#include "3D/Matrix3x3.inl.h"
#include "3D/Matrix3x4.inl.h"
#include "3D/Matrix4x4.inl.h"
#include "3D/Basis3D.inl.h"
#include "3D/Reference3D.inl.h"
	
#include "2D/Point2D.inl.h"
#include "2D/Point2DI.inl.h"
#include "2D/AffineTransform2D.inl.h"
	
#ifdef TEC_USE_NAMESPACES
}

typedef Tec3D::Vector3D		TecVector3D;
typedef Tec3D::Point3D		TecPoint3D;
typedef Tec3D::Vector4D		TecVector4D;
typedef Tec3D::Quaternion	TecQuaternion;
typedef Tec3D::Matrix3x3	TecMatrix3x3;
typedef Tec3D::Matrix3x4	TecMatrix3x4;
typedef Tec3D::Matrix4x4	TecMatrix4x4;
typedef Tec3D::Basis3D		TecBasis3D;
typedef Tec3D::Reference3D	TecReference3D;

using namespace Tec3D;
#else
typedef Vector3D	TecVector3D;
typedef Point3D		TecPoint3D;
typedef Vector4D	TecVector4D;
typedef Quaternion	TecQuaternion;
typedef Matrix3x3	TecMatrix3x3;
typedef Matrix3x4	TecMatrix3x4;
typedef Matrix4x4	TecMatrix4x4;
typedef Basis3D		TecBasis3D;
typedef Reference3D	TecReference3D;
#endif

// Shorthand syntax
using v2f = Point2D;
using v3f = Point3D;
using v4f = Vector4D;

using v2i = Point2DI;
using v2u = Point2DUI;

using v3i = Point3DI;
using v3u = Point3DUI;

using mat3 = Matrix3x3;
using mat3x4 = Matrix3x4;
using mat4 = Matrix4x4;

using quat = Quaternion;

#define V4F_FMT(n) "v4(%."#n"f, %."#n"f, %."#n"f, %."#n"f)"
#define V4F_EXP(v) v.x, v.y, v.z, v.w

#define V3F_FMT(n) "v3(%."#n"f, %."#n"f, %."#n"f)"
#define V3F_EXP(v) v.x, v.y, v.z

#define V2F_FMT(n) "v2(%."#n"f, %."#n"f)"
#define V2F_EXP(v) v.x, v.y


#endif//_3D_h_


