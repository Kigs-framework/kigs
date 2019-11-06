#include <cmath>

v3d::v3d()
{
}

v3d::v3d( const double& x , const double& y , const double& z ) : x(x), y(y), z(z)
{
}

v3d::v3d( const v3d& P ) : x(P.x), y(P.y), z(P.z)
{
}

v3d::v3d( const v3d& A , const v3d& B ,const asVector& v) : v3d(B.x-A.x, B.y-A.y, B.z-A.z)
{
}


v3d::v3d(const v3d& A, const v3d& B, const asMiddle& v) : v3d((B.x + A.x)*0.5, (B.y + A.y)*0.5, (B.z + A.z)*0.5)
{
}



// Set (constructors-like)
void v3d::Set( const double& fValue )
{
    x = y = z = fValue;
}
void v3d::Set( const double& x , const double& y , const double& z )
{
    this->x = x;
    this->y = y;
    this->z = z;
}

void v3d::Set( const v3d& P )
{
    x = P.x;
    y = P.y;
    z = P.z;
}

void v3d::Set( const v3d& A, const v3d& B)
{
	x = B.x - A.x;
	y = B.y - A.y;
	z = B.z - A.z;
}


// Assignement
v3d& v3d::operator =( const v3d& V )
{
    x = V.x;
    y = V.y;
    z = V.z;
	
    return *this;
}

// Assignement
v3d& v3d::operator =( const double& V )
{
    x = V;
    y = V;
	z = V;
    return *this;
}



// ----------------------------------------------------------------------
// +---------
// | Addition/Substraction
// +---------
// With another vector (Internal laws)


v3d operator + ( const v3d& U , const v3d& V )
{
    return v3d(U.x + V.x, U.y + V.y, U.z + V.z);
}


v3d operator - ( const v3d& U , const v3d& V )
{
	return v3d(U.x - V.x, U.y - V.y, U.z - V.z);
}

// With assignement
v3d& v3d::operator += ( const v3d& V )
{
    x += V.x;
    y += V.y;
    z += V.z;
	
    return *this;
}
v3d& v3d::operator -= ( const v3d& V )
{
    x -= V.x;
    y -= V.y;
    z -= V.z;
	
    return *this;
}

// Unary
v3d operator - ( const v3d& V )
{
    return v3d(-V.x, -V.y, -V.z);
}


// ----------------------------------------------------------------------
// +---------
// | Multiplication/Division
// +---------
// With a scalar (External laws)
v3d operator * ( const double& fValue , const v3d& V )
{
    return v3d(fValue * V.x, fValue * V.y, fValue * V.z);
}

v3d operator * ( const v3d& V , const double& fValue )
{
    return v3d(V.x * fValue, V.y * fValue, V.z * fValue);
}

v3d operator * ( v3d V , const v3d& W )
{
	V *= W;
	return V;
}

v3d operator / ( const double& fValue , const v3d& V )
{
    return v3d(fValue / V.x, fValue / V.y, fValue / V.z);
}

v3d operator / ( const v3d& V , const double& fValue )
{
    return v3d(V.x / fValue, V.y / fValue, V.z / fValue);
}

v3d operator / ( v3d V , const v3d& W )
{
	V /= W;
	return V;
}


// With assignement
v3d& v3d::operator *= ( const double& fValue )
{
    x *= fValue;
    y *= fValue;
    z *= fValue;
	
    return *this;
}

v3d& v3d::operator /= ( const double& fValue )
{
	const double invfValue = 1.0 / fValue;
    x *= invfValue;
    y *= invfValue;
    z *= invfValue;
	
    return *this;
}


v3d& v3d::operator *= ( const v3d& p )
{
    x *= p.x;
    y *= p.y;
    z *= p.z;
    return *this;
}

v3d& v3d::operator /= ( const v3d& p )
{
    x /= p.x;
    y /= p.y;
    z /= p.z;
    return *this;
}


v3d Hadamard(v3d a, const v3d& b)
{
	a *= b;
	return a;
}



