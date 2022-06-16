// **********************************************************************
// * FILE  : SIMDv4f.cpp
// * GROUP : TecLibs, section Math
// *---------------------------------------------------------------------
// * PURPOSE : Implementation of SIMDv4f
// *---------------------------------------------------------------------
// **********************************************************************



// ----------------------------------------------------------------------
// +---------
// | Life-cycle
// +---------
// Constructors
SIMDv4f::SIMDv4f()
{
}

SIMDv4f::SIMDv4f(const float x, const float y, const float z, const float w) : x(x), y(y), z(z), w(w)
{
}

SIMDv4f::SIMDv4f(const SIMDv4f& P) : v(P.v)
{

}

SIMDv4f::SIMDv4f(const SIMDv4f& A, const SIMDv4f& B, const asVector& asv)
{
    v = vsubq_f32(B.v, A.v);
}


SIMDv4f::SIMDv4f(const SIMDv4f& A, const SIMDv4f& B, const asMiddle& asmiddle)
{
    v = vaddq_f32(A.v, B.v);
    float32x4_t tmp = vdupq_n_f32(0.5);
    v = vmulq_f32(tmp, v);
}

// Set (constructors-like)
void SIMDv4f::Set(const float fValue)
{
    // warning, w is also set to fValue here
    v=vdupq_n_f32(fValue);
}
void SIMDv4f::Set(const float x, const float y, const float z, const float w) 
{
    v = { x,y,z,w };
}

void SIMDv4f::Set(const SIMDv4f& P)
{
    v=P.v;
}


// Assignement
SIMDv4f& SIMDv4f::operator =(const SIMDv4f& V)
{
    v = V.v;

    return *this;
}

// Assignement
SIMDv4f& SIMDv4f::operator =(const float V)
{
    // warning, w is also set to fValue here
    v = vdupq_n_f32(V);
    return *this;
}



// ----------------------------------------------------------------------
// +---------
// | Addition/Substraction
// +---------
// With another vector (Internal laws)


SIMDv4f operator + (const SIMDv4f& U, const SIMDv4f& V)
{
    SIMDv4f result;
    result.v = vaddq_f32(U.v, V.v);
    return result;
}


SIMDv4f operator - (const SIMDv4f& U, const SIMDv4f& V)
{
    SIMDv4f result;
    result.v = vsubq_f32(U.v, V.v);
    return result;
}

// With assignement
SIMDv4f& SIMDv4f::operator += (const SIMDv4f& V)
{
    v = vaddq_f32(v, V.v);
    return *this;
}
SIMDv4f& SIMDv4f::operator -= (const SIMDv4f& V)
{
    v = vsubq_f32(v, V.v);
    return *this;
}

// Unary
SIMDv4f operator - (const SIMDv4f& V)
{
    SIMDv4f result;
    result.v = vnegq_f32(V.v);
    return result;
}


// ----------------------------------------------------------------------
// +---------
// | Multiplication/Division
// +---------
// With a scalar (External laws)
SIMDv4f operator * (const float fValue, const SIMDv4f& V)
{
    SIMDv4f result;
    float32x4_t tmp = vdupq_n_f32(fValue);
    result.v = vmulq_f32(tmp, V.v);
    return result;
}

SIMDv4f operator * (const SIMDv4f& V, const float fValue)
{
    SIMDv4f result;
    float32x4_t tmp = vdupq_n_f32(fValue);
    result.v = vmulq_f32(tmp, V.v);
    return result;
}

SIMDv4f operator * (SIMDv4f V, const SIMDv4f& W)
{
    V.v = vmulq_f32(V.v, W.v);
    return V;
}

SIMDv4f operator / (const float fValue, const SIMDv4f& V)
{
    SIMDv4f result;
    float32x4_t tmp = vdupq_n_f32(fValue);

    float32x4_t reciprocal = vrecpeq_f32(V.v);
    reciprocal = vmulq_f32(vrecpsq_f32(V.v, reciprocal), reciprocal);
    // needs precision ?
    reciprocal = vmulq_f32(vrecpsq_f32(V.v, reciprocal), reciprocal);

    result.v = vmulq_f32(tmp, reciprocal);
    return result;

}

SIMDv4f operator / (const SIMDv4f& V, const float fValue)
{
    SIMDv4f result;
    float32x4_t tmp = vdupq_n_f32(1.0f/fValue);
    result.v = vmulq_f32(tmp, V.v);
    return result;
}

SIMDv4f operator / (SIMDv4f V, const SIMDv4f& W)
{
    
    float32x4_t reciprocal = vrecpeq_f32(W.v);
    reciprocal = vmulq_f32(vrecpsq_f32(W.v, reciprocal), reciprocal);
    // needs precision ?
    reciprocal = vmulq_f32(vrecpsq_f32(W.v, reciprocal), reciprocal);

    V.v = vmulq_f32(V.v, reciprocal);
    return V;
}


