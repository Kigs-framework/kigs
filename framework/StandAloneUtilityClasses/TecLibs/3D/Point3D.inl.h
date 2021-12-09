// **********************************************************************
// * FILE  : Point3D.cpp
// * GROUP : TecLibs, section Math
// *---------------------------------------------------------------------
// * PURPOSE : Implementation of Point3D
// *---------------------------------------------------------------------
// **********************************************************************



// ----------------------------------------------------------------------
// +---------
// | Life-cycle
// +---------
// Constructors
Point3D::Point3D()
{
}

Point3D::Point3D( const Float& x , const Float& y , const Float& z ) : x(x), y(y), z(z)
{
}

Point3D::Point3D( const Point3D& P ) : x(P.x), y(P.y), z(P.z)
{
}

Point3D::Point3D( const Point3D& A , const Point3D& B ,const asVector& v) : Point3D(B.x-A.x, B.y-A.y, B.z-A.z)
{
}


Point3D::Point3D(const Point3D& A, const Point3D& B, const asMiddle& v) : Point3D((B.x + A.x)*0.5f, (B.y + A.y)*0.5f, (B.z + A.z)*0.5f)
{
}



// Set (constructors-like)
void Point3D::Set( const Float& fValue )
{
    x = y = z = fValue;
}
void Point3D::Set( const Float& x , const Float& y , const Float& z )
{
    this->x = x;
    this->y = y;
    this->z = z;
}

void Point3D::Set( const Point3D& P )
{
    x = P.x;
    y = P.y;
    z = P.z;
}

void Point3D::Set( const Point3D& A, const Point3D& B)
{
	x = B.x - A.x;
	y = B.y - A.y;
	z = B.z - A.z;
}


// Assignement
Point3D& Point3D::operator =( const Point3D& V )
{
    x = V.x;
    y = V.y;
    z = V.z;
	
    return *this;
}

// Assignement
Point3D& Point3D::operator =( const Float& V )
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


Point3D operator + ( const Point3D& U , const Point3D& V )
{
    return Point3D(U.x + V.x, U.y + V.y, U.z + V.z);
}


Point3D operator - ( const Point3D& U , const Point3D& V )
{
	return Point3D(U.x - V.x, U.y - V.y, U.z - V.z);
}

// With assignement
Point3D& Point3D::operator += ( const Point3D& V )
{
    x += V.x;
    y += V.y;
    z += V.z;
	
    return *this;
}
Point3D& Point3D::operator -= ( const Point3D& V )
{
    x -= V.x;
    y -= V.y;
    z -= V.z;
	
    return *this;
}

// Unary
Point3D operator - ( const Point3D& V )
{
    return Point3D(-V.x, -V.y, -V.z);
}


// ----------------------------------------------------------------------
// +---------
// | Multiplication/Division
// +---------
// With a scalar (External laws)
Point3D operator * ( const Float& fValue , const Point3D& V )
{
    return Point3D(fValue * V.x, fValue * V.y, fValue * V.z);
}

Point3D operator * ( const Point3D& V , const Float& fValue )
{
    return Point3D(V.x * fValue, V.y * fValue, V.z * fValue);
}

Point3D operator * ( Point3D V , const Point3D& W )
{
	V *= W;
	return V;
}

Point3D operator / ( const Float& fValue , const Point3D& V )
{
    return Point3D(fValue / V.x, fValue / V.y, fValue / V.z);
}

Point3D operator / ( const Point3D& V , const Float& fValue )
{
    return Point3D(V.x / fValue, V.y / fValue, V.z / fValue);
}

Point3D operator / ( Point3D V , const Point3D& W )
{
	V /= W;
	return V;
}


// With assignement
Point3D& Point3D::operator *= ( const Float& fValue )
{
    x *= fValue;
    y *= fValue;
    z *= fValue;
	
    return *this;
}

Point3D& Point3D::operator /= ( const Float& fValue )
{
	const Float invfValue = KFLOAT_ONE / fValue;
    x *= invfValue;
    y *= invfValue;
    z *= invfValue;
	
    return *this;
}