// ----------------------------------------------------------------------
// +---------
// | Euclidian operations
// +---------
double Norm( const v3d& P )
{
	return sqrt( sqr(P.x) + sqr(P.y) + sqr(P.z) );
}

double NormSquare( const v3d& P )
{
    return sqr(P.x) + sqr(P.y) + sqr(P.z);
}

double Dist( const v3d& P1, const v3d& P2 )
{
	const double dx = P2.x - P1.x;
	const double dy = P2.y - P1.y;
	const double dz = P2.z - P1.z;
	return sqrt(sqr(dx) + sqr(dy) + sqr(dz));
}

double DistSquare( const v3d& P1, const v3d& P2 )
{
	const double dx = P2.x - P1.x;
	const double dy = P2.y - P1.y;
	const double dz = P2.z - P1.z;
	return sqr(dx) + sqr(dy) + sqr(dz);
}

#define SMALL_NUM  0.001

double SegmentDist(const v3d& Pt1, const v3d& Pt2,const v3d& Pt3, const v3d& Pt4)
{
    v3d u = Pt2 - Pt1;
	v3d v = Pt4 - Pt3;
	v3d w = Pt1 - Pt3;
    double    a = Dot(u,u);        // always >= 0
    double    b = Dot(u,v);
    double    c = Dot(v,v);        // always >= 0
    double    d = Dot(u,w);
    double    e = Dot(v,w);
    double    D = a*c - b*b;       // always >= 0
    double    sc, sN, sD = D;      // sc = sN / sD, default sD = D >= 0
    double    tc, tN, tD = D;      // tc = tN / tD, default tD = D >= 0
	
    // compute the line parameters of the two closest points
    if (D < (SMALL_NUM)) { // the lines are almost parallel
        sN = (0.0);        // force using point P0 on segment S1
        sD = (1.0);        // to prevent possible division by 0.0 later
        tN = e;
        tD = c;
    }
    else {                // get the closest points on the infinite lines
        sN = (b*e - c*d);
        tN = (a*e - b*d);
        if (sN < (0.0)) {       // sc < 0 => the s=0 edge is visible
            sN = (0.0);
            tN = e;
            tD = c;
        }
        else if (sN > sD) {  // sc > 1 => the s=1 edge is visible
            sN = sD;
            tN = e + b;
            tD = c;
        }
    }
	
    if (tN < (0.0)) {           // tc < 0 => the t=0 edge is visible
        tN = (0.0);
        // recompute sc for this edge
        if (-d < (0.0))
            sN = (0.0);
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
        if ((-d + b) < (0.0))
            sN = (0.0);
        else if ((-d + b) > a)
            sN = sD;
        else {
            sN = (-d + b);
            sD = a;
        }
    }
    // finally do the division to get sc and tc
    sc = (std::abs(sN) < SMALL_NUM ? 0.0 : sN / sD);
    tc = (std::abs(tN) < SMALL_NUM ? 0.0 : tN / tD);
	
    // get the difference of the two closest points
	v3d dP = w + (sc * u) - (tc * v);  // = S1(sc) - S2(tc)
	
    return Norm(dP);   // return the closest distance
	
}

double PointToSegmentDist(const v3d& Pt, const v3d& Pt1,const v3d& Pt2,v3d& nearest,bool& insideSegment)
{
	v3d v = Pt2 - Pt1;
	v3d w = Pt - Pt1;
	
    double c1 = Dot(w,v);
	insideSegment=false;
    if ( c1 <= (0.0) )
	{
		nearest=Pt1;
        return Dist(Pt, Pt1);
	}
	
    double c2 = Dot(v,v);
    if ( c2 <= c1 )
	{
		nearest=Pt2;
        return Dist(Pt, Pt2);
	}
	
	insideSegment=true;
    double b = c1 / c2;
    v3d Pb = Pt1 + (v3d)((double)b * v);
	nearest=Pb;
    return Dist(Pt, Pb);
}


