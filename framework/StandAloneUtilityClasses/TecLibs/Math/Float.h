// **********************************************************************
// * FILE  : Float.h
// * GROUP : TecLibs/Math
// *---------------------------------------------------------------------
// * PURPOSE : Common Float operations
// * COMMENT : This unit serves to unify the notation for all standard
// *		   mathematical operations, whatever you defined the Float
// *		   type was (only float or double at the current time)
// **********************************************************************

#ifndef _Float_h_
#define _Float_h_

// ----------------------------------------------------------------------
// +---------
// | Common includes
// +---------
#include "TecLibs/TecLibs.h"

inline Float absF( const Float& );

// floats overloads
inline Float cosF( const Float& );
inline Float sinF( const Float& );
inline Float tanF( const Float& );

inline Float acosF( const Float& );
inline Float asinF( const Float& );
inline Float atanF( const Float& );
inline Float atan2F( const Float&, const Float& );

inline Float absF( const Float& );
inline Float sqrtF( const Float& );

inline Float expF( const Float& );
inline Float logF( const Float& );
inline Float log10F( const Float& );

inline Float powF( const Float&, const Float& );


#endif//_Float_h_

