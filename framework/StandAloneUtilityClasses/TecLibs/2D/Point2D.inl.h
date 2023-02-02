// **********************************************************************
// * FILE  : Point2D.cpp
// * GROUP : TecLibs, section Math
// *---------------------------------------------------------------------
// * PURPOSE : Implementation of Point2DBase
// *---------------------------------------------------------------------
// **********************************************************************

namespace Kigs
{
	namespace Maths
	{

		// ----------------------------------------------------------------------
		// +---------
		// | Life-cycle
		// +---------
		// Constructors
		template<typename floatType>
		Point2DBase<floatType>::Point2DBase()
		{
			;
		}

		template<typename floatType>
		Point2DBase<floatType>::Point2DBase(const floatType& x, const floatType& y)
			:x(x), y(y)
		{
			;
		}

		template<typename floatType>
		Point2DBase<floatType>::Point2DBase(const Point2DBase<floatType>& P)
			:x(P.x), y(P.y)
		{
			;
		}

		// Set (constructors-like)
		template<typename floatType>
		void Point2DBase<floatType>::Set(const floatType& fValue)
		{
			x = y = fValue;
		}

		template<typename floatType>
		void Point2DBase<floatType>::Set(const floatType& x, const floatType& y)
		{
			this->x = x;
			this->y = y;
		}

		template<typename floatType>
		void Point2DBase<floatType>::Set(const Point2DBase<floatType>& P)
		{
			x = P.x;
			y = P.y;
		}

		template<typename floatType>
		void Point2DBase<floatType>::Set(const Vector3D& P)
		{
			x = P.x;
			y = P.y;
		}

		// Assignement
		template<typename floatType>
		const Point2DBase<floatType>& Point2DBase<floatType>::operator =(const Point2DBase<floatType>& V)
		{
			x = V.x;
			y = V.y;

			return *this;
		}
		// Assignement
		template<typename floatType>
		const Point2DBase<floatType>& Point2DBase<floatType>::operator =(const floatType& V)
		{
			x = V;
			y = V;

			return *this;
		}


		// ----------------------------------------------------------------------
		// +---------
		// | Addition/Substraction
		// +---------
		// With another vector (Internal laws)

		// With assignement
		template<typename floatType>
		const Point2DBase<floatType>& Point2DBase<floatType>::operator += (const Point2DBase<floatType>& V)
		{
			x += V.x;
			y += V.y;

			return *this;
		}
		template<typename floatType>
		const Point2DBase<floatType>& Point2DBase<floatType>::operator -= (const Point2DBase<floatType>& V)
		{
			x -= V.x;
			y -= V.y;

			return *this;
		}



		// With another Point2DBase

		template<typename floatType>
		Point2DBase<floatType> Point2DBase<floatType>::operator - (const Point2DBase<floatType>& V) const
		{
			return Point2DBase(x - V.x, y - V.y);
		}

		template<typename floatType>
		Point2DBase<floatType> Point2DBase<floatType>::operator + (const Point2DBase<floatType>& V) const
		{
			return Point2DBase(x + V.x, y + V.y);
		}



		// With assignement
		template<typename floatType>
		const Point2DBase<floatType>& Point2DBase<floatType>::operator *= (const floatType& fValue)
		{
			x *= fValue;
			y *= fValue;

			return *this;
		}

		template<typename floatType>
		const Point2DBase<floatType>& Point2DBase<floatType>::operator /= (const floatType& fValue)
		{
			const floatType invfValue = floatType(1.0) / fValue;
			x *= invfValue;
			y *= invfValue;

			return *this;
		}

		template<typename floatType>
		const Point2DBase<floatType>& Point2DBase<floatType>::operator *= (const Point2DBase<floatType>& p)
		{
			x *= p.x;
			y *= p.y;

			return *this;
		}

		template<typename floatType>
		const Point2DBase<floatType>& Point2DBase<floatType>::operator /= (const Point2DBase<floatType>& p)
		{
			x /= p.x;
			y /= p.y;

			return *this;
		}