// ----------------------------------------------------------------------
// +---------
// | Affine operations
// +---------
v3d Mid( const v3d& P , const v3d& Q )
{
    return v3d( (P.x + Q.x) * (0.5),
				   (P.y + Q.y) * (0.5),
				   (P.z + Q.z) * (0.5) );
}

v3d Bary( const double& a , const v3d& P , const double& b , const v3d& Q )
{
    return v3d( ((a * P.x) + (b * Q.x)) ,
				   ((a * P.y) + (b * Q.y)) ,
				   ((a * P.z) + (b * Q.z)) );
}


// ----------------------------------------------------------------------
// +---------
// | Utilities
// +---------
void v3d::Normalize( void )
{
    double tmp = Norm( *this );
    if (tmp != (0.0))
	{
		tmp = (1.0)/tmp;
		x *= tmp;
		y *= tmp;
		z *= tmp;
	}
}

inline v3d v3d::Normalized( void ) const
{
	v3d result = *this;
	result.Normalize();
	return result;
}



inline v3d Lerp( const v3d& P , const v3d& Q, const double& t )
{
	double s = (1.0) - t;
    return v3d( (s * P.x) + (t * Q.x),
				   (s * P.y) + (t * Q.y),
				   (s * P.z) + (t * Q.z) );
}


inline v3d ProjectOnPlane(v3d q, v3d o, v3d n)
{
	return q - (v3d)(Dot(q-o, n)*n); 
}




bool operator<( const v3d& U , const v3d& V )
{
	return (U.x < V.x && U.y < V.y && U.z < V.z);
}

bool operator<=( const v3d& U , const v3d& V )
{
	return (U.x <= V.x && U.y <= V.y && U.z <= V.z);
}

bool operator>( const v3d& U , const v3d& V )
{
	return (U.x > V.x && U.y > V.y && U.z > V.z);
}

bool operator>=( const v3d& U , const v3d& V )
{
	return (U.x >= V.x && U.y >= V.y && U.z >= V.z);
}

bool operator==( const v3d& U , const v3d& V )
{
	return (U.x == V.x && U.y == V.y && U.z == V.z);
}

bool operator!=( const v3d& U , const v3d& V )
{
	return (U.x != V.x || U.y != V.y || U.z != V.z);
}





// +---------
// | Acces Operators
// +---------
const double& v3d::operator[](Int i) const
{
	assert(i>=0 && i<3);
	return *((&x)+i);
}

double& v3d::operator[](Int i)
{
	assert(i>=0 && i<3);
	return *((&x)+i);
}


void v3d::ClampMax(const double v)
{
	if (x>v)
		x = v;
	if (y>v)
		y = v;
	if (z>v)
		z = v;
}

void v3d::ClampMin(const double v)
{
	if (x<v)
		x = v;
	if (y<v)
		y = v;
	if (z<v)
		z = v;
}





// ----------------------------------------------------------------------
// +---------
// | Cross product
// +---------
v3d operator ^ ( const v3d& U , const v3d& V )
{
    return v3d((U.y * V.z) - (U.z * V.y),
				   (U.z * V.x) - (U.x * V.z),
				   (U.x * V.y) - (U.y * V.x));
}

// With assignement
const v3d& v3d::operator ^= ( const v3d& V )
{
	double _x = x, _y = y;
    x = (_y * V.z) - (z * V.y);
    y = (z * V.x) - (_x * V.z);
    z = (_x * V.y) - (_y * V.x);
	
    return *this;
}

void v3d::CrossProduct( const v3d& V, const v3d& W )
{
    x = (V.y * W.z) - (V.z * W.y);
    y = (V.z * W.x) - (V.x * W.z);
    z = (V.x * W.y) - (V.y * W.x);
}

// ----------------------------------------------------------------------
// +---------
// | Euclidian operations
// +---------
double Dot( const v3d& U , const v3d& V )
{
    return ((U.x * V.x) + (U.y * V.y) + (U.z * V.z));
}


#undef SMALL_NUM