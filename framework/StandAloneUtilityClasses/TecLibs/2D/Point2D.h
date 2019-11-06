// **********************************************************************
// * FILE  : Point2D.h
// * GROUP : TecLibs/2D
// *---------------------------------------------------------------------
// * PURPOSE : Class for 2D Point
// * COMMENT : 
// *---------------------------------------------------------------------
// **********************************************************************
#ifndef _Point2D_h_
#define _Point2D_h_

struct Vector3D;

// ----------------------------------------------------------------------
// **************************************************
// * Point2DBase
// *-------------------------------------------------
// * - Definition of 3D vector
// * - 
// **************************************************


template<typename floatType>
struct Point2DBase
{
    // +---------
    // | Life-cyle
    // +---------
    // Constructors
    inline Point2DBase(); // DO NOTHING! (For immediate creation)
	
	// don't use one float constructor as it introduce cast ambiguity
	/*inline explicit Point2DBase(const floatType& fValue)
	{
	 x = y = fValue;
	}*/
	// Set all coordinates to fValue
    inline Point2DBase( const floatType& x , const floatType& y  );
    inline Point2DBase( const Point2DBase& P );
	
	// construct middle
	inline Point2DBase(const Point2DBase& V1,const Point2DBase& V2, const asMiddle& m)
	{
		x=V1.x+V2.x;
		y=V1.y+V2.y;
		x*=KFLOAT_CONST(0.5f);	   	y*=KFLOAT_CONST(0.5f);	
	}

	inline Point2DBase(const Point2DBase& V1, const Point2DBase& V2,const asVector& v)
	{
		x = V2.x - V1.x;
		y = V2.y - V1.y;
	}
    
    // Set (constructors-like)
    inline void Set( const floatType& fValue );
    inline void Set( const floatType& x , const floatType& y );
    inline void Set( const Point2DBase& V );
	inline void Set(const Vector3D& P);
	
    // Assignement
    inline const Point2DBase& operator =( const Point2DBase& V );
	
	inline const Point2DBase& operator =(const floatType& V);
	
	// equality
	bool operator==(const Point2DBase& Other) const
	{
		return (x == Other.x && y ==Other.y);
	}
	
	bool operator!=(const Point2DBase& Other) const
	{
		return (x != Other.x || y != Other.y);
	}
	
	bool operator>(const Point2DBase& Other) const
	{
		return (x > Other.x && y > Other.y);
	}
	
	bool operator<(const Point2DBase& Other) const
	{
		return (x < Other.x && y < Other.y);
	}
	bool operator>=(const Point2DBase& Other) const
	{
		return (x >= Other.x && y >= Other.y);
	}
	
	bool operator<=(const Point2DBase& Other) const
	{
		return (x <= Other.x && y <= Other.y);
	}    
	// +---------
    // | Addition/Substraction
    // +---------
	
    // With assignement
    inline const Point2DBase& operator += ( const Point2DBase& V );
    inline const Point2DBase& operator -= ( const Point2DBase& V );
	

	
	// With another Point2DBase
	inline Point2DBase operator - ( const Point2DBase& V ) const;
	inline Point2DBase operator + ( const Point2DBase& V ) const;
	
    
    // +---------
    // | Multiplication/Division
    // +---------
  

	
    // With assignement
    inline const Point2DBase& operator *= ( const floatType& fValue );
    inline const Point2DBase& operator /= ( const floatType& fValue );
	inline const Point2DBase& operator *= ( const Point2DBase& p);
	inline const Point2DBase& operator /= ( const Point2DBase& p);
	
	
	friend inline Point2DBase operator - (const Point2DBase& V);
	friend inline Point2DBase operator * (const floatType& fValue, const Point2DBase& V);
	friend inline Point2DBase operator * (const Point2DBase& V, const floatType& fValue);
	friend inline Point2DBase operator * (const Point2DBase& V, const Point2DBase& fValue);
	friend inline Point2DBase operator / (const floatType& fValue, const Point2DBase& V);
	friend inline Point2DBase operator / (const Point2DBase& V, const floatType& fValue);
    // +---------
    // | Euclidian operations
    // +---------

    friend inline floatType Norm( const Point2DBase& P );
    friend inline floatType NormSquare( const Point2DBase& P );
	friend inline floatType Dist( const Point2DBase& P1, const Point2DBase& P2 );
	friend inline floatType DistSquare( const Point2DBase& P1, const Point2DBase& P2 );
	
	friend inline floatType Dot( const Point2DBase& U , const Point2DBase& V );
	friend inline floatType Cross( const Point2DBase& V, const Point2DBase& W );
	
    // +---------
    // | Affine operations
    // +---------
    friend inline Point2DBase Mid( const Point2DBase& P , const Point2DBase& Q );
    friend inline Point2DBase Bary( const floatType& a , const Point2DBase& P , const floatType& b , const Point2DBase& Q );
	
    friend inline Point2DBase Lerp( const Point2DBase& P , const Point2DBase& Q, const floatType& t );
	// Blend is the same as Lerp, but Blend is available and as the same prototype for Vector3D, Point2DBase, Quaternion... so 
	// it can be called in template methods
	friend inline Point2DBase Blend(const Point2DBase& U , const Point2DBase& V,floatType t) 
	{
		return Lerp(U,V,t);
	}
	

	friend inline bool Intersect(const Point2DBase& PS1 , const Point2DBase& PE1,const Point2DBase& PS2 , const Point2DBase& PE2,Point2DBase& result);
	
	
    // +---------
    // | Utilities
    // +---------
	inline void Normalize(void);
	inline Point2DBase Normalized( void ) const;
	inline void ClampMax(const floatType v);
	inline void ClampMin(const floatType v);
	
	// +---------
	// | Acces Operators
	// +---------
	inline const floatType& operator[](Int i) const;
	inline floatType& operator[](Int i);
	
    // +---------
    // | Coordinates
    // +---------
    floatType   x;
    floatType   y;
};

typedef Point2DBase<float> Point2D;
template struct Point2DBase<float>;

typedef Point2DBase<double> Point2D_d;
template struct Point2DBase<double>;

#endif //_Point2D_h_


