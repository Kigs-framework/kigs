#pragma once

namespace Kigs
{
	namespace Maths
	{
		struct v3d
		{
			// +---------
			// | Life-cyle
			// +---------
			// Constructors
			inline v3d(); // DO NOTHING! (For immediate creation)

			// Set all coordinates to fValue
			inline v3d(const double& x, const double& y, const double& z);
			inline v3d(const v3d& P);
			// Vector A to B = B-A
			inline v3d(const v3d& A, const v3d& B, const asVector& v);

			// Vector A to B = (A+B)*0.5;
			inline v3d(const v3d& A, const v3d& B, const asMiddle& m);

			// Set (constructors-like)
			inline void Set(const double& fValue);
			inline void Set(const double& x, const double& y, const double& z);
			inline void Set(const v3d& V);
			// Vector A to B = B-A
			inline void Set(const v3d& A, const v3d& B);
			//inline void Set( const Vector3D& P );

			// Assignement
			inline v3d& operator =(const v3d& V);
			inline v3d& operator =(const double& V);

			// +---------
			// | Addition/Substraction
			// +---------
			// With another vector (Internal laws)
			friend inline v3d operator + (const v3d& U, const v3d& V);
			friend inline v3d operator - (const v3d& U, const v3d& V);

			// With assignement
			inline v3d& operator += (const v3d& V);
			inline v3d& operator -= (const v3d& V);

			// Unary
			friend inline v3d operator - (const v3d& V);

			Point3D to_v3f() { return Point3D{ float(x), float(y), float(z) }; }

			// +---------
			// | Multiplication/Division
			// +---------
			// With a scalar (External laws)
			friend inline v3d operator * (const double& fValue, const v3d& V);
			friend inline v3d operator * (const v3d& V, const double& fValue);

			// Avoid using this, it's too ambiguous (dot, cross or hadamard)
			friend inline v3d operator * (v3d V, const v3d& W);

			// Same as a*b but the intent is more clear
			friend inline v3d Hadamard(v3d a, const v3d& b);

			friend inline v3d operator / (const double& fValue, const v3d& V);
			friend inline v3d operator / (const v3d& V, const double& fValue);

			friend inline v3d operator / (v3d V, const v3d& W);

			// With assignement
			inline v3d& operator *= (const double& fValue);
			inline v3d& operator /= (const double& fValue);

			inline v3d& operator *= (const v3d& p);
			inline v3d& operator /= (const v3d& p);

			// +---------
			// | Euclidian operations
			// +---------
			friend inline double Norm(const v3d& P);
			friend inline double NormSquare(const v3d& P);
			friend inline double Dist(const v3d& P1, const v3d& P2);
			friend inline double DistSquare(const v3d& P1, const v3d& P2);
			friend inline double SegmentDist(const v3d& Pt1, const v3d& Pt2, const v3d& Pt3, const v3d& Pt4);
			friend inline double PointToSegmentDist(const v3d& Pt, const v3d& Pt1, const v3d& Pt2, v3d& nearest, bool& insideSegment);



			// +---------
			// | Cross product
			// +---------
			friend inline v3d operator ^ (const v3d& U, const v3d& V);

			// With assignement
			inline const v3d& operator ^= (const v3d& V);
			inline void CrossProduct(const v3d& V, const v3d& W);

			// +---------
			// | Euclidian operations
			// +---------
			friend inline double Dot(const v3d& U, const v3d& V);



			// +---------
			// | Affine operations
			// +---------
			friend inline v3d Mid(const v3d& P, const v3d& Q);
			friend inline v3d Bary(const double& a, const v3d& P, const double& b, const v3d& Q);
			friend inline v3d Lerp(const v3d& P, const v3d& Q, const double& t);
			// Blend is the same as Lerp, but Blend is available and as the same prototype for Vector3D, Point2D, Quaternion... so 
			// it can be called in template methods
			friend inline v3d Blend(const v3d& U, const v3d& V, double t)
			{
				return Lerp(U, V, t);
			}

			// +---------
			// | Utilities
			// +---------
			inline void Normalize();
			inline v3d Normalized() const;
			inline void ClampMax(const double v);
			inline void ClampMin(const double v);

			friend inline bool operator< (const v3d& U, const v3d& V);
			friend inline bool operator<=(const v3d& U, const v3d& V);
			friend inline bool operator> (const v3d& U, const v3d& V);
			friend inline bool operator>=(const v3d& U, const v3d& V);
			friend inline bool operator==(const v3d& U, const v3d& V);
			friend inline bool operator!=(const v3d& U, const v3d& V);
			friend inline bool operator==(const v3d& a, const v3d& b);
			friend inline bool operator!=(const v3d& a, const v3d& b);


			friend inline v3d ProjectOnPlane(v3d q, v3d o, v3d n);

			// +---------
			// | Acces Operators
			// +---------
			inline const double& operator[](int i) const;
			inline double& operator[](int i);

			// +---------
			// | Coordinates
			// +---------

			union
			{
				struct
				{
					double   x;
					double   y;
					double   z;
				};
				double data[3];
			};
		};

	}
}

