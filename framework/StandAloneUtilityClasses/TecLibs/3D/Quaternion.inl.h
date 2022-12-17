// **********************************************************************
// * FILE  : Quaternion.inl
// * GROUP : Teclib/3D
// *---------------------------------------------------------------------
// * PURPOSE : Implementation of the inline methods found in the the quaternion class definition
// * COMMENT : Supposed to be included in the Quaternion.h file... Not to be use as this
// **********************************************************************

inline Quaternion::Quaternion()
{
    ;
}

// +---------
// | Copy constructor
// +---------
inline Quaternion::Quaternion(const Quaternion& q)
{
	V = q.V;
	w = q.w;
}

// +---------
// | Constructor
// +---------
inline Quaternion::Quaternion(const Float& x, const Float& y, const Float& z, const Float& w)
{
	V.x = x;
	V.y = y;
	V.z = z;
	this->w = w;
}

// +---------
// | Constructor from a Matrix
// +---------
inline Quaternion::Quaternion(const Matrix3x3& crMatrix)
{
	Set(crMatrix);
}

// +---------
// | Constructor from a Matrix
// +---------
inline Quaternion::Quaternion(const Matrix3x4& crMatrix)
{
	Set(crMatrix);
}

// +---------
// | Set Identity
// +---------
inline void Quaternion::SetIdentity()
{
	w = 1;
	V.x = V.y = V.z = 0;
}

// +---------
// | Set from Floats
// +---------
inline void Quaternion::Set(const Float& x, const Float& y, const Float& z, const Float& tw)
{
	V.x = x;
	V.y = y;
	V.z = z;
	w = tw;
}

// +---------
// | Set from a Matrix
// +---------
inline void Quaternion::Set(const Matrix3x3& crMatrix)
{
	Float T = crMatrix.e[0][0] + crMatrix.e[1][1] + crMatrix.e[2][2];
	if ( T > 0.001f )
	{
		Float S = sqrtf(T+1.0f) * 2;
		V.x = ( crMatrix.e[2][1] - crMatrix.e[1][2] ) / S;
		V.y = ( crMatrix.e[0][2] - crMatrix.e[2][0] ) / S;
		V.z = ( crMatrix.e[1][0] - crMatrix.e[0][1] ) / S;
		w = 0.25f * S;
		return;
	}
	
    if ( crMatrix.e[0][0] > crMatrix.e[1][1] && crMatrix.e[0][0] > crMatrix.e[2][2] )
	{
        Float S  = sqrtf( 1.0f + crMatrix.e[0][0] - crMatrix.e[1][1] - crMatrix.e[2][2] ) * 2.0f;
        V.x = 0.25f * S;
        V.y = (crMatrix.e[1][0] + crMatrix.e[0][1] ) / S;
        V.z = (crMatrix.e[0][2] + crMatrix.e[2][0] ) / S;
        w = (crMatrix.e[2][1] - crMatrix.e[1][2] ) / S;
    }
	else
		if ( crMatrix.e[1][1] > crMatrix.e[2][2] )
	{
		Float S  = sqrtf( 1.0f + crMatrix.e[1][1] - crMatrix.e[0][0] - crMatrix.e[2][2] ) * 2.0f;
		V.x = (crMatrix.e[1][0] + crMatrix.e[0][1] ) / S;
		V.y = 0.25f * S;
		V.z = (crMatrix.e[2][1] + crMatrix.e[1][2] ) / S;
		w = (crMatrix.e[0][2] - crMatrix.e[2][0] ) / S;
	}
	else
	{
		Float S  = sqrtf( 1.0f + crMatrix.e[2][2] - crMatrix.e[0][0] - crMatrix.e[1][1] ) * 2.0f;
		V.x = (crMatrix.e[0][2] + crMatrix.e[2][0] ) / S;
		V.y = (crMatrix.e[2][1] + crMatrix.e[1][2] ) / S;
		V.z = 0.25f * S;
		w = (crMatrix.e[1][0] - crMatrix.e[0][1] ) / S;
	}
	Normalize();
}

