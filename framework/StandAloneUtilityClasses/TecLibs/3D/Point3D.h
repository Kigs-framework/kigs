#pragma once
// **********************************************************************
// * FILE  : Point3D.h
// * GROUP : TecLibs/3D
// *---------------------------------------------------------------------
// * PURPOSE : Class for 3D Point
// * COMMENT :Only difference with Vector3D is that Matrix3x4 tranform applies the translation
// *---------------------------------------------------------------------
// **********************************************************************
namespace Kigs
{
	namespace Maths
	{

		// ----------------------------------------------------------------------
		// **************************************************
		// * Point3D
		// *-------------------------------------------------
		// * - Definition of 3D vector
		// * - 
		// **************************************************
		struct Point3D
		{
			// +---------
			// | Life-cyle
			// +---------
			// Constructors
			inline Point3D(); // DO NOTHING! (For immediate creation)

			// Set all coordinates to fValue
			inline Point3D(const Float& x, const Float& y, const Float& z);
			inline Point3D(const Point3D& P);
			// Vector A to B = B-A
			inline Point3D(const Point3D& A, const Point3D& B, const asVector& v);

			// Vector A to B = (A+B)*0.5;
			inline Point3D(const Point3D& A, const Point3D& B, const asMiddle& m);

			// Set (constructors-like)
			inline void Set(const Float& fValue);
			inline void Set(const Float& x, const Float& y, const Float& z);
			inline void Set(const Point3D& V);
			// Vector A to B = B-A
			inline void Set(const Point3D& A, const Point3D& B);
			//inline void Set( const Vector3D& P );

			// Assignement
			inline Point3D& operator =(const Point3D& V);
			inline Point3D& operator =(const Float& V);

			// +---------
			// | Addition/Substraction
			// +---------
			// With another vector (Internal laws)
			friend inline Point3D operator + (const Point3D& U, const Point3D& V);
			friend inline Point3D operator - (const Point3D& U, const Point3D& V);

			// With assignement
			inline Point3D& operator += (const Point3D& V);
			inline Point3D& operator -= (const Point3D& V);

			// Unary
			friend inline Point3D operator - (const Point3D& V);


			// +---------
			// | Multiplication/Division
			// +---------
			// With a scalar (External laws)
			friend inline Point3D operator * (const Float& fValue, const Point3D& V);
			friend inline Point3D operator * (const Point3D& V, const Float& fValue);

			// Avoid using this, it's too ambiguous (dot, cross or hadamard)
			friend inline Point3D operator * (Point3D V, const Point3D& W);

			// Same as a*b but the intent is more clear
			friend inline Point3D Hadamard(Point3D a, const Point3D& b);

			friend inline Point3D operator / (const Float& fValue, const Point3D& V);
			friend inline Point3D operator / (const Point3D& V, const Float& fValue);

			friend inline Point3D operator / (Point3D V, const Point3D& W);

			// With assignement
			inline Point3D& operator *= (const Float& fValue);
			inline Point3D& operator += (const Float& fValue);
			inline Point3D& operator -= (const Float& fValue);
			inline Point3D& operator /= (const Float& fValue);

			inline Point3D& operator *= (const Point3D& p);
			inline Point3D& operator /= (const Point3D& p);

			// +---------
			// | Euclidian operations
			// +---------
			friend inline Float Norm(const Point3D& P);
			friend inline Float NormSquare(const Point3D& P);
			friend inline Float Dist(const Point3D& P1, const Point3D& P2);
			friend inline Float DistSquare(const Point3D& P1, const Point3D& P2);
			friend inline Float SegmentDist(const Point3D& Pt1, const Point3D& Pt2, const Point3D& Pt3, const Point3D& Pt4);
			friend inline Float PointToSegmentDist(const Point3D& Pt, const Point3D& Pt1, const Point3D& Pt2, Point3D& nearest, bool& insideSegment);



			// +---------
			// | Cross product
			// +---------
			friend inline Point3D operator ^ (const Point3D& U, const Point3D& V);

			// With assignement
			inline const Point3D& operator ^= (const Point3D& V);
			inline void CrossProduct(const Point3D& V, const Point3D& W);

			// +---------
			// | Euclidian operations
			// +---------
			friend inline Float Dot(const Point3D& U, const Point3D& V);



			// +---------
			// | Affine operations
			// +---------
			friend inline Point3D Mid(const Point3D& P, const Point3D& Q);
			friend inline Point3D Bary(const Float& a, const Point3D& P, const Float& b, const Point3D& Q);
			friend inline Point3D Lerp(const Point3D& P, const Point3D& Q, const Float& t);
			// Blend is the same as Lerp, but Blend is available and as the same prototype for Vector3D, Point2D, Quaternion... so 
			// it can be called in template methods
			friend inline Point3D Blend(const Point3D& U, const Point3D& V, Float t)
			{
				return Lerp(U, V, t);
			}

			// +---------
			// | Utilities
			// +---------
			inline void Normalize(void);
			inline Point3D Normalized(void) const;
			inline void ClampMax(const Float v);
			inline void ClampMin(const Float v);

			friend inline bool operator< (const Point3D& U, const Point3D& V);
			friend inline bool operator<=(const Point3D& U, const Point3D& V);
			friend inline bool operator> (const Point3D& U, const Point3D& V);
			friend inline bool operator>=(const Point3D& U, const Point3D& V);
			friend inline bool operator==(const Point3D& U, const Point3D& V);
			friend inline bool operator!=(const Point3D& U, const Point3D& V);
			friend inline bool operator==(const Point3D& a, const Point3D& b);
			friend inline bool operator!=(const Point3D& a, const Point3D& b);


			friend inline Point3D ProjectOnPlane(Point3D q, Point3D o, Point3D n);

			// +---------
			// | Acces Operators
			// +---------
			inline const Float& operator[](Int i) const;
			inline Float& operator[](Int i);

			Point2D XZ() const { return Point2D(x, z); }

			// +---------
			// | Coordinates
			// +---------

			union
			{
				struct
				{
					Float   x;
					Float   y;
					Float   z;
				};

				struct
				{
					Point2D xy;
					Float __Ignored1;
				};

				struct
				{
					Float __Ignored2;
					Point2D yz;
				};

				Float data[3];
			};
		};

		// define struct Segment3D

		struct Segment3D
		{
			inline Segment3D() { ; } // DO NOTHING! (For immediate creation)
			Segment3D(const Segment3D& s) : p1(s.p1), p2(s.p2) { ; }
			Segment3D& operator=(const Segment3D& s) { p1 = s.p1; p2 = s.p2; return *this; }
			union {
				struct {
					Point3D	p1;
					Point3D p2;
				};

				Point3D	pts[2];
			};
		};

	}
}


