#pragma once

// ----------------------------------------------------------------------
// **************************************************
// * Point2DIBase & Point2DSI
// *-------------------------------------------------
// * - Definition of 2D point with integer coordType coordinates
// * - 
// **************************************************

#include "TecLibs/Tec3D.h"
namespace Kigs
{
	namespace Maths
	{
		template<typename coordType>
		struct Point2DIBase
		{
			// +---------
			// | Life-cyle
			// +---------
			// Constructors
			inline Point2DIBase()
			{

			}

			// from float and Point2D
			inline explicit Point2DIBase(const Float& fValue)
			{
				x = (coordType)(fValue + 0.5f);
				y = x;
			}
			inline Point2DIBase(const Float& fx, const Float& fy)
			{
				x = (coordType)(fx + 0.5f);
				y = (coordType)(fy + 0.5f);
			}
			inline Point2DIBase(const Point2D& P)
			{
				x = (coordType)(P.x + 0.5f);
				y = (coordType)(P.y + 0.5f);
			}
			/*inline Point2DIBase( const Point3DI& P )
		 {
		  x=(coordType)(P.x);
		  y=(coordType)(P.y);
		 }*/
		 // from coordType and Point2DIBase
			inline explicit Point2DIBase(const coordType& Value)
			{
				x = Value;
				y = x;
			}
			inline Point2DIBase(const coordType ix, const coordType iy)
			{
				x = ix;
				y = iy;
			}
			inline void Set(const coordType ix, const coordType iy)
			{
				x = ix;
				y = iy;
			}
			inline Point2DIBase(const Point2DIBase& P)
			{
				x = P.x;
				y = P.y;
			}

			// construct middle
			inline Point2DIBase(const Point2DIBase& V1, const Point2DIBase& V2)
			{
				x = V1.x + V2.x;
				y = V1.y + V2.y;
				x >>= 1;	   	y >>= 1;
			}


			// Assignement
			inline const Point2DIBase& operator =(const Point2DIBase& V)
			{
				x = V.x;
				y = V.y;
				return (*this);
			}

			// Conversion
			inline operator Point2D() const { return Point2D((Float)x, (Float)y); }

			// compare
			bool operator==(const Point2DIBase& Other) const
			{
				return (x == Other.x && y == Other.y);
			}

			bool operator!=(const Point2DIBase& Other) const
			{
				return x != Other.x || y != Other.y;
			}

			// +---------
			// | Addition/Substraction
			// +---------

			// With assignement
			inline const Point2DIBase& operator += (const Point2DIBase& V)
			{
				x += V.x;
				y += V.y;
				return (*this);
			}
			inline const Point2DIBase& operator -= (const Point2DIBase& V)
			{
				x -= V.x;
				y -= V.y;
				return (*this);
			}

			// Unary
			friend inline Point2DIBase operator - (const Point2DIBase& V)
			{
				return Point2DIBase(-V.x, -V.y);
			}

			// +---------
			// | Multiplication/Division
			// +---------
			// With a scalar (External laws)
			friend inline Point2DIBase operator * (const Float& fValue, const Point2DIBase& V)
			{
				return Point2DIBase(((Float)V.x) * fValue, ((Float)V.y) * fValue);
			}
			friend inline Point2DIBase operator * (const Point2DIBase& V, const Float& fValue)
			{
				return Point2DIBase(((Float)V.x) * fValue, ((Float)V.y) * fValue);
			}
			friend inline Point2DIBase operator / (const Float& fValue, const Point2DIBase& V)
			{
				return Point2DIBase(fValue / ((Float)V.x), fValue / ((Float)V.y));
			}
			friend inline Point2DIBase operator / (const Point2DIBase& V, const Float& fValue)
			{
				return Point2DIBase(((Float)V.x) / fValue, ((Float)V.y) / fValue);
			}
			friend inline Point2DIBase operator+(Point2DIBase V, const Point2DIBase& rhs) {
				V += rhs;
				return V;
			}
			friend inline Point2DIBase operator-(Point2DIBase V, const Point2DIBase& rhs) {
				V -= rhs;
				return V;
			}

			// With assignement
			inline const Point2DIBase& operator *= (const Float& fValue)
			{
				x = (coordType)(((Float)x) * fValue + 0.5f);
				y = (coordType)(((Float)y) * fValue + 0.5f);
				return (*this);
			}
			inline const Point2DIBase& operator *= (const coordType& Value)
			{
				x *= Value;
				y *= Value;
				return (*this);
			}
			inline const Point2DIBase& operator /= (const Float& fValue)
			{
				x = (coordType)(((Float)x) / fValue + 0.5f);
				y = (coordType)(((Float)y) / fValue + 0.5f);
				return (*this);
			}

			inline const Point2DIBase& operator /= (const coordType& Value)
			{
				x /= Value;
				y /= Value;
				return (*this);
			}

			// +---------
			// | Euclidian operations
			// +---------
			friend inline Float Norm(const Point2DIBase& P)
			{
				return sqrtf((Float)(P.x * P.x + P.y * P.y));
			}
			friend inline int NormSquare(const Point2DIBase& P)
			{
				return (int)(P.x * P.x + P.y * P.y);
			}
			friend inline coordType Dot(const Point2DIBase& P1, const Point2DIBase& P2)
			{
				return P1.x * P2.x + P1.y * P2.y;
			}

			// +---------
			// | Acces Operators
			// +---------
			inline const coordType& operator[](size_t i) const;
			inline coordType& operator[](size_t i);

			// +---------
			// | Coordinates
			// +---------
			coordType   x;
			coordType   y;
		};

		typedef Point2DIBase<IntS32> Point2DI;
		typedef Point2DIBase<IntS16> Point2DSI;
		typedef Point2DIBase<IntU32> Point2DUI;

	}
}
