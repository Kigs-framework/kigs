#pragma once
// **********************************************************************
// * FILE  : 3D.h
// * GROUP : TecLibs/3D
// *---------------------------------------------------------------------
// * PURPOSE : 3D classes
// * COMMENT : ...
// *---------------------------------------------------------------------
// **********************************************************************

// ----------------------------------------------------------------------
// +---------
// | 3D section
// +---------

// General includes
//#include "TecLibs.h"                // To be Tec
//#include "TecMath.h"                // To use sqrt, cos, etc.


// Shorthand syntax
using s8 = int8_t;
using u8 = uint8_t;
using s16 = int16_t;
using u16 = uint16_t;
using s32 = int32_t;
using u32 = uint32_t;
using s64 = int64_t;
using u64 = uint64_t;

using f32 = float;
using f64 = double;

/*namespace Kigs
{
	namespace Maths
	{
		constexpr float SMALL_NUM = 0.001f;
		// Forward declarations
		struct v3f;
		struct Point3D;
		struct Vector4D;
		struct Quaternion;

		struct Matrix3x3;
		struct Matrix3x4;
		struct Matrix4x4;
		class Basis3D;
		class Reference3D;
	}
}

// Classes definitions

#include "2D/Point2D.h"         // 
#include "2D/Point2DI.h"         //

#include "3D/Point3D.h"             // To use 3D point
#include "3D/v3f.h"            // To use 3D vector
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
#include "3D/v3f.inl.h"
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
*/


#include "Eigen/Core"
#include "Eigen/Geometry"
// Shorthand syntax

using v2f = Eigen::Vector2f;
using v3f = Eigen::Vector3f;
using v4f = Eigen::Vector4f;

using v2i = Eigen::Vector2i;
using v2u = Eigen::Vector2<u32>;

using v3i = Eigen::Vector3i;
using v3u = Eigen::Vector3<u32>;

using mat3 = Eigen::Matrix3f;
using mat3x4 = Eigen::Matrix<f32, 3, 4>;
using mat4 = Eigen::Matrix4f;


class KigsQuaternion : public Eigen::Quaternion<f32>
{
public:

	using Eigen::Quaternion<f32>::Quaternion;

	inline f32& operator[](size_t index)
	{
		return coeffs()[index];
	}

	inline const f32& operator[](size_t index) const
	{
		return coeffs()[index];
	}
};

using quat = KigsQuaternion;



/*using v2f = Kigs::Maths::Point2D;
using v3f = Kigs::Maths::Point3D;
using v4f = Kigs::Maths::Vector4D;

using v2i = Kigs::Maths::Point2DI;
using v2u = Kigs::Maths::Point2DUI;

using v3i = Kigs::Maths::Point3DI;
using v3u = Kigs::Maths::Point3DUI;

using mat3 = Kigs::Maths::Matrix3x3;
using mat3x4 = Kigs::Maths::Matrix3x4;
using mat4 = Kigs::Maths::Matrix4x4;

using quat = Kigs::Maths::Quaternion;
*/
#define V4F_FMT(n) "v4(%."#n"f, %."#n"f, %."#n"f, %."#n"f)"
#define V4F_EXP(v) v.x, v.y, v.z, v.w

#define V3F_FMT(n) "v3(%."#n"f, %."#n"f, %."#n"f)"
#define V3F_EXP(v) v.x, v.y, v.z

#define V2F_FMT(n) "v2(%."#n"f, %."#n"f)"
#define V2F_EXP(v) v.x, v.y