Point3D& Point3D::operator *= ( const Point3D& p )
{
    x *= p.x;
    y *= p.y;
    z *= p.z;
    return *this;
}

Point3D& Point3D::operator /= ( const Point3D& p )
{
    x /= p.x;
    y /= p.y;
    z /= p.z;
    return *this;
}


Point3D Hadamard(Point3D a, const Point3D& b)
{
	a *= b;
	return a;
}



// ----------------------------------------------------------------------
// +---------
// | Euclidian operations
// +---------
Float Norm( const Point3D& P )
{
	return sqrtF( sqr(P.x) + sqr(P.y) + sqr(P.z) );
}

Float NormSquare( const Point3D& P )
{
    return sqr(P.x) + sqr(P.y) + sqr(P.z);
}

Float Dist( const Point3D& P1, const Point3D& P2 )
{
	const Float dx = P2.x - P1.x;
	const Float dy = P2.y - P1.y;
	const Float dz = P2.z - P1.z;
	return sqrtF(sqr(dx) + sqr(dy) + sqr(dz));
}

Float DistSquare( const Point3D& P1, const Point3D& P2 )
{
	const Float dx = P2.x - P1.x;
	const Float dy = P2.y - P1.y;
	const Float dz = P2.z - P1.z;
	return sqr(dx) + sqr(dy) + sqr(dz);
}

#define SMALL_NUM  0.001f

Float SegmentDist(const Point3D& Pt1, const Point3D& Pt2,const Point3D& Pt3, const Point3D& Pt4)
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
    if (D < KFLOAT_CONST(SMALL_NUM)) { // the lines are almost parallel
        sN = KFLOAT_CONST(0.0f);        // force using point P0 on segment S1
        sD = KFLOAT_CONST(1.0f);        // to prevent possible division by 0.0 later
        tN = e;
        tD = c;
    }
    else {                // get the closest points on the infinite lines
        sN = (b*e - c*d);
        tN = (a*e - b*d);
        if (sN < KFLOAT_CONST(0.0f)) {       // sc < 0 => the s=0 edge is visible
            sN = KFLOAT_CONST(0.0f);
            tN = e;
            tD = c;
        }
        else if (sN > sD) {  // sc > 1 => the s=1 edge is visible
            sN = sD;
            tN = e + b;
            tD = c;
        }
    }
	
    if (tN < KFLOAT_CONST(0.0f)) {           // tc < 0 => the t=0 edge is visible
        tN = KFLOAT_CONST(0.0f);
        // recompute sc for this edge
        if (-d < KFLOAT_CONST(0.0f))
            sN = KFLOAT_CONST(0.0f);
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
        if ((-d + b) < KFLOAT_CONST(0.0f))
            sN = KFLOAT_CONST(0.0f);
        else if ((-d + b) > a)
            sN = sD;
        else {
            sN = (-d + b);
            sD = a;
        }
    }
    // finally do the division to get sc and tc
    sc = (absF(sN) < KFLOAT_CONST(SMALL_NUM) ? KFLOAT_CONST(0.0f) : sN / sD);
    tc = (absF(tN) < KFLOAT_CONST(SMALL_NUM) ? KFLOAT_CONST(0.0f) : tN / tD);
	
    // get the difference of the two closest points
    Vector3D   dP = w + (sc * u) - (tc * v);  // = S1(sc) - S2(tc)
	
    return Norm(dP);   // return the closest distance
	
}

Float PointToSegmentDist(const Point3D& Pt, const Point3D& Pt1,const Point3D& Pt2,Point3D& nearest,bool& insideSegment)
{
	Vector3D v = (Vector3D)(Pt2 - Pt1);
	Vector3D w = (Vector3D)(Pt - Pt1);
	
    Float c1 = Dot(w,v);
	insideSegment=false;
    if ( c1 <= KFLOAT_CONST(0.0f) )
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
    Point3D Pb = Pt1 + (Point3D)((Float)b * v);
	nearest=Pb;
    return Dist(Pt, Pb);
}


