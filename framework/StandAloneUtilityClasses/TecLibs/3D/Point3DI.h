// **********************************************************************
// * FILE  : Point3DIBase.h
// * GROUP : TecLibs/3D
// *---------------------------------------------------------------------
// * PURPOSE : Class for 3D Point with integer "coordType" coordinates
// * COMMENT : 
// *           
// *---------------------------------------------------------------------
// **********************************************************************
#ifndef _Point3DI_h_
#define _Point3DI_h_

#include "Point3D.h"

// ----------------------------------------------------------------------
// **************************************************
// * Point3DIBase & Point3DSI
// *-------------------------------------------------
// * - 
// * - 
// **************************************************
template<typename coordType>
struct Point3DIBase
{
    // +---------
    // | Life-cyle
    // +---------
    // Constructors
    inline Point3DIBase();
	// DO NOTHING! (For immediate creation)
    inline explicit Point3DIBase( const coordType& iValue );
	// Set all coordinates to iValue
    inline Point3DIBase( const coordType& x , const coordType& y , const coordType& z );
    inline Point3DIBase( const Point3DIBase& P );
	
    // Set (constructors-like)
    inline void Set( const coordType& iValue );
    inline void Set( const coordType& x , const coordType& y , const coordType& z );
    inline void Set( const Point3DIBase& V );
	
    // Assignement
    inline const Point3DIBase& operator =( const Point3DIBase& V );
	
    // +---------
    // | Addition/Substraction
    // +---------
    // With assignement
    inline const Point3DIBase& operator += ( const Point3DIBase& V );
    inline const Point3DIBase& operator -= ( const Point3DIBase& V );
	
    // With assignement
    inline const Point3DIBase& operator *= ( const coordType& iValue );
    inline const Point3DIBase& operator /= ( const coordType& iValue );
	inline const Point3DIBase& operator *= ( const Point3DIBase& p);
	inline const Point3DIBase& operator /= ( const Point3DIBase& p);
	
	inline operator Point3D() const
	{
		Point3D result((Float)x, (Float)y, (Float)z);
		return result;
	}

    // +---------
    // | Utilities
    // +---------
    inline void Normalize( void );
	
	// +---------
	// | Acces Operators
	// +---------
	inline const coordType& operator[](Int i) const;
	inline coordType& operator[](Int i);
	
	bool operator==(const Point3DIBase& Other) const
	{
		return (x == Other.x && y ==Other.y && z ==Other.z);
	}
	
    // +---------
    // | Coordinates
    // +---------
	union
	{
		struct
		{
			coordType   x;
			coordType   y;
			coordType   z;
		};
		
		struct 
		{
			Point2DIBase<coordType> xy;
			coordType __Ignored1;
		};
		
		struct 
		{
			coordType __Ignored2;
			Point2DIBase<coordType> yz;
		};
		
		coordType data[3];
	};
    
};

typedef Point3DIBase<IntS32> Point3DI;
typedef Point3DIBase<IntS16> Point3DSI;
typedef Point3DIBase<IntU32> Point3DUI;

// Unary
template<typename coordType>
inline Point3DIBase<coordType> operator - (const Point3DIBase<coordType>& V);
// +---------
// | Euclidian operations
// +---------
template<typename coordType>
inline coordType Norm(const Point3DIBase<coordType>& P);
template<typename coordType>
inline coordType NormSquare(const Point3DIBase<coordType>& P);
template<typename coordType>
inline coordType Dist(const Point3DIBase<coordType>& P1, const Point3DIBase<coordType>& P2);
template<typename coordType>
inline coordType DistSquare(const Point3DIBase<coordType>& P1, const Point3DIBase<coordType>& P2);
template<typename coordType>
inline coordType SegmentDist(const Point3DIBase<coordType>& Pt1, const Point3DIBase<coordType>& Pt2, const Point3DIBase<coordType>& Pt3, const Point3DIBase<coordType>& Pt4);
template<typename coordType>
inline coordType PointToSegmentDist(const Point3DIBase<coordType>& Pt, const Point3DIBase<coordType>& Pt1, const Point3DIBase<coordType>& Pt2, Point3DIBase<coordType>& nearest, bool& insideSegment);
// +---------
// | Multiplication/Division
// +---------
// With a scalar (External laws)
template<typename coordType>
inline Point3DIBase<coordType> operator * (const coordType& iValue, const Point3DIBase<coordType>& V);
template<typename coordType>
inline Point3DIBase<coordType> operator * (const Point3DIBase<coordType>& V, const coordType& iValue);
template<typename coordType>
inline Point3DIBase<coordType> operator / (const coordType& iValue, const Point3DIBase<coordType>& V);
template<typename coordType>
inline Point3DIBase<coordType> operator / (const Point3DIBase<coordType>& V, const coordType& iValue);



#endif //_Point3DI_h_