// With assignement
SIMDv4f& SIMDv4f::operator *= (const float fValue)
{
    float32x4_t tmp = vdupq_n_f32(fValue);
    v = vmulq_f32(tmp, v);
    return *this;
}

// add the same value to the 3 coords
SIMDv4f& SIMDv4f::operator += (const float fValue)
{
    float32x4_t tmp = vdupq_n_f32(fValue);
    v = vaddq_f32(tmp, v);
    return *this;
}

SIMDv4f& SIMDv4f::operator /= (const float fValue)
{
    float32x4_t tmp = vdupq_n_f32(1.0f / fValue);
    v = vmulq_f32(tmp, v);

    return *this;
}

// remove the same value to the 3 coords
SIMDv4f& SIMDv4f::operator -= (const float fValue)
{
    float32x4_t tmp = vdupq_n_f32(fValue);
    v = vsubq_f32(v,tmp);
    return *this;
}

SIMDv4f& SIMDv4f::operator *= (const SIMDv4f& p)
{
    v = vsubq_f32(v, p.v);
    return *this;
}

SIMDv4f& SIMDv4f::operator /= (const SIMDv4f& p)
{
    float32x4_t reciprocal = vrecpeq_f32(p.v);
    reciprocal = vmulq_f32(vrecpsq_f32(p.v, reciprocal), reciprocal);
    // needs precision ?
    reciprocal = vmulq_f32(vrecpsq_f32(p.v, reciprocal), reciprocal);
    v = vmulq_f32(v, reciprocal);
    return *this;
}


SIMDv4f Hadamard(SIMDv4f a, const SIMDv4f& b)
{
    a *= b;
    return a;
}



// ----------------------------------------------------------------------
// +---------
// | Euclidian operations
// +---------
float Norm(const SIMDv4f& P)
{
    float32x4_t result = vmulq_f32(P.v, P.v);
    float32x2_t r = vadd_f32(vget_high_f32(result), vget_low_f32(result));
    return sqrtF(vget_lane_f32(vpadd_f32(r, r), 0));
}

float NormSquare(const SIMDv4f& P)
{
    float32x4_t result = vmulq_f32(P.v, P.v);
    float32x2_t r = vadd_f32(vget_high_f32(result), vget_low_f32(result));
    return vget_lane_f32(vpadd_f32(r, r), 0);
}

float Dist(const SIMDv4f& P1, const SIMDv4f& P2)
{
    return Norm(P2 - P2);
}

float DistSquare(const SIMDv4f& P1, const SIMDv4f& P2)
{
    return NormSquare(P2 - P2);
}

#define SMALL_NUM  0.001f