// +---------
// | Set from a Matrix
// +---------
inline void Quaternion::Set(const Matrix3x4& crMatrix)
{
	Float T = crMatrix.e[0][0] + crMatrix.e[1][1] + crMatrix.e[2][2];
	if ( T > 0.001f )
	{
		Float S = sqrtf(T+1.0f) * 2;
		V.x = ( crMatrix.e[2][1] - crMatrix.e[1][2] ) / S;
		V.y = ( crMatrix.e[0][2] - crMatrix.e[2][0] ) / S;
		V.z = ( crMatrix.e[1][0] - crMatrix.e[0][1] ) / S;
		w = 0.25f * S;
		return;
	}
	
    if ( crMatrix.e[0][0] > crMatrix.e[1][1] && crMatrix.e[0][0] > crMatrix.e[2][2] )
	{
        Float S  = sqrtf( 1.0f + crMatrix.e[0][0] - crMatrix.e[1][1] - crMatrix.e[2][2] ) * 2.0f;
        V.x = 0.25f * S;
        V.y = (crMatrix.e[1][0] + crMatrix.e[0][1] ) / S;
        V.z = (crMatrix.e[0][2] + crMatrix.e[2][0] ) / S;
        w = (crMatrix.e[2][1] - crMatrix.e[1][2] ) / S;
    }
	else
		if ( crMatrix.e[1][1] > crMatrix.e[2][2] )
	{
		Float S  = sqrtf( 1.0f + crMatrix.e[1][1] - crMatrix.e[0][0] - crMatrix.e[2][2] ) * 2.0f;
		V.x = (crMatrix.e[1][0] + crMatrix.e[0][1] ) / S;
		V.y = 0.25f * S;
		V.z = (crMatrix.e[2][1] + crMatrix.e[1][2] ) / S;
		w = (crMatrix.e[0][2] - crMatrix.e[2][0] ) / S;
	}
	else
	{
		Float S  = sqrtf( 1.0f + crMatrix.e[2][2] - crMatrix.e[0][0] - crMatrix.e[1][1] ) * 2.0f;
		V.x = (crMatrix.e[0][2] + crMatrix.e[2][0] ) / S;
		V.y = (crMatrix.e[2][1] + crMatrix.e[1][2] ) / S;
		V.z = 0.25f * S;
		w = (crMatrix.e[1][0] - crMatrix.e[0][1] ) / S;
	}
}

inline void Quaternion::SetAngAxis(const Vector3D& axis, const Float& angle)
{
	Float sf=sinf(angle*0.5f);
	
	V=axis;
	V.Normalize();
	
	w   =   cosf(angle*0.5f);
    V.x   =   axis.x * sf;
    V.y   =   axis.y * sf;
    V.z   =   axis.z * sf;
}

inline void Quaternion::GetAngAxis(Vector3D& axis, Float& angle)
{
	Float scale = sqrtf (V.x*V.x + V.y*V.y + V.z*V.z);
	
	axis=V;
	axis*=1.0f/scale;
	
	angle=2.0f*acosf(w);
	
}

// get euler angles

inline void Quaternion::GetEulerAngles(Vector3D& euler) const
{
	Float sqw = w*w;    
	Float sqx = V.x*V.x;    
	Float sqy = V.y*V.y;    
	Float sqz = V.z*V.z;    
	
	euler.x = atan2f(2.0f * (V.x*V.y + V.z*w), sqx - sqy - sqz + sqw);    		
	euler.y = asinf(-2.0f * (V.x*V.z - V.y*w));
	euler.z = atan2f(2.0f * (V.y*V.z + V.x*w), -sqx - sqy + sqz + sqw);    
}

// +---------
// | Copy
// +---------
inline void Quaternion::operator=(const Quaternion& q)
{
	V = q.V;
	w = q.w;
}


inline bool Quaternion::operator==(const Quaternion& q)
{
	return (V == q.V) && (w == q.w);
}

inline bool Quaternion::operator!=(const Quaternion& q)
{
	return (V != q.V)||(w != q.w);
}



inline Matrix3x3 Quaternion::ToMatrix() const
{
	Matrix3x3 result;
	result.Set(*this);
	return result;
}


/*
// +---------
// | Cast From a matrix 3x4
// +---------
inline Quaternion::operator const Matrix3x4 (void)
{
 Matrix3x4 m;
 
 m.Set(*this);
 
 return m;
}
*/
// +---------
// | Normalize the quaternion
// +---------
inline void Quaternion::Normalize()
{
	Float InvNorm = (static_cast<Float> (1.0f))/Norm(*this);
	V*=InvNorm;
	w*=InvNorm;
}

// +---------
// | returns the norme of the quaternion
// +---------
inline Float Norm(const Quaternion & cr_Quaternion)
{
	return sqrtF(NormSquare(cr_Quaternion));
}


// +---------
// | returns the squarred Norm q.conjugate(q)  (((  conjugate(q) = (w,-v)   )))
// +---------
inline Float NormSquare(const Quaternion & cr_Quaternion)
{
	return (NormSquare(cr_Quaternion.V) + cr_Quaternion.w*cr_Quaternion.w);
}


