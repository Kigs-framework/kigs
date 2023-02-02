// **********************************************************************
// * FILE  : Point3DI.inl
// * GROUP : TecLibs, section Math
// *---------------------------------------------------------------------
// * PURPOSE : Implementation of Point3DIBase
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
        template<typename coordType>
        Point3DIBase<coordType>::Point3DIBase()
        {
            ;
        }

        template<typename coordType>
        Point3DIBase<coordType>::Point3DIBase(const coordType& iValue)
        {
            x = y = z = iValue;
        }

        template<typename coordType>
        Point3DIBase<coordType>::Point3DIBase(const coordType& x, const coordType& y, const coordType& z)
            :x(x), y(y), z(z)
        {
            ;
        }

        template<typename coordType>
        Point3DIBase<coordType>::Point3DIBase(const Point3DIBase<coordType>& P)
            :x(P.x), y(P.y), z(P.z)
        {
            ;
        }

        // Set (constructors-like)
        template<typename coordType>
        void Point3DIBase<coordType>::Set(const coordType& iValue)
        {
            x = y = z = iValue;
        }

        template<typename coordType>
        void Point3DIBase<coordType>::Set(const coordType& x, const coordType& y, const coordType& z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        template<typename coordType>
        void Point3DIBase<coordType>::Set(const Point3DIBase<coordType>& P)
        {
            x = P.x;
            y = P.y;
            z = P.z;
        }

        // Assignement
        template<typename coordType>
        const Point3DIBase<coordType>& Point3DIBase<coordType>::operator =(const Point3DIBase<coordType>& V)
        {
            x = V.x;
            y = V.y;
            z = V.z;

            return *this;
        }

        // ----------------------------------------------------------------------
        // +---------
        // | Addition/Substraction
        // +---------

        // With assignement
        template<typename coordType>
        const Point3DIBase<coordType>& Point3DIBase<coordType>::operator += (const Point3DIBase<coordType>& V)
        {
            x += V.x;
            y += V.y;
            z += V.z;

            return *this;
        }
        template<typename coordType>
        const Point3DIBase<coordType>& Point3DIBase<coordType>::operator -= (const Point3DIBase<coordType>& V)
        {
            x -= V.x;
            y -= V.y;
            z -= V.z;

            return *this;
        }

        // Unary
        template<typename coordType>
        Point3DIBase<coordType> operator - (const Point3DIBase<coordType>& V)
        {
            return Point3DIBase<coordType>(-V.x, -V.y, -V.z);
        }


        // ----------------------------------------------------------------------
        // +---------
        // | Multiplication/Division
        // +---------
        // With a scalar (External laws)
        template<typename coordType>
        Point3DIBase<coordType> operator * (const coordType& iValue, const Point3DIBase<coordType>& V)
        {
            return Point3DIBase<coordType>(iValue * V.x, iValue * V.y, iValue * V.z);
        }

        template<typename coordType>
        Point3DIBase<coordType> operator * (const Point3DIBase<coordType>& V, const coordType& iValue)
        {
            return Point3DIBase<coordType>(V.x * iValue, V.y * iValue, V.z * iValue);
        }

        template<typename coordType>
        Point3DIBase<coordType> operator / (const coordType& iValue, const Point3DIBase<coordType>& V)
        {
            return Point3DIBase<coordType>(iValue / V.x, iValue / V.y, iValue / V.z);
        }

        template<typename coordType>
        Point3DIBase<coordType> operator / (const Point3DIBase<coordType>& V, const coordType& iValue)
        {
            return Point3DIBase<coordType>(V.x / iValue, V.y / iValue, V.z / iValue);
        }

        // With assignement
        template<typename coordType>
        const Point3DIBase<coordType>& Point3DIBase<coordType>::operator *= (const coordType& iValue)
        {
            x *= iValue;
            y *= iValue;
            z *= iValue;

            return *this;
        }

        template<typename coordType>
        const Point3DIBase<coordType>& Point3DIBase<coordType>::operator /= (const coordType& iValue)
        {
            x /= iValue;
            y /= iValue;
            z /= iValue;

            return *this;
        }

        template<typename coordType>
        const Point3DIBase<coordType>& Point3DIBase<coordType>::operator *= (const Point3DIBase<coordType>& p)
        {
            x *= p.x;
            y *= p.y;
            z *= p.z;

            return *this;
        }

        template<typename coordType>
        const Point3DIBase<coordType>& Point3DIBase<coordType>::operator /= (const Point3DIBase<coordType>& p)
        {
            x /= p.x;
            y /= p.y;
            z /= p.z;

            return *this;
        }

        // ----------------------------------------------------------------------
        // +---------
        // | Euclidian operations
        // +---------
        template<typename coordType>
        coordType Norm(const Point3DIBase<coordType>& P)
        {
            return (coordType)sqrtF((Float)(sqr(P.x) + sqr(P.y) + sqr(P.z)));
        }

        template<typename coordType>
        coordType NormSquare(const Point3DIBase<coordType>& P)
        {
            return sqr(P.x) + sqr(P.y) + sqr(P.z);
        }

        template<typename coordType>
        coordType Dist(const Point3DIBase<coordType>& P1, const Point3DIBase<coordType>& P2)
        {
            const coordType dx = P2.x - P1.x;
            const coordType dy = P2.y - P1.y;
            const coordType dz = P2.z - P1.z;
            return (coordType)sqrtF((Float)(sqr(dx) + sqr(dy) + sqr(dz)));
        }

        template<typename coordType>
        coordType DistSquare(const Point3DIBase<coordType>& P1, const Point3DIBase<coordType>& P2)
        {
            const coordType dx = P2.x - P1.x;
            const coordType dy = P2.y - P1.y;
            const coordType dz = P2.z - P1.z;
            return sqr(dx) + sqr(dy) + sqr(dz);
        }
        /*

        coordType SegmentDist(const Point3DIBase& Pt1, const Point3DIBase& Pt2,const Point3DIBase& Pt3, const Point3DIBase& Pt4)
        {
            Vector3D   u = Pt2 - Pt1;
            Vector3D   v = Pt4 - Pt3;
            Vector3D   w = Pt1 - Pt3;
            Float    a = Dot(u,u);        // always >= 0
            Float    b = Dot(u,v);
            Float    c = Dot(v,v);        // always >= 0
            Float    d = Dot(u,w);
            Float    e = Dot(v,w);
            Float    D = a*c - b*b;       // always >= 0
            Float    sc, sN, sD = D;      // sc = sN / sD, default sD = D >= 0
            Float    tc, tN, tD = D;      // tc = tN / tD, default tD = D >= 0

            // compute the line parameters of the two closest points
            if (D < (SMALL_NUM)) { // the lines are almost parallel
                sN = 0.0f;        // force using point P0 on segment S1
                sD = 1.0f;        // to prevent possible division by 0.0 later
                tN = e;
                tD = c;
            }
            else {                // get the closest points on the infinite lines
                sN = (b*e - c*d);
                tN = (a*e - b*d);
                if (sN < 0.0f) {       // sc < 0 => the s=0 edge is visible
                    sN = 0.0f;
                    tN = e;
                    tD = c;
                }
                else if (sN > sD) {  // sc > 1 => the s=1 edge is visible
                    sN = sD;
                    tN = e + b;
                    tD = c;
                }
            }

            if (tN < 0.0f) {           // tc < 0 => the t=0 edge is visible
                tN = 0.0f;
                // recompute sc for this edge
                if (-d < 0.0f)
                    sN = 0.0f;
                else if (-d > a)
                    sN = sD;
                else {
                    sN = -d;
                    sD = a;
                }
            }
            else if (tN > tD) {      // tc > 1 => the t=1 edge is visible
                tN = tD;
                // recompute sc for this edge
                if ((-d + b) < 0.0f)
                    sN = 0.0f;
                else if ((-d + b) > a)
                    sN = sD;
                else {
                    sN = (-d + b);
                    sD = a;
                }
            }
            // finally do the division to get sc and tc
            sc = (absF(sN) < (SMALL_NUM) ? 0.0f : sN / sD);
            tc = (absF(tN) < (SMALL_NUM) ? 0.0f : tN / tD);

            // get the difference of the two closest points
            Vector3D   dP = w + (sc * u) - (tc * v);  // = S1(sc) - S2(tc)

            return Norm(dP);   // return the closest distance

        }

        coordType PointToSegmentDist(const Point3DIBase& Pt, const Point3DIBase& Pt1,const Point3DIBase& Pt2,Point3DIBase& nearest,bool& insideSegment)
        {
            Vector3D v = Pt2 - Pt1;
            Vector3D w = Pt - Pt1;

            Float c1 = Dot(w,v);
            insideSegment=false;
            if ( c1 <= 0.0f )
            {
                nearest=Pt1;
                return Dist(Pt, Pt1);
            }

            Float c2 = Dot(v,v);
            if ( c2 <= c1 )
            {
                nearest=Pt2;
                return Dist(Pt, Pt2);
            }

            insideSegment=true;
            Float b = c1 / c2;
            Point3DIBase Pb = Pt1 + (Float)b * v;
            nearest=Pb;
            return Dist(Pt, Pb);
        }
        */

        // ----------------------------------------------------------------------
        // +---------
        // | Utilities
        // +---------
        /*
        void Point3DIBase::Normalize( void )
        {
            register coordType tmp = Norm( *this );
            if (tmp != 0.0f)
            {
                tmp = 1.0f/tmp;
                x *= tmp;
                y *= tmp;
                z *= tmp;
            }
        }

        */

        // +---------
        // | Acces Operators
        // +---------
        template<typename coordType>
        const coordType& Point3DIBase<coordType>::operator[](size_t i) const
        {
            assert(i >= 0 && i < 3);
            return *((&x) + i);
        }

        template<typename coordType>
        coordType& Point3DIBase<coordType>::operator[](size_t i)
        {
            assert(i >= 0 && i < 3);
            return *((&x) + i);
        }

    }
}