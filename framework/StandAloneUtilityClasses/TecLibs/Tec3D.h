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
#include "unsupported/Eigen/EulerAngles"
// Shorthand syntax



namespace Kigs
{
	using v2f = Eigen::Vector2f;
	using v3f = Eigen::Vector3f;
	using v4f = Eigen::Vector4f;

	using v2i = Eigen::Vector2i;
	using v2u = Eigen::Vector2<u32>;

	using v3i = Eigen::Vector3i;
	using v3si = Eigen::Vector3<short>;
	using v3u = Eigen::Vector3<u32>;



	namespace Maths
	{
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

		template<size_t rowC, size_t colC>
		class KigsMat : public Eigen::Matrix<f32, rowC, colC, Eigen::ColMajor>
		{
		public:

			class KigsColAccess
			{
			protected:
				Eigen::Matrix<f32, rowC, colC, Eigen::ColMajor>& mMat;
				size_t											 mColIndex;
			public:
				KigsColAccess(Eigen::Matrix<f32, rowC, colC, Eigen::ColMajor>& mat, size_t index) : mMat(mat), mColIndex(index)
				{

				}

				KigsColAccess(const KigsColAccess& other) : mMat(other.mMat), mColIndex(other.mColIndex)
				{

				}

				inline f32& operator[](size_t index)
				{
					return mMat(index, mColIndex);
				}
				inline const f32& operator[](size_t index) const
				{
					return mMat(index, mColIndex);
				}

				inline auto operator()(size_t index)
				{
					return mMat(index, mColIndex);
				}
			};

			class KigsColAccessConst : public Eigen::Matrix<f32, colC, 1>
			{
			protected:
				
			public:
				KigsColAccessConst(const Eigen::Matrix<f32, rowC, colC, Eigen::ColMajor>& mat, size_t index)
				{
					(*this) = mat.col(index);
				}
			};


			using Eigen::Matrix<f32, rowC, colC, Eigen::ColMajor>::Matrix;
			using Eigen::Matrix<f32, rowC, colC, Eigen::ColMajor>::operator=;
			using Eigen::Matrix<f32, rowC, colC, Eigen::ColMajor>::operator();

			inline KigsColAccess operator[](size_t index) 
			{
				return KigsColAccess(*this, index);
			}

			inline auto operator()(size_t index) 
			{
				return KigsColAccess(*this, index);
			}

			inline  KigsColAccessConst operator[](size_t index) const
			{
				return KigsColAccessConst(*this,index);
			}

			inline KigsColAccessConst operator()(size_t index) const
			{
				return KigsColAccessConst(*this, index);
			}
		};
	}
	using mat3 = Maths::KigsMat<3,3>;
	using mat3x4 = Maths::KigsMat<3,4>;
	using mat4 = Maths::KigsMat<4,4>;

	namespace Maths
	{
		inline void toPRS(const mat3x4& fromMat, v3f& pos, v3f& rot, f32& s)
		{
			mat3 rotmat = fromMat.block<3, 3>(0, 0);
			mat3 scalemat;

			Eigen::Transform<f32, 3, Eigen::AffineCompact>	transform(rotmat);
			pos = transform.translation();
			
			transform.computeScalingRotation(&scalemat, &rotmat);

			rot=rotmat.canonicalEulerAngles(2, 1, 0);
			s = scalemat.norm();
		}

		inline mat3x4 fromPRS(v3f& pos, v3f& rot, f32& s)
		{
			Eigen::Transform<f32, 3, Eigen::AffineCompact>	transform;

			transform.setIdentity();
			mat3 m;
			m = Eigen::AngleAxisf(rot.x(), Eigen::Vector3f::UnitX())
				* Eigen::AngleAxisf(rot.y(), Eigen::Vector3f::UnitY())
				* Eigen::AngleAxisf(rot.z(), Eigen::Vector3f::UnitZ());

			transform.rotate(m);
			transform.prescale(s);
			transform.translate(pos);
			return  transform.matrix();

		}

		inline void	TransformPoints(const mat3x4& transform, v3f* pts, size_t count)
		{
			mat3 submat = transform.block<3, 3>(0, 0);
			for (size_t i = 0; i < count; i++)
			{
				*pts=(submat * (*pts))+ transform.col(3);
				++pts;
			}
		}
		inline void	TransformPoint(const mat3x4& transform, v3f& pt)
		{
			mat3 submat = transform.block<3, 3>(0, 0);
			pt = submat * pt + transform.col(3);
		}

		inline void	TransformVectors(const mat3x4& transform, v3f* pts, size_t count)
		{
			mat3 submat= transform.block<3, 3>(0, 0);
			for (size_t i = 0; i < count; i++)
			{
				*pts = (submat * (*pts));
				++pts;
			}
		}
		inline void	TransformVector(const mat3x4& transform, v3f& v)
		{
			mat3 submat = transform.block<3, 3>(0, 0);
			v = submat * v;
		}

		inline mat3x4 LookAt(const v3f& eye, const v3f& target, const v3f& up)
		{
			v3f f = (target - eye).normalized();
			v3f u = up.normalized();
			v3f s = f.cross(u).normalized();
			u = s.cross(f);
			mat3x4 mat = mat3x4::Zero();
			mat(0, 0) = s.x();
			mat(0, 1) = s.y();
			mat(0, 2) = s.z();
			mat(0, 3) = -s.dot(eye);
			mat(1, 0) = u.x();
			mat(1, 1) = u.y();
			mat(1, 2) = u.z();
			mat(1, 3) = -u.dot(eye);
			mat(2, 0) = -f.x();
			mat(2, 1) = -f.y();
			mat(2, 2) = -f.z();
			mat(2, 3) = f.dot(eye);

			return mat;
		}


	}

	using quat = Kigs::Maths::KigsQuaternion;
}





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