		template<typename floatType>
		void Point2DBase<floatType>::Normalize(void)
		{
			floatType tmp = Norm(*this);
			if (tmp != floatType(0.0))
			{
				tmp = floatType(1.0) / tmp;
				x *= tmp;
				y *= tmp;
			}
		}

		template<typename floatType>
		Point2DBase<floatType> Point2DBase<floatType>::Normalized(void) const
		{
			Point2DBase<floatType> result = *this;
			result.Normalize();
			return result;
		}


		// +---------
		// | Acces Operators
		// +---------
		template<typename floatType>
		const floatType& Point2DBase<floatType>::operator[](size_t i) const
		{
			assert(i >= 0 && i < 2);
			return *((&x) + i);
		}
		template<typename floatType>
		floatType& Point2DBase<floatType>::operator[](size_t i)
		{
			assert(i >= 0 && i < 2);
			return *((&x) + i);
		}

		template<typename floatType>
		void Point2DBase<floatType>::ClampMax(const floatType v)
		{
			if (x > v)
				x = v;
			if (y > v)
				y = v;
		}
		template<typename floatType>
		void Point2DBase<floatType>::ClampMin(const floatType v)
		{
			if (x < v)
				x = v;
			if (y < v)
				y = v;
		}

#define POINT2D_FRIEND_DECLARATION(ptype,ftype) \
ptype operator - (const ptype& V)\
{\
	return ptype(-V.x, -V.y);\
}\
ptype operator * (const ftype& fValue, const ptype& V)\
{\
	return ptype(fValue * V.x, fValue * V.y);\
}\
\
ptype operator * (const ptype& V, const ftype& fValue)\
{\
	return ptype(V.x * fValue, V.y * fValue);\
}\
\
ptype operator * (const ptype& V, const ptype& fValue)\
{\
	return ptype(V.x * fValue.x, V.y * fValue.y);\
}\
\
ptype operator / (const ftype& fValue, const ptype& V)\
{\
	return ptype(fValue / V.x, fValue / V.y);\
}\
\
ptype operator / (const ptype& V, const ftype& fValue)\
{\
	return ptype(V.x / fValue, V.y / fValue);\
}\
\
ftype Norm( const ptype& P )\
{\
	return (ftype)sqrt( sqr(P.x) + sqr(P.y) );\
}\
\
ftype NormSquare( const ptype& P )\
{\
    return sqr(P.x) + sqr(P.y) ;\
}\
\
ftype Dist( const ptype& P1, const ptype& P2 )\
{\
	const ftype dx = P2.x - P1.x;\
	const ftype dy = P2.y - P1.y;\
	return (ftype)sqrt(sqr(dx) + sqr(dy));\
}\
\
ftype Dot( const ptype& U , const ptype& V )\
{\
	return U.x*V.x+U.y*V.y;\
}\
\
ftype Cross( const ptype& V, const ptype& W )\
{\
    return (V.x * W.y) - (V.y * W.x);\
}\
\
ftype DistSquare( const ptype& P1, const ptype& P2 )\
{\
	const ftype dx = P2.x - P1.x;\
	const ftype dy = P2.y - P1.y;\
	return sqr(dx) + sqr(dy);\
}\
\
ptype Mid( const ptype& P , const ptype& Q )\
{\
    return ptype( (P.x + Q.x) * ftype(0.5),\
		(P.y + Q.y) * ftype(0.5) );\
}\
\
ptype Bary( const ftype& a , const ptype& P , const ftype& b , const ptype& Q )\
{\
    return ptype( ((a * P.x) + (b * Q.x)) ,	((a * P.y) + (b * Q.y)) );\
}\
\
inline bool Intersect(const ptype& PS1 , const ptype& PE1,const ptype& PS2 , const ptype& PE2,ptype& result)\
{\
	ftype denom=(PE2.y-PS2.y)*(PE1.x-PS1.x)-(PE2.x-PS2.x)*(PE1.y-PS1.y);\
\
	if((denom<ftype(-0.01))||(denom>ftype(0.01)))\
	{\
		ftype ua=((PE2.x-PS2.x)*(PS1.y-PS2.y)-(PE2.y-PS2.y)*(PS1.x-PS2.x))/denom;\
		result.x=PS1.x+ua*(PE1.x-PS1.x);\
		result.y=PS1.y+ua*(PE1.y-PS1.y);\
		return true;\
	}\
\
	return false;\
}\
\
inline bool SegmentIntersect(const ptype& PS1, const ptype& PE1, const ptype& PS2, const ptype& PE2, ptype& result)\
{\
	ftype denom = (PE2.y - PS2.y)*(PE1.x - PS1.x) - (PE2.x - PS2.x)*(PE1.y - PS1.y);\
\
	if ((denom<ftype(-0.01)) || (denom>ftype(0.01)))\
	{\
		ftype ua = ((PE2.x - PS2.x)*(PS1.y - PS2.y) - (PE2.y - PS2.y)*(PS1.x - PS2.x)) / denom;\
		if ((ua >= 0.0) && (ua <= 1.0))\
		{\
			ftype ub = ((PE1.x - PS1.x)*(PS1.y - PS2.y) - (PE1.y - PS1.y)*(PS1.x - PS2.x)) / denom;\
			if ((ub >= 0.0) && (ub <= 1.0))\
			{\
				result.x = PS1.x + ua*(PE1.x - PS1.x);\
				result.y = PS1.y + ua*(PE1.y - PS1.y);\
				return true;\
			}\
		}\
\
	}\
\
	return false;\
}\
inline bool SegmentLineIntersect(const ptype& PS1, const ptype& PE1, const ptype& PS2, const ptype& PE2, ptype& result)\
{\
	ftype denom = (PE2.y - PS2.y)*(PE1.x - PS1.x) - (PE2.x - PS2.x)*(PE1.y - PS1.y);\
\
	if ((denom<ftype(-0.01)) || (denom>ftype(0.01)))\
	{\
		ftype ua = ((PE2.x - PS2.x)*(PS1.y - PS2.y) - (PE2.y - PS2.y)*(PS1.x - PS2.x)) / denom;\
		if ((ua >= 0.0) && (ua <= 1.0))\
		{\
			result.x = PS1.x + ua*(PE1.x - PS1.x);\
			result.y = PS1.y + ua*(PE1.y - PS1.y);\
			return true;\
		}\
\
	}\
\
	return false;\
}\
\
inline bool SegmentStrictIntersect(const ptype& PS1, const ptype& PE1, const ptype& PS2, const ptype& PE2, ptype& result,ftype disterror=0.0)\
{\
	ftype denom = (PE2.y - PS2.y)*(PE1.x - PS1.x) - (PE2.x - PS2.x)*(PE1.y - PS1.y);\
\
	if ((denom<ftype(-0.01)) || (denom>ftype(0.01)))\
	{\
		ftype ua = ((PE2.x - PS2.x)*(PS1.y - PS2.y) - (PE2.y - PS2.y)*(PS1.x - PS2.x)) / denom;\
		if ((ua > disterror) && (ua < (1.0- disterror)))\
		{\
			ftype ub = ((PE1.x - PS1.x)*(PS1.y - PS2.y) - (PE1.y - PS1.y)*(PS1.x - PS2.x)) / denom;\
			if ((ub > disterror) && (ub < (1.0 - disterror)))\
			{\
				result.x = PS1.x + ua*(PE1.x - PS1.x);\
				result.y = PS1.y + ua*(PE1.y - PS1.y);\
				return true;\
			}\
		}\
\
	}\
\
	return false;\
}\
\
inline ptype Lerp( const ptype& P , const ptype& Q, const ftype& t )\
{\
	ftype s = ftype(1.0) - t;\
    return ptype( (s * P.x) + (t * Q.x),\
		(s * P.y) + (t * Q.y));\
}



		POINT2D_FRIEND_DECLARATION(Point2D, float)
			POINT2D_FRIEND_DECLARATION(Point2D_d, double)

	}
}