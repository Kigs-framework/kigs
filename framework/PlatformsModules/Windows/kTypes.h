#ifndef _KTYPES_H_
#define _KTYPES_H_

#include <float.h>

// define base types that can be changed on a per platform basis 

inline float CastToFloat(const float &f) {return (float)f;}
inline double CastToDouble(const double &f) {return (double)f;}

#endif //_KTYPES_H_
