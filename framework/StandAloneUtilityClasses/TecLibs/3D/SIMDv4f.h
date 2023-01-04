// **********************************************************************
// * FILE  : SIMDv4f.h
// * GROUP : TecLibs/3D
// *---------------------------------------------------------------------
// * PURPOSE : Class for 3D (or 4d) Points/vectors computation using 4 floats simd vectors
// *---------------------------------------------------------------------
// **********************************************************************

#pragma once

#ifdef USE_SIMD
#include "NEON_2_SSE.h"

namespace Kigs
{
	namespace Maths
	{


		// ----------------------------------------------------------------------
		// **************************************************
		// * SIMDv4f
		// *-------------------------------------------------
		// * - 
		// * - 
		// **************************************************




		struct SIMDv4f
		{
			// +---------
			// | Life-cyle
			// +---------
			// Constructors
			inline SIMDv4f(); // DO NOTHING! (For immediate creation)

			// Set all coordinates to fValue
			inline SIMDv4f(const float x, const float y, const float z, const float w = 0);
			inline SIMDv4f(const SIMDv4f& P);
			inline SIMDv4f(const Point3D& P);
			// Vector A to B = B-A
			inline SIMDv4f(const SIMDv4f& A, const SIMDv4f& B, const asVector& v);

			// Vector A to B = (A+B)*0.5;
			inline SIMDv4f(const SIMDv4f& A, const SIMDv4f& B, const asMiddle& m);

			// Set (constructors-like)
			inline void Set(const float fValue);
			inline void Set(const float x, const float y, const float z, const float w = 0);
			inline void Set(const SIMDv4f& V);

			// Assignement
			inline SIMDv4f& operator =(const SIMDv4f& V);
			inline SIMDv4f& operator =(const float V);

			// +---------
			// | Addition/Substraction
			// +---------
			// With another vector (Internal laws)
			friend inline SIMDv4f operator + (const SIMDv4f& U, const SIMDv4f& V);
			friend inline SIMDv4f operator - (const SIMDv4f& U, const SIMDv4f& V);

			// With assignement
			inline SIMDv4f& operator += (const SIMDv4f& V);
			inline SIMDv4f& operator -= (const SIMDv4f& V);

			// Unary
			friend inline SIMDv4f operator - (const SIMDv4f& V);

			// +---------
			// | Multiplication/Division
			// +---------
			// With a scalar (External laws)
			friend inline SIMDv4f operator * (const float fValue, const SIMDv4f& V);
			friend inline SIMDv4f operator * (const SIMDv4f& V, const float fValue);

			// Avoid using this, it's too ambiguous (dot, cross or hadamard)
			friend inline SIMDv4f operator * (SIMDv4f V, const SIMDv4f& W);

			// Same as a*b but the intent is more clear
			friend inline SIMDv4f Hadamard(SIMDv4f a, const SIMDv4f& b);

			friend inline SIMDv4f operator / (const float fValue, const SIMDv4f& V);
			friend inline SIMDv4f operator / (const SIMDv4f& V, const float fValue);

			friend inline SIMDv4f operator / (SIMDv4f V, const SIMDv4f& W);

			// With assignement
			inline SIMDv4f& operator *= (const float fValue);
			inline SIMDv4f& operator += (const float fValue);
			inline SIMDv4f& operator -= (const float fValue);
			inline SIMDv4f& operator /= (const float fValue);

			inline SIMDv4f& operator *= (const SIMDv4f& p);
			inline SIMDv4f& operator /= (const SIMDv4f& p);

			// +---------
			// | Euclidian operations
			// +---------
			friend inline float Norm(const SIMDv4f& P);
			friend inline float NormSquare(const SIMDv4f& P);
			friend inline float Dist(const SIMDv4f& P1, const SIMDv4f& P2);
			friend inline float DistSquare(const SIMDv4f& P1, const SIMDv4f& P2);
			friend inline float SegmentDist(const SIMDv4f& Pt1, const SIMDv4f& Pt2, const SIMDv4f& Pt3, const SIMDv4f& Pt4);
			friend inline float PointToSegmentDist(const SIMDv4f& Pt, const SIMDv4f& Pt1, const SIMDv4f& Pt2, SIMDv4f& nearest, bool& insideSegment);

			// +---------
			// | Cross product
			// +---------
			friend inline SIMDv4f operator ^ (const SIMDv4f& U, const SIMDv4f& V);

			// With assignement
			inline const SIMDv4f& operator ^= (const SIMDv4f& V);
			inline void CrossProduct(const SIMDv4f& V, const SIMDv4f& W);

			// +---------
			// | Euclidian operations
			// +---------
			friend inline float Dot(const SIMDv4f& U, const SIMDv4f& V);

			// +---------
			// | Affine operations
			// +---------
			friend inline SIMDv4f Mid(const SIMDv4f& P, const SIMDv4f& Q);
			friend inline SIMDv4f Bary(const float a, const SIMDv4f& P, const float b, const SIMDv4f& Q);
			friend inline SIMDv4f Lerp(const SIMDv4f& P, const SIMDv4f& Q, const float t);
			// Blend is the same as Lerp, but Blend is available and as the same prototype for Vector3D, Point2D, Quaternion... so 
			// it can be called in template methods
			friend inline SIMDv4f Blend(const SIMDv4f& U, const SIMDv4f& V, float t)
			{
				return Lerp(U, V, t);
			}

			// +---------
			// | Utilities
			// +---------
			inline void Normalize(void);
			inline void NormalizeBarycentricCoords();
			inline SIMDv4f Normalized(void) const;
			inline SIMDv4f NormalizedBarycentricCoords(void) const;
			inline void ClampMax(const float v);
			inline void ClampMin(const float v);

			friend inline bool operator< (const SIMDv4f& U, const SIMDv4f& V);
			friend inline bool operator<=(const SIMDv4f& U, const SIMDv4f& V);
			friend inline bool operator> (const SIMDv4f& U, const SIMDv4f& V);
			friend inline bool operator>=(const SIMDv4f& U, const SIMDv4f& V);
			friend inline bool operator==(const SIMDv4f& U, const SIMDv4f& V);
			friend inline bool operator!=(const SIMDv4f& U, const SIMDv4f& V);
			friend inline bool operator==(const SIMDv4f& a, const SIMDv4f& b);
			friend inline bool operator!=(const SIMDv4f& a, const SIMDv4f& b);


			friend inline SIMDv4f ProjectOnPlane(SIMDv4f q, SIMDv4f o, SIMDv4f n);

			// +---------
			// | Acces Operators
			// +---------
			inline const float& operator[](Int i) const;
			inline float& operator[](Int i);

			Point2D XZ() const { return Point2D(x, z); }

			union
			{
				struct
				{
					float   x;
					float   y;
					float   z;
					float	w;
				};

				struct
				{
					Point2D xy;
					Point2D zw;
				};

				struct
				{
					Point3D xyz;
					float	w;
				};

				struct
				{
					float	x;
					Point3D yzw;
				};

				float32x4_t	v;
			};

		};

	}
}

#endif