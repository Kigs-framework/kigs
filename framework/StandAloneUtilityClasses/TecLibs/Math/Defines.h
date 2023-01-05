// **********************************************************************
// * FILE  : Defines.h
// * GROUP : TecLibs/Math
// *---------------------------------------------------------------------
// * PURPOSE : Math definitions
// * COMMENT : ...
// *---------------------------------------------------------------------
// **********************************************************************
#pragma once

namespace Kigs
{
	namespace Maths
	{

		// ----------------------------------------------------------------------
		// +---------
		// | Useful definitions
		// +---------

		constexpr float 	fPI = 		3.14159265358979f;
		constexpr double 	PI = 		3.14159265358979f;
		constexpr double 	PI_2 = 		(PI/2.0);
		constexpr double 	PI_180 =	(PI/180.0);
		constexpr float 	fPI_180 =	(fPI/180.0f);


		// Other constants

		constexpr double 	SQRT2 =1.41421356237309;
		constexpr double 	EPSILON =0.000000000000001;


		// Conversion operations
		template<typename T>
		constexpr T deg2rad(const T& angle) { return (T)(angle*PI_180); }


		template<typename T>
		constexpr T sqr(const T& x) { return x*x; }


		template<typename T>
		constexpr T cube(const T& x) { return x*x*x; }

		template<typename T>
		constexpr bool isnan(const T& x) { return  ( !((x <= 0) || (x > 0)) ); }

	}
}