// can probably be optimized specifically for SIMD
float SegmentDist(const SIMDv4f& Pt1, const SIMDv4f& Pt2, const SIMDv4f& Pt3, const SIMDv4f& Pt4)
{
    SIMDv4f   u = Pt2 - Pt1;
    SIMDv4f   v = Pt4 - Pt3;
    SIMDv4f   w = Pt1 - Pt3;
    float    a = Dot(u, u);        // always >= 0
    float    b = Dot(u, v);
    float    c = Dot(v, v);        // always >= 0
    float    d = Dot(u, w);
    float    e = Dot(v, w);
    float    D = a * c - b * b;       // always >= 0
    float    sc, sN, sD = D;      // sc = sN / sD, default sD = D >= 0
    float    tc, tN, tD = D;      // tc = tN / tD, default tD = D >= 0

    // compute the line parameters of the two closest points
    if (D < KFLOAT_CONST(SMALL_NUM)) { // the lines are almost parallel
        sN = KFLOAT_CONST(0.0f);        // force using point P0 on segment S1
        sD = KFLOAT_CONST(1.0f);        // to prevent possible division by 0.0 later
        tN = e;
        tD = c;
    }
    else {                // get the closest points on the infinite lines
        sN = (b * e - c * d);
        tN = (a * e - b * d);
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
    SIMDv4f   dP = w + (sc * u) - (tc * v);  // = S1(sc) - S2(tc)

    return Norm(dP);   // return the closest distance

}

// can probably be optimized specifically for SIMD
float PointToSegmentDist(const SIMDv4f& Pt, const SIMDv4f& Pt1, const SIMDv4f& Pt2, SIMDv4f& nearest, bool& insideSegment)
{
    SIMDv4f v = (Pt2 - Pt1);
    SIMDv4f w = (Pt - Pt1);

    float c1 = Dot(w, v);
    insideSegment = false;
    if (c1 <= KFLOAT_CONST(0.0f))
    {
        nearest = Pt1;
        return Dist(Pt, Pt1);
    }

    float c2 = Dot(v, v);
    if (c2 <= c1)
    {
        nearest = Pt2;
        return Dist(Pt, Pt2);
    }

    insideSegment = true;
    float b = c1 / c2;
    SIMDv4f Pb = Pt1 + (SIMDv4f)((float)b * v);
    nearest = Pb;
    return Dist(Pt, Pb);
}


// ----------------------------------------------------------------------
// +---------
// | Affine operations
// +---------
SIMDv4f Mid(const SIMDv4f& P, const SIMDv4f& Q)
{
    SIMDv4f result;
    result.v = vaddq_f32(P.v, Q.v);
    float32x4_t tmp = vdupq_n_f32(0.5);
    result.v = vmulq_f32(tmp, result.v);

    return result;
}

SIMDv4f Bary(const float a, const SIMDv4f& P, const float b, const SIMDv4f& Q)
{
    SIMDv4f result;
   
    float32x4_t tmpa = vdupq_n_f32(a);
    tmpa=vmulq_f32(tmpa, P.v);
    float32x4_t tmpb = vdupq_n_f32(b);
    tmpb = vmulq_f32(tmpb, Q.v);
    result.v = vaddq_f32(tmpa, tmpb);
    return result;

}


// ----------------------------------------------------------------------
// +---------
// | Utilities
// +---------
void SIMDv4f::Normalize(void)
{
    float tmp = Norm(*this);
    if (tmp != 0.0f)
    {
        (*this) *= 1.0f / tmp;
    }
}

// for barycentric coords, sum of coordinates must be 1
void SIMDv4f::NormalizeBarycentricCoords(void)
{
    float32x2_t r = vadd_f32(vget_high_f32(v), vget_low_f32(v));
    float tmp = vget_lane_f32(vpadd_f32(r, r), 0);
    (*this) *= 1.0f / tmp;
}

inline SIMDv4f SIMDv4f::Normalized(void) const
{
    SIMDv4f result(*this);
    result.Normalize();
    return result;
}

inline SIMDv4f SIMDv4f::NormalizedBarycentricCoords(void) const
{
    SIMDv4f result(*this);
    result.NormalizeBarycentricCoords();
    return result;
}

inline SIMDv4f Lerp(const SIMDv4f& P, const SIMDv4f& Q, const float t)
{
    float s = 1.0f - t;
    return Bary(s, P, t, Q);
}


inline SIMDv4f ProjectOnPlane(SIMDv4f q, SIMDv4f o, SIMDv4f n)
{
    return q - (SIMDv4f)(Dot(SIMDv4f(o, q, asVector()), n) * n);
}

bool operator==(const SIMDv4f& U, const SIMDv4f& V)
{
    return (U.x == V.x && U.y == V.y && U.z == V.z);
}

bool operator!=(const SIMDv4f& U, const SIMDv4f& V)
{
    return (U.x != V.x || U.y != V.y || U.z != V.z);
}

// +---------
// | Acces Operators
// +---------
const float& SIMDv4f::operator[](Int i) const
{
    assert(i >= 0 && i < 3);
    return *((&x) + i);
}

float& SIMDv4f::operator[](Int i)
{
    assert(i >= 0 && i < 3);
    return *((&x) + i);
}


void SIMDv4f::ClampMax(const float m)
{
    float32x4_t maxv = vdupq_n_f32(m);
    v=vmaxq_f32(v, maxv);
}

void SIMDv4f::ClampMin(const float m)
{
    float32x4_t maxv = vdupq_n_f32(m);
    v = vminq_f32(v, maxv);
}

// ----------------------------------------------------------------------
// +---------
// | Cross product
// +---------
// assume 3D vector and cross product is not efficient in ARM SIMD
SIMDv4f operator^ (const SIMDv4f& U, const SIMDv4f& V)
{

    return SIMDv4f((U.y * V.z) - (U.z * V.y),
        (U.z * V.x) - (U.x * V.z),
        (U.x * V.y) - (U.y * V.x));
}

// With assignement
const SIMDv4f& SIMDv4f::operator ^= (const SIMDv4f& V)
{
    float _x = x, _y = y;
    x = (_y * V.z) - (z * V.y);
    y = (z * V.x) - (_x * V.z);
    z = (_x * V.y) - (_y * V.x);
    w = 0;
    return *this;
}

void SIMDv4f::CrossProduct(const SIMDv4f& V, const SIMDv4f& W)
{
    x = (V.y * W.z) - (V.z * W.y);
    y = (V.z * W.x) - (V.x * W.z);
    z = (V.x * W.y) - (V.y * W.x);
    w = 0;
}

// ----------------------------------------------------------------------
// +---------
// | Euclidian operations
// +---------
float Dot(const SIMDv4f& U, const SIMDv4f& V)
{
    float32x4_t result = vmulq_f32(U.v, V.v);
    float32x2_t r = vadd_f32(vget_high_f32(result), vget_low_f32(result));
    return vget_lane_f32(vpadd_f32(r, r), 0);
}

#undef SMALL_NUM