// +---------
// | returns the rotation angle corresponding to q, given in the interval [-pi,pi[
// +---------
inline Float Angle(const Quaternion& q)
{
	Float theta = 2*acosF(q.w);
	if (theta >= (Float)PI) return theta - 2 * (Float)PI;
	return theta;
}


// +---------
// | Sets the quaternion for a rotation around X of an angle of cr_Angle
// +---------
inline void Quaternion::SetRotationX(const Float& cr_Angle)
{
	Quaternion q;
	w = cosF(cr_Angle*0.5f);
	V.x = sinF(cr_Angle*0.5f);
	V.y = V.z = 0;
}


// +---------
// | Sets the quaternion for a rotation around Y of an angle of cr_Angle
// +---------
inline void Quaternion::SetRotationY(const Float& cr_Angle)
{
	w = cosF(cr_Angle*0.5f);
	V.y = sinF(cr_Angle*0.5f);
	V.x = V.z = 0;
}

// +---------
// | Sets the quaternion for a rotation around Z of an angle of cr_Angle
// +---------
inline void Quaternion::SetRotationZ(const Float& cr_Angle)
{
	w = cosF(cr_Angle*0.5f);
	V.z = sinF(cr_Angle*0.5f);
	V.x = V.y = 0;
}


// +---------
// | Sets the quaternion for a rotation around a direction cr_Vector of an angle of cr_Angle
// +---------
inline void Quaternion::SetRotationU(const Vector3D& cr_Vector, const Float& cr_Angle)
{
	Float sin_a, cos_a;
	sin_a = sinF( cr_Angle / 2 );
	cos_a = cosF( cr_Angle / 2 );
    V.x	= cr_Vector.x * sin_a;
	V.y	= cr_Vector.y * sin_a;
	V.z	= cr_Vector.z * sin_a;
	w		= cos_a;
}

// +---------
// | unary -
// +---------
inline Quaternion Quaternion::operator-() const
{
	return Quaternion(-V, -w);
}

// +---------
// | Addition
// +---------
inline Quaternion operator+ (Quaternion q1, const Quaternion& q2) 
{
	q1 += q2;
	return q1;
}

// +---------
// | Substraction
// +---------
inline Quaternion operator- (Quaternion q1, const Quaternion& q2)
{
	q1 -= q2;
	return q1;
}


// +---------
// | Multiplication of 2 quaternion (if they repesent a rotation, then this product
// | represents the composition of those 2 rotations)
// +---------
inline Quaternion& Quaternion::operator*= (const Quaternion& b)
{
	Quaternion a = *this;
	w     =   a.w   *   b.w     -   a.V.x *   b.V.x
		-   a.V.y *   b.V.y   -   a.V.z *   b.V.z;
	
	V.x   =   a.w   *   b.V.x   +   a.V.x *   b.w
		+   a.V.y *   b.V.z   -   a.V.z *   b.V.y;
	
	V.y   =   a.w   *   b.V.y   +   a.V.y *   b.w
		+   a.V.z *   b.V.x   -   a.V.x *   b.V.z;
	
	V.z   =   a.w   *   b.V.z   +   a.V.z *   b.w
		+   a.V.x *   b.V.y   -   a.V.y *   b.V.x;
	
	
	return *this;
}

inline Quaternion operator* (Quaternion q1, const Quaternion& q2) 
{
	q1 *= q2;
	return q1;
}




// +---------
// | Adds a quaternion to himself
// +---------
inline Quaternion& Quaternion::operator+= (const Quaternion& q)
{
	v4 += q.v4;
	return *this;
}

// +---------
// | Substract a quaternion to Himself
// +---------
inline Quaternion& Quaternion::operator-= (const Quaternion& q)
{
	v4 -= q.v4;
	return *this;
}


// +---------
// | returns the inverse quaternion of cr_Quaternion ~~~~equilvalent to the conjugate of the quaternion
// +---------
inline Quaternion Inv(const Quaternion& cr_Quaternion)
{
/*
#ifdef _DEBUG
	if (absF(Norm(cr_Quaternion)-1.0f)>1e-6)
	{	
		// The quaternion is supposed to be Normalized (i.e. Represents a Rotation) so
		// if its norm!=1.0 this quaternion does not represent a rotation....
		assert(0);
	}
	
#endif
*/
	return Quaternion(-cr_Quaternion.V, cr_Quaternion.w);
}

// +---------
// | Multiply a quaternion with  a Float
// +---------
inline Quaternion operator* (Quaternion q, const Float& f)
{
	q *= f;
	return q;
}

// +---------
// | Multiply a Float with a Quaternion
// +---------
inline Quaternion operator*( const Float& f, Quaternion q)
{
	q *= f;
	return q;
}


