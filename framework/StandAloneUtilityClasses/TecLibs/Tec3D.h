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
/*#include "TecLibs.h"                // To be Tec
#include "TecMath.h"                // To use sqrt, cos, etc.
*/
#include "glm/glm.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext.hpp"
#undef GLM_ENABLE_EXPERIMENTAL

namespace Kigs
{

	namespace Maths
	{
		/*struct Quaternion : public glm::quat
		{

		};*/


		/*constexpr float SMALL_NUM = 0.001f;
		// Forward declarations
		struct v3f;
		struct v3f;
		struct v4f;
		struct Quaternion;

		struct mat3;
		struct mat4;
		struct mat4;
		class Basis3D;
		class Reference3D;*/


	}
}

// Classes definitions
/*
#include "2D/v2f.h"         // 
#include "2D/v2i.h"         //

#include "3D/v3f.h"             // To use 3D point
#include "3D/v3f.h"            // To use 3D vector
#include "3D/Point3DI.h"             // To use 3D point
#include "3D/v3d.h"             	// To use 3D point


#include "3D/v4f.h"            // To use 4D vector (projective geometry)
#include "3D/Quaternion.h"          // To use quaternion

#include "2D/AffineTransform2D.h"
#include "3D/mat3.h"           // To use 3x3 matrix
#include "3D/mat4.h"           // To use 3x4 matrix
#include "3D/mat4.h"           // To use 4x4 matrix
#include "3D/Basis3D.h"             // To use 3D basis
#include "3D/Reference3D.h"         // To use 3D reference


// Classes implementations
#include "3D/v3f.inl.h"
#include "3D/v3f.inl.h"
#include "3D/v3d.inl.h"
#include "3D/Point3DI.inl.h"
#include "3D/v4f.inl.h"
#include "3D/Quaternion.inl.h"
#include "3D/mat3.inl.h"
#include "3D/mat4.inl.h"
#include "3D/mat4.inl.h"
#include "3D/Basis3D.inl.h"
#include "3D/Reference3D.inl.h"

#include "2D/v2f.inl.h"
#include "2D/v2i.inl.h"
#include "2D/AffineTransform2D.inl.h"

#include "3D/SIMDv4f.h"
#include "3D/SIMDv4f.inl.h"*/
	
// Shorthand syntax
using v2f = glm::vec2;
using v3f = glm::vec3;
using v4f = glm::vec4;

using v2i = glm::ivec2;
using v2u = glm::uvec2;

using v3i = glm::ivec3;
using v3u = glm::uvec3;

using mat3 = glm::mat3x3;
using mat4 = glm::mat4x4;

using quat = glm::quat;

#define V4F_FMT(n) "v4(%."#n"f, %."#n"f, %."#n"f, %."#n"f)"
#define V4F_EXP(v) v.x, v.y, v.z, v.w

#define V3F_FMT(n) "v3(%."#n"f, %."#n"f, %."#n"f)"
#define V3F_EXP(v) v.x, v.y, v.z

#define V2F_FMT(n) "v2(%."#n"f, %."#n"f)"
#define V2F_EXP(v) v.x, v.y

namespace glm
{

	// mat4 v3f
	inline void transformPoint(const mat4& x, v3f& v)
	{
		v = x * v4f(v, 1.0);
	}

	inline void transformPoint(const mat4& x, const v3f& in, v3f& out)
	{
		out = x * v4f(in, 1.0);
	}

	inline v3f transformPoint(const mat4& x, v3f const & v)
	{
		return x* v4f(v, 1.0);
	}

	inline void transformPoints(const mat4& x, v3f* v , size_t count)
	{
		for (size_t i = 0; i < count; i++)
		{
			v[i] = x * v4f(v[i], 1.0);
		}
	}

	inline void transformVector(const mat4& x, v3f& v)
	{
		v = x * v4f(v, 0.0);
	}