// ----------------------------------------------------------------------
// +---------
// | Affine operations
// +---------
Point3D Mid( const Point3D& P , const Point3D& Q )
{
    return Point3D( (P.x + Q.x) * KFLOAT_CONST(0.5f),
				   (P.y + Q.y) * KFLOAT_CONST(0.5f),
				   (P.z + Q.z) * KFLOAT_CONST(0.5f) );
}

Point3D Bary( const Float& a , const Point3D& P , const Float& b , const Point3D& Q )
{
    return Point3D( ((a * P.x) + (b * Q.x)) ,
				   ((a * P.y) + (b * Q.y)) ,
				   ((a * P.z) + (b * Q.z)) );
}


// ----------------------------------------------------------------------
// +---------
// | Utilities
// +---------
void Point3D::Normalize( void )
{
    Float tmp = Norm( *this );
    if (tmp != KFLOAT_CONST(0.0f))
	{
		tmp = KFLOAT_CONST(1.0f)/tmp;
		x *= tmp;
		y *= tmp;
		z *= tmp;
	}
}

inline Point3D Point3D::Normalized( void ) const
{
	Point3D result = *this;
	result.Normalize();
	return result;
}



inline Point3D Lerp( const Point3D& P , const Point3D& Q, const Float& t )
{
	Float s = KFLOAT_CONST(1.0f) - t;
    return Point3D( (s * P.x) + (t * Q.x),
				   (s * P.y) + (t * Q.y),
				   (s * P.z) + (t * Q.z) );
}


inline Point3D ProjectOnPlane(Point3D q, Point3D o, Point3D n)
{
	return q - (Point3D)(Dot(Vector3D(o,q,asVector()), n)*n); 
}



/*
bool operator<( const Point3D& U , const Point3D& V )
{
	return (U.x < V.x && U.y < V.y && U.z < V.z);
}

bool operator<=( const Point3D& U , const Point3D& V )
{
	return (U.x <= V.x && U.y <= V.y && U.z <= V.z);
}

bool operator>( const Point3D& U , const Point3D& V )
{
	return (U.x > V.x && U.y > V.y && U.z > V.z);
}

bool operator>=( const Point3D& U , const Point3D& V )
{
	return (U.x >= V.x && U.y >= V.y && U.z >= V.z);
}*/

bool operator==( const Point3D& U , const Point3D& V )
{
	return (U.x == V.x && U.y == V.y && U.z == V.z);
}

bool operator!=( const Point3D& U , const Point3D& V )
{
	return (U.x != V.x || U.y != V.y || U.z != V.z);
}





// +---------
// | Acces Operators
// +---------
const Float& Point3D::operator[](Int i) const
{
	assert(i>=0 && i<3);
	return *((&x)+i);
}

Float& Point3D::operator[](Int i)
{
	assert(i>=0 && i<3);
	return *((&x)+i);
}


void Point3D::ClampMax(const Float v)
{
	if (x>v)
		x = v;
	if (y>v)
		y = v;
	if (z>v)
		z = v;
}

void Point3D::ClampMin(const Float v)
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
Point3D operator ^ ( const Point3D& U , const Point3D& V )
{
    return Point3D((U.y * V.z) - (U.z * V.y),
				   (U.z * V.x) - (U.x * V.z),
				   (U.x * V.y) - (U.y * V.x));
}

// With assignement
const Point3D& Point3D::operator ^= ( const Point3D& V )
{
	Float _x = x, _y = y;
    x = (_y * V.z) - (z * V.y);
    y = (z * V.x) - (_x * V.z);
    z = (_x * V.y) - (_y * V.x);
	
    return *this;
}

void Point3D::CrossProduct( const Point3D& V, const Point3D& W )
{
    x = (V.y * W.z) - (V.z * W.y);
    y = (V.z * W.x) - (V.x * W.z);
    z = (V.x * W.y) - (V.y * W.x);
}

// ----------------------------------------------------------------------
// +---------
// | Euclidian operations
// +---------
Float Dot( const Point3D& U , const Point3D& V )
{
    return ((U.x * V.x) + (U.y * V.y) + (U.z * V.z));
}





#undef SMALL_NUM