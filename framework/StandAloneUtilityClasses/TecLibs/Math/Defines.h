// **********************************************************************
// * FILE  : Defines.h
// * GROUP : TecLibs/Math
// *---------------------------------------------------------------------
// * PURPOSE : Math definitions
// * COMMENT : ...
// *---------------------------------------------------------------------
// **********************************************************************
#ifndef _Defines_h_
#define _Defines_h_



// ----------------------------------------------------------------------
// +---------
// | Useful definitions
// +---------

// PI constants (<math.h> notation: '_'=divide)
#ifndef fPI
#define fPI 3.14159265358979f
#endif
#ifndef PI
#define PI          3.14159265358979
#endif
#ifndef PI_2
#define PI_2        (PI/2.0)
#endif
#ifndef PI_180
#define PI_180      0.01745329251994
#endif
#ifndef fPI_180
#define fPI_180      0.01745329251994f
#endif

// Other constants
#ifndef SQRT2
#define SQRT2       1.41421356237309
#endif
#ifndef EPSILON
#define EPSILON     0.000000000000001
#endif


// Conversion operations
#ifndef deg2rad
#define deg2rad(Angle)      ((Angle)*PI_180)
#endif

// Sqr & cube operations
/*#ifndef sqr
#define sqr(x)              ((x)*(x))
#endif*/

template<typename T>
constexpr T sqr(const T& x) { return x*x; }

/*
#ifndef cube
#define cube(x)             ((x)*(x)*(x))
#endif
*/

template<typename T>
constexpr T cube(const T& x) { return x*x*x; }

#ifndef isnan
#define isnan(x) ( !((x <= 0) || (x > 0)) )
#endif

#endif//_Defines_h_