// +---------
// | divise a Quaternion by a Float
// +---------
inline Quaternion operator/ (Quaternion q, const Float& f) 
{
	q /= f;
	return q;
}


// +---------
// | multiply himself with a Float
// +---------
inline Quaternion& Quaternion::operator*= (const Float& f)
{
	v4 *= f;
	return *this;
}


// +---------
// | divide himself by a Float
// +---------
inline Quaternion& Quaternion::operator/= (const Float& f)
{
	v4 /= f;
	return *this;
}

inline Point3D operator*(const Quaternion& q, const Point3D& v)
{
	Matrix3x3 mat = q.ToMatrix();
	return mat * v;
}

// +---------
// | Acces Operators
// +---------
inline const Float& Quaternion::operator[](Int i) const
{
	assert(i >= 0 && i<3);
	return *((&(V.x)) + i);
}

inline Float& Quaternion::operator[](Int i)
{
	assert(i >= 0 && i<3);
	return *((&(V.x)) + i);
}


// +---------
// | Dot Product
// +---------
inline Float Dot(const Quaternion& cr_Quaternion1, const Quaternion& cr_Quaternion2)
{
	return Dot(cr_Quaternion1.V,cr_Quaternion2.V) + cr_Quaternion1.w*cr_Quaternion2.w;
}


// +---------
// | Spherical Interpolation ~ (1-cr_weight)*cr_Quaternion1 + cr_weight*cr_Quaternion2
// +---------
inline Quaternion Slerp(const Quaternion& cr_Quaternion1, const Quaternion& cr_Quaternion2, const Float& cr_weight)
{
	return SlerpNearest(cr_Quaternion1,cr_Quaternion2,cr_weight);
	/*		Float w = acos(Dot(cr_Quaternion1,cr_Quaternion2));
	  Float s_inv = (static_cast<Float>(1.0))/sin(w);
	  Quaternion QuatTmp;
	  QuatTmp = cr_Quaternion1*s_inv*sin((1-cr_weight)*w);
	  Quaternion::ms_Quaternion = cr_Quaternion2;
	  Quaternion::ms_Quaternion *= s_inv*sin(cr_weight*w);
	  Quaternion::ms_Quaternion += QuatTmp;
	  return Quaternion::ms_Quaternion;
	  
	*/
	Float omega,cosom,sinom,sclp,sclq;
	IntU32 i;
	
	Quaternion result;
	
    const Quaternion& p=cr_Quaternion1;
    const Quaternion& q=cr_Quaternion2;
    const Float& t=cr_weight;
	
    cosom = Dot(p,q);
	
	if (  cosom > (0.000001f-1.0f) )
	{
		// usual case
		if (  cosom < (1.0f-0.000001f) )
		{
			//usual case
			omega = (Float)acosF(cosom);
            // as cosom > -0.999999 ans cosom < 0.999999 sinom != 0
            // so we can invert it without test
			sinom = 1.0f/(Float)sinF(omega);
			sclp = (Float)sinF((1.0f - t)*omega) * sinom;
			sclq = (Float)sinF(t*omega) * sinom;
		}
		else
		{	// ends very close -- just lerp
			sclp = 1.0f - t;
			sclq = t;
		}
		result.V.x = sclp*p.V.x + sclq*q.V.x;
        result.V.y = sclp*p.V.y + sclq*q.V.y;
        result.V.z = sclp*p.V.z + sclq*q.V.z;
        result.w = sclp*p.w + sclq*q.w;
	}
	else
	{
		/* p and q nearly opposite on sphere-- this is a 360 degree
		rotation, but the axis of rotation is undefined, so
		slerp really is undefined too.  So this apparently picks
		an arbitrary plane of rotation. However, I think this
		code is incorrect.*/
		
        // this case should not happen and is not optimised
		
		result.V.x = -p.V.y;
        result.V.y = p.V.x;  // qt is orthogonal to p
		result.V.z = -p.w;
        result.w = p.V.z;
		
		sclp = sinF((Float)(((Float)1.0f - t) * PI_2));
		sclq = (Float)sinF( (Float)(t * (Float)PI_2));
		for (i = 0; i<3; i++) (&result.V.x)[i] = sclp*(&p.V.x)[i] + sclq*(&result.V.x)[i];
	}
	
	return result;
	
}