	inline v3f transformVector(const mat4& x, v3f const & v)
	{
		return x * v4f(v, 0.0);
	}

	inline void transformVectors(const mat4& x, v3f* v, size_t count)
	{
		for (size_t i = 0; i < count; i++)
		{
			v[i] = x * v4f(v[i], 0.0);
		}
	}

	inline bool isIdentity(const mat4& x)
	{
		for (mat4::length_type r = 0; r < 4; r++)
		{
			for (mat4::length_type c = 0; c < 4; c++)
			{
				if (x[c][r] != ((c == r) ? 1.0f : 0.0f))
				{
					return false;
				}
			}
		}
		return true;
	}

	inline v3f projectOnPlane(const v3f& q, const v3f& o, const v3f& n)
	{
		return q - (dot(v3f(q - o), n) * n);
	}

	// mat3 v2f
	inline void transformPoint(const mat3& x, v2f& v)
	{
		v = x * v3f(v, 1.0);
	}

	inline void transformPoint(const mat3& x, const v2f& in, v2f& out)
	{
		out = x * v3f(in, 1.0);
	}

	inline v3f transformPoint(const mat3& x, v2f const& v)
	{
		return x * v3f(v, 1.0);
	}

	inline void transformPoints(const mat3& x, v2f* v, size_t count)
	{
		for (size_t i = 0; i < count; i++)
		{
			v[i] = x * v3f(v[i], 1.0);
		}
	}

	inline void transformVector(const mat3& x, v2f& v)
	{
		v = x * v3f(v, 0.0);
	}

	inline v3f transformVector(const mat3& x, v2f const& v)
	{
		return x * v3f(v, 0.0);
	}

	inline void transformVectors(const mat3& x, v2f* v, size_t count)
	{
		for (size_t i = 0; i < count; i++)
		{
			v[i] = x * v3f(v[i], 0.0);
		}
	}

	inline mat3 preScale(const mat3& m,const float ScaleX, const float ScaleY, const float ScaleZ)
	{
		mat3 result;
		result[0][0] = m[0][0] * ScaleX;
		result[0][1] = m[0][1] * ScaleX;
		result[0][2] = m[0][2] * ScaleX;
		result[1][0] = m[1][0] * ScaleY;
		result[1][1] = m[1][1] * ScaleY;
		result[1][2] = m[1][2] * ScaleY;
		result[2][0] = m[2][0] * ScaleZ;
		result[2][1] = m[2][1] * ScaleZ;
		result[2][2] = m[2][2] * ScaleZ;
		return result;
	}


	// +---------
	// | PostScale   =  Scale_Matrix * (*this)
	// +---------
	inline mat3 postScale(const mat3& m, const float ScaleX, const float ScaleY, const float ScaleZ)
	{
		mat3 result;
		result[0][0] = m[0][0] * ScaleX;
		result[0][1] = m[0][1] * ScaleY;
		result[0][2] = ScaleZ * m[0][2];
		result[1][0] = ScaleX * m[1][0];
		result[1][1] = m[1][1] * ScaleY;
		result[1][2] = ScaleZ * m[1][2];
		result[2][0] = ScaleX * m[2][0];
		result[2][1] = m[2][1] * ScaleY;
		result[2][2] = m[2][2] * ScaleZ;
		return result;
	}

	inline mat3 preRotateZ(const mat3& m, const float angle)
	{
		mat3 result;
		float COS = cosf(angle);
		float SIN = sinf(angle);
		result[1][0] = -m[0][0] * SIN + m[1][0] * COS;
		result[1][1] = -m[0][1] * SIN + m[1][1] * COS;
		result[1][2] = -m[0][2] * SIN + m[1][2] * COS;
		result[0][0] = m[0][0] * COS + m[1][0] * SIN;
		result[0][1] = m[0][1] * COS + m[1][1] * SIN;
		result[0][2] = m[0][2] * COS + m[1][2] * SIN;
		return result;
	}

}