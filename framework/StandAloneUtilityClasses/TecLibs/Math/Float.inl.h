// **********************************************************************
// * FILE  : Float.inl
// * GROUP : TecLibs/Math
// *---------------------------------------------------------------------
// * PURPOSE : Common Float operations
// * COMMENT : This unit serves to unify the notation for all standard
// *		   mathematical operations, whatever you defined the Float
// *		   type was (only float or double at the current time)
// **********************************************************************

// ----------------------------------------------------------------------
// +---------
// | Platform test
// +---------
#ifndef _ANSI
#error ANSI C Standard librairies are needed (Cf. in Math/Float.h)
#endif



// ----------------------------------------------------------------------
// +---------
// | Local includes
// +---------
#include <math.h>

#ifdef TEC_USE_FLOAT

// float overloads

Float cosF( const Float& fValue )
{
	return cosf( fValue );
}

Float sinF( const Float& fValue )
{
	return sinf( fValue );
}

Float tanF( const Float& fValue )
{
	return tanf( fValue );
}

Float acosF( const Float& fValue )
{
	return acosf( fValue );
}

Float asinF( const Float& fValue )
{
	return asinf( fValue );
}

Float atanF( const Float& fValue )
{
	return atanf( fValue );
}

Float atan2F( const Float& fValueX, const Float& fValueY )
{
	return atan2f( fValueX, fValueY );
}

Float absF( const Float& fValue )
{
	return  fabsf( fValue );
}

Float sqrtF( const Float& fValue )
{
	return sqrtf( fValue );
}

Float logF( const Float& fValue)
{
	return logf(fValue);
}

Float expF( const Float& fValue)
{
	return expf(fValue);
}

Float log10F( const Float& fValue)
{
	return log10f(fValue);
}

Float powF( const Float& fValue, const Float& fExp)
{
	return powf(fValue, fExp);
}

#else


Float absF( const Float& dValue )
{
	return abs(dValue);
}

// float overloads

Float cosF( const Float& fValue )
{
	return cos( fValue );
}

Float sinF( const Float& fValue )
{
	return sin( fValue );
}

Float tanF( const Float& fValue )
{
	return tan( fValue );
}

Float acosF( const Float& fValue )
{
	return acos( fValue );
}

Float asinF( const Float& fValue )
{
	return asin( fValue );
}

Float atanF( const Float& fValue )
{
	return atan( fValue );
}

Float atan2F( const Float& fValueX, const Float& fValueY )
{
	return atan2( fValueX, fValueY );
}

Float sqrtF( const Float& fValue )
{
	return sqrt( fValue );
}

Float logF( const Float& fValue)
{
	return log(fValue);
}

Float expF( const Float& fValue)
{
	return exp(fValue);
}

Float log10F( const Float& fValue)
{
	return log10(fValue);
}

Float powF( const Float& fValue, const Float& fExp)
{
	return pow(fValue, fExp);
}

#endif


