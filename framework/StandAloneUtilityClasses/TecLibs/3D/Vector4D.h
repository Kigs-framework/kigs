// **********************************************************************
// * FILE  : Vector4D.h
// * GROUP : TecLibs/3D
// *---------------------------------------------------------------------
// * PURPOSE : Class for 4D Vector (projective geometry)
// * COMMENT : 
// **********************************************************************
#ifndef _Vector4D_h_
#define _Vector4D_h_



// ----------------------------------------------------------------------
// **************************************************
// * Vector4D
// *-------------------------------------------------
// * - Definition of 4D vector
// * - w=0 => vector 3D, w=1 => point 3D
// **************************************************
struct Vector4D
{
    // +---------
    // | Life-cyle
    // +---------
    // Constructors
    inline Vector4D();
	// DO NOTHING! (For immediate creation)
    inline explicit Vector4D( const Float& fValue );
	// Set all coordinates to fValue
    inline Vector4D( const Float& x , const Float& y , const Float& z , const Float& w );
    inline Vector4D( const Vector4D& V );
    inline explicit Vector4D( const Vector3D& V );
    inline explicit Vector4D( const Point3D& P );
	// Convert 3D point or vector to 4D vector (w=0 for vector, and w=1 for point)
	
    // Set (constructors-like)
    inline void Set( const Float& fValue );
    inline void Set( const Float& x , const Float& y , const Float& z , const Float& w );
    inline void Set( const Vector4D& V );
    inline void Set( const Point3D& P );
    inline void Set( const Vector3D& V );
	
    // Assignement
    inline const Vector4D& operator =( const Vector4D& V );
	
    // Conversions
    inline explicit operator const Point3D ( void );
    inline explicit operator const Vector3D ( void );
	
	inline const Float& operator[](Int i) const;
	inline Float& operator[](Int i);

    // +---------
    // | Addition/Substraction
    // +---------
    // With another vector (Internal laws)
    friend inline Vector4D operator + ( const Vector4D& U , const Vector4D& V );
    friend inline Vector4D operator - ( const Vector4D& U , const Vector4D& V );
	
    // With assignement
    inline const Vector4D& operator += ( const Vector4D& V );
    inline const Vector4D& operator -= ( const Vector4D& V );
	
    // Unary
    friend inline Vector4D operator - ( const Vector4D& U );
	
    
    // +---------
    // | Multiplication/Division
    // +---------
    // With a scalar (External laws)
    friend inline Vector4D operator * ( const Float& fValue , const Vector4D& V );
    friend inline Vector4D operator * ( const Vector4D& V , const Float& fValue );
    friend inline Vector4D operator / ( const Float& fValue , const Vector4D& V );
    friend inline Vector4D operator / ( const Vector4D& V , const Float& fValue );
	
    // With assignement
    inline const Vector4D& operator *= ( const Float& fValue );
	inline const Vector4D& operator *= (const Vector4D& fValue);
    inline const Vector4D& operator /= ( const Float& fValue );
	inline const Vector4D& operator /= (const Vector4D& other);
	
	
	//// ADD LLC: Peut-etre un jour?
	//     // +---------
	//     // | Cross product
	//     // +---------
	//     friend inline const Vector4D& operator ^ ( const Vector4D& U , const Vector4D& V );
	// 
	//     // With assignement
	//     inline const Vector4D& operator ^= ( const Vector4D& V );
	// 
	// 
	//// END
	
    // +---------
    // | Euclidian operations
    // +---------
    friend inline Float Dot( const Vector4D& U , const Vector4D& V );
    friend inline Float Norm( const Vector4D& V );
    friend inline Float NormSquare( const Vector4D& V );
	
    // +---------
    // | Affine operations
    // +---------
    friend inline Vector4D Mid( const Vector4D& U , const Vector4D& V );
    friend inline Vector4D Bary( const Float& a , const Vector4D& U , const Float& b , const Vector4D& V );
	
	
    // +---------
    // | Utilities
    // +---------
    inline void Normalize( void );
	inline Vector4D Normalized(void);
	
	
	friend inline bool operator==(const Vector4D& U, const Vector4D& V);
	friend inline bool operator!=(const Vector4D& U, const Vector4D& V);
	
	
    // +---------
    // | Coordinates
    // +---------
	
	
	union
	{
		struct
		{
			union
			{
				Float x;
				Float r;
			};
			
			union
			{
				Float y;
				Float g;
			};
			
			union
			{
				Float z;
				Float b;
			};
			
			union
			{
				Float w;
				Float a;
			};
		};
		
		struct
		{
			Point3D xyz;
			Float __Ignored1;
		};
		
		struct
		{
			Float __Ignored2;
			Point3D yzw;
		};

		Float data[4];
		
	};
	
};

#endif //_Vector4D_h_