inline Quaternion SlerpNearest(const Quaternion& p, const Quaternion& qr, const Float& t)
{
	Float omega,cosom,sinom,sclp,sclq;
	
	Quaternion result;
	Quaternion q;
    cosom = Dot(p,qr);
	
	if (cosom < 0) // we use -q, to interpolate through the shortest path
	{
		q = -qr;
		cosom = Dot(p, q);
	}
	else
	{
		q = qr;
	}
	if (  cosom < (1.0f-0.000001f) )
	{
		//usual case
		omega = (Float)acosF(cosom);
        // as cosom > -0.999999 ans cosom < 0.999999 sinom != 0
        // so we can invert it without test
		sinom = 1.0f/(Float)sin(omega);
		sclp = (Float)sinF((1.0f - t)*omega) * sinom;
		sclq = (Float)sinF(t*omega) * sinom;
	}
	else
	{
		// ends very close -- just lerp
		sclp = 1.0f - t;
		sclq = t;
	}
	result.V.x = sclp*p.V.x + sclq*q.V.x;
    result.V.y = sclp*p.V.y + sclq*q.V.y;
    result.V.z = sclp*p.V.z + sclq*q.V.z;
    result.w = sclp*p.w + sclq*q.w;
	
	return result;
}

// +---------------------
// | Quaternion Log
// +---------------------

Vector3D Log(const Quaternion& q)
{
	// NB: Log is only valid on an unit quaternion, that is on a rotation,
	// the returned vector being rotation_axis * rotation_angle
	const Float norm = Norm(q.V);
	if (norm == 0) return q.V;
	return q.V * (2*acosF(q.w)/norm);
}

Quaternion Exp(const Vector3D& v)
{
    // the norm is equal to the angle ...
	
    Float norm = Norm(v) ;
	
    // if v is null, we must return the Identity quaternion
	
    Quaternion res ;
	
    if (norm==0)
    {
        res.SetIdentity() ;
    }
    else
    {
        res.SetRotationU(v/norm,norm) ;
    }
	
    return res ;
}

// +------------------------------------------
// | Quaternion spherical quadratic interpolation
// +------------------------------------------

// return Slerp(Slerp(p,q,t), Slerp(a,b,t), 2*t*(1-t))

Quaternion Squad(Float t, const Quaternion& p, const Quaternion& q,
				 const Quaternion& a, const Quaternion& b)
{
    return Slerp(Slerp(p,q,t), Slerp(a,b,t), 2*t*(1-t)) ;
}

// use Squad with optimal values

Quaternion SmoothSplineInterpolation(Float t, const Quaternion& q0, const Quaternion& q1,
									 const Quaternion& q2, const Quaternion& q3)
{
    Quaternion inv_q1 (Inv(q1)) ;
	
    Quaternion a (q1*(Exp(-(Log(inv_q1*q0)+Log(inv_q1*q2))/4))) ;
	
    Quaternion inv_q2 (Inv(q2)) ;
	
    Quaternion b (q2*(Exp(-(Log(inv_q2*q1)+Log(inv_q2*q3))/4))) ;
	
    return Squad(t, q1, q2, a, b) ;
	
}

Quaternion SmoothSplineInterpolationNearest(Float t, const Quaternion& tq0, const Quaternion& q1,
											const Quaternion& tq2, const Quaternion& tq3)
{
	Quaternion q2 = ((Dot(q1, tq2) < 0)?-tq2:tq2);
	Quaternion q3 = ((Dot(q2, tq3) < 0)?-tq3:tq3);
	Quaternion q0 = ((Dot(q1, tq0) < 0)?-tq0:tq0);
	
    Quaternion inv_q1 (Inv(q1)) ;
	
    Quaternion a (q1*(Exp(-(Log(inv_q1*q0)+Log(inv_q1*q2))/4))) ;
	
    Quaternion inv_q2 (Inv(q2)) ;
	
    Quaternion b (q2*(Exp(-(Log(inv_q2*q1)+Log(inv_q2*q3))/4))) ;
	
    return Squad(t, q1, q2, a, b) ;
	
}

// +-----
// | Rotation arc
// +-----

Quaternion RotationArc(const Vector3D& v0, const Vector3D& v1)
{
	Quaternion q;
	Vector3D v0n, v1n;
	Vector3D c;
	Float d;
	v0n = v0; v0n.Normalize();
	v1n = v1; v1n.Normalize();
	c.CrossProduct(v0n, v1n);
	d = Dot(v0n, v1n);
	q.w = sqrtF((1+d)*2);
	if (q.w <= 0) // with the error margin, we have a 180deg rotation, so we can't guess the axis to use.
	{
		q.V.x = q.V.y = q.w = 0;
		q.V.z = 1;
		return q;
	}
	d = 1 / q.w; q.w *= 0.5f;
	q.V.x = c.x * d;
	q.V.y = c.y * d;
	q.V.z = c.z * d;
	return q;
}


