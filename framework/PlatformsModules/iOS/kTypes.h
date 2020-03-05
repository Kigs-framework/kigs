#ifndef _KTYPES_H_
#define _KTYPES_H_

#include <float.h>

// define base types that can be changed on a per platform basis 

typedef double	kdouble;
typedef float	kfloat;

#define KDOUBLE_CONST(a) (a)
#define KFLOAT_CONST(a) (a)
#define KFLOAT_ZERO		(0.0f)
#define KFLOAT_ONE		(1.0f)
#define KFLOAT_CONST_MIN (-FLT_MAX)
#define KFLOAT_CONST_MAX FLT_MAX
#define KFLOAT_CONST_PI (3.141592653589f)


inline float CastToFloat(const kfloat &f) {return (float)f;}
inline double CastToDouble(const kdouble &f) {return (double)f;}

#endif //_KTYPES_H_
