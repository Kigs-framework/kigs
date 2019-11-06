// **********************************************************************
// * FILE  : TecLibs.h
// * GROUP : TecLibs, base definitions
// *---------------------------------------------------------------------
// **********************************************************************
#ifndef	_TecLibs_h_
#define _TecLibs_h_


// ----------------------------------------------------------------------
// +---------
// | Supported definitions
// +---------

// Default definitions
#define _ANY        // We suppose that C++ language is defined!!
#define _ANSI       // We suppose that ANSI C Standard libraries are defined (most cases)


// ----------------------------------------------------------------------
// +---------
// | Common includes
// +---------

#include <assert.h> // To use assert()

// ----------------------------------------------------------------------
// +---------
// | Common macros
// +---------

// Min & max operations
#ifndef MIN
#define MIN(x,y)            ((x)<(y)?(x):(y))
#endif
#ifndef MAX
#define MAX(x,y)            ((x)<(y)?(y):(x))
#endif

// ----------------------------------------------------------------------
// +---------
// | Fast & Sized types
// +---------


#if 0

// Fast integer
// -----User definition-----Sytem dependent type
typedef int                 Int;
typedef unsigned int		UInt;

// Sized integers
// -----User definition-----Type--------Bits count-----Signed------Range of value
typedef signed char         IntS8;   // 8              Yes         -128 to 127
typedef unsigned char       IntU8;   // 8              No          0 to 255
typedef signed short        IntS16;  // 16             Yes         -32768 to 32767
typedef unsigned short      IntU16;  // 16             No          0 to 65535
typedef long                IntS32;  // 32             Yes         -2147483648 to 2147483647
typedef unsigned long       IntU32;  // 32             No          0 to 4294967295
#ifdef WIN32
typedef __int64             IntS64;  // 64             Yes         -9223372036854775808 to 9223372036854775807
typedef unsigned __int64    IntU64;  // 64             No          0 to 18446744073709551615
#else
typedef long long           IntS64;  // 64             Yes         -9223372036854775808 to 9223372036854775807  
typedef unsigned long long  IntU64;  // 64             No          0 to 18446744073709551615
#endif



// Fast float
// -----User definition-----Sytem dependent type
// default type is float. Use a project preprocessor define to use anything else
// to allow easy switching between types, all standard math.h methods are redefined
// in Float.h when needed with the same naming conventions.
// This concerns: cos, sin, tan, acos, asin, atan, abs, sqrt, exp, log  
#ifndef TEC_USE_DOUBLE
#ifndef TEC_USE_FLOAT
#define TEC_USE_FLOAT
#endif
#endif
#ifdef TEC_USE_FLOAT
typedef float           Float;
const   Float		    Float_Max = 3.4e38f;
const   Float           Float_Min = 1.2e-38f;
#endif
#ifdef TEC_USE_DOUBLE
typedef double          Float;
const	Float			Float_Max = 1.7e308;
const	Float			Float_Min = 2.3e-308;
#endif

// Sized floats
// -----User definition-----Type--------Bits count-----Signed------Range of value
typedef float               Float32; // 32             Yes         +/-3.4E +/-  38  (precision 7 digits)
typedef double              Float64; // 64             Yes         +/-1.7E +/- 308  (precision 15 digits)
typedef double              Float64; // 64             Yes         +/-1.7E +/- 308  (precision 15 digits)
#else

#include <cstdint>
#include <cfloat>

#ifndef TEC_USE_FLOAT
#define TEC_USE_FLOAT
#endif

// TecLib backward compatibility
using IntS8 = int8_t;
using IntU8 = uint8_t;

using IntS16 = int16_t;
using IntU16 = uint16_t;

using IntS32 = int32_t;
using IntU32 = uint32_t;

using IntS64 = int64_t;
using IntU64 = uint64_t;

using Float = float;
const Float Float_Max = FLT_MAX;
const Float Float_Min = FLT_MIN;

using Int = IntS32;
using UInt = IntU32;

using Float32 = float;
using Float64 = double;


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

#endif
#ifndef KFLOAT_CONST
#define KFLOAT_CONST(a) (a)
#endif

#ifndef KFLOAT_ZERO
#define KFLOAT_ZERO 0.0f
#endif

#ifndef KFLOAT_ONE
#define KFLOAT_ONE 1.0f
#endif

#ifndef KFLOAT_CONST_MIN
#define KFLOAT_CONST_MIN (-FLT_MAX)
#endif

#ifndef KFLOAT_CONST_MAX
#define KFLOAT_CONST_MAX FLT_MAX
#endif

#ifndef KFLOAT_CONST_PI 
#define KFLOAT_CONST_PI (3.141592653589f)
#endif


// Sized flags
typedef IntU8  Flag8;
typedef IntU16 Flag16;
typedef IntU32 Flag32;
typedef IntU64 Flag64;

// useful struct for Point2D / Point3D constructors
struct asVector
{};

struct asMiddle
{
};

#endif //_TecLibs_h_


