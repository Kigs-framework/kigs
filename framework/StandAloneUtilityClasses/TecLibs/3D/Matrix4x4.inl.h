// +-----
// | Init
// +-----

void Matrix4x4::Set(const Float* data)
{
	memcpy(e, data, sizeof(Float) * 16);
}

void Matrix4x4::Set(const Matrix4x4& mat)
{
	memcpy(e, mat.e, sizeof(Float) * 16);	
}

void Matrix4x4::Set(const Matrix3x4& mat)
{
	e[0][0] = mat.e[0][0] ; e[1][0] = mat.e[1][0] ; e[2][0] = mat.e[2][0]; e[3][0] = mat.e[3][0];
    e[0][1] = mat.e[0][1] ; e[1][1] = mat.e[1][1] ; e[2][1] = mat.e[2][1]; e[3][1] = mat.e[3][1];
    e[0][2] = mat.e[0][2] ; e[1][2] = mat.e[1][2] ; e[2][2] = mat.e[2][2]; e[3][2] = mat.e[3][2];
    e[0][3] = e[1][3] = e[2][3] = 0;
	e[3][3] = 1;
}

void Matrix4x4::Set(const Matrix3x3& mat)
{
	e[0][0] = mat.e[0][0] ; e[1][0] = mat.e[1][0] ; e[2][0] = mat.e[2][0];
    e[0][1] = mat.e[0][1] ; e[1][1] = mat.e[1][1] ; e[2][1] = mat.e[2][1];
    e[0][2] = mat.e[0][2] ; e[1][2] = mat.e[1][2] ; e[2][2] = mat.e[2][2];
    e[3][0] = e[3][1] = e[3][2] = e[0][3] = e[1][3] = e[2][3] = KFLOAT_ZERO;
	e[3][3] = KFLOAT_ONE;
}

void Matrix4x4::Set(const Quaternion& q, const Vector3D& v)
{
	Float dx = KFLOAT_CONST(2.0f) * q.V.x;
	Float dy = KFLOAT_CONST(2.0f) * q.V.y;
	Float dz = KFLOAT_CONST(2.0f) * q.V.z;
    Float xx = dx * q.V.x;
	Float xy = dx * q.V.y;
	Float xz = dx * q.V.z;
	Float xw = dx * q.w;
	Float yy = dy * q.V.y;
	Float yz = dy * q.V.z;
	Float yw = dy * q.w;
	Float zz = dz * q.V.z;
	Float zw = dz * q.w;
	
	e[0][0]  = KFLOAT_ONE - ( yy + zz );
	e[1][0]  =     ( xy - zw );
	e[2][0]  =     ( xz + yw );
	
	e[0][1]  =     ( xy + zw );
	e[1][1]  = KFLOAT_ONE - ( xx + zz );
	e[2][1]  =     ( yz - xw );
	
	e[0][2]  =     ( xz - yw );
	e[1][2]  =     ( yz + xw );
	e[2][2]  = KFLOAT_ONE - ( xx + yy );	
	
	e[3][0]  = v.x;
	e[3][1]  = v.y;
	e[3][2]  = v.z;
	
	e[3][0]  = e[3][1] = e[3][2] =
		e[0][3]  = e[1][3] = e[2][3] = KFLOAT_ZERO;
	e[3][3]  = KFLOAT_ONE;
}

void	Matrix4x4::Set(const Vector4D& v0, const Vector4D& v1, const Vector4D& v2, const Vector4D& v3)
{
	e[0][0] = v0.x; e[0][1] = v0.y; e[0][2] = v0.z; e[0][3] = v0.w;
	e[1][0] = v1.x; e[1][1] = v1.y; e[1][2] = v1.z; e[1][3] = v1.w;
	e[2][0] = v2.x; e[2][1] = v2.y; e[2][2] = v2.z; e[2][3] = v2.w;
	e[3][0] = v3.x; e[3][1] = v3.y; e[3][2] = v3.z; e[3][3] = v3.w;
}

void	Matrix4x4::Set( const Float& e00, const Float& e10, const Float& e20, const Float& e30,
					const Float& e01, const Float& e11, const Float& e21, const Float& e31,
					const Float& e02, const Float& e12, const Float& e22, const Float& e32,
					const Float& e03, const Float& e13, const Float& e23, const Float& e33 )
{
    e[0][0] = e00 ; e[1][0] = e10 ; e[2][0] = e20 ; e[3][0] = e30 ;
    e[0][1] = e01 ; e[1][1] = e11 ; e[2][1] = e21 ; e[3][1] = e31 ;
    e[0][2] = e02 ; e[1][2] = e12 ; e[2][2] = e22 ; e[3][2] = e32 ;
    e[0][3] = e03 ; e[1][3] = e13 ; e[2][3] = e23 ; e[3][3] = e33 ;
}

void	Matrix4x4::SetNull()
{
	e[0][0] = e[0][1] = e[0][2] = e[0][3] =
		e[1][0] = e[1][1] = e[1][2] = e[1][3] =
		e[2][0] = e[2][1] = e[2][2] = e[2][3] =
		e[3][0] = e[3][1] = e[3][2] = e[3][3] = 0;
}

void	Matrix4x4::SetIdentity()
{
	e[0][1] = e[0][2] = e[0][3] =
		e[1][0] = e[1][2] = e[1][3] =
		e[2][0] = e[2][1] = e[2][3] =
		e[3][0] = e[3][1] = e[3][2] = 0;
	e[0][0] = e[1][1] = e[2][2] = e[3][3] = 1;
}

void	Matrix4x4::SetInvert()
{
	Matrix4x4 mat = *this;
	
	float* inv = &e[0][0];
	const float* m=&mat.e[0][0];
	float  det;
	int i;
	
	inv[0] = m[5] * m[10] * m[15] -
		m[5] * m[11] * m[14] -
		m[9] * m[6] * m[15] +
		m[9] * m[7] * m[14] +
		m[13] * m[6] * m[11] -
		m[13] * m[7] * m[10];
	
	inv[4] = -m[4] * m[10] * m[15] +
		m[4] * m[11] * m[14] +
		m[8] * m[6] * m[15] -
		m[8] * m[7] * m[14] -
		m[12] * m[6] * m[11] +
		m[12] * m[7] * m[10];
	
	inv[8] = m[4] * m[9] * m[15] -
		m[4] * m[11] * m[13] -
		m[8] * m[5] * m[15] +
		m[8] * m[7] * m[13] +
		m[12] * m[5] * m[11] -
		m[12] * m[7] * m[9];
	
	inv[12] = -m[4] * m[9] * m[14] +
		m[4] * m[10] * m[13] +
		m[8] * m[5] * m[14] -
		m[8] * m[6] * m[13] -
		m[12] * m[5] * m[10] +
		m[12] * m[6] * m[9];
	
	inv[1] = -m[1] * m[10] * m[15] +
		m[1] * m[11] * m[14] +
		m[9] * m[2] * m[15] -
		m[9] * m[3] * m[14] -
		m[13] * m[2] * m[11] +
		m[13] * m[3] * m[10];
	
	inv[5] = m[0] * m[10] * m[15] -
		m[0] * m[11] * m[14] -
		m[8] * m[2] * m[15] +
		m[8] * m[3] * m[14] +
		m[12] * m[2] * m[11] -
		m[12] * m[3] * m[10];
	
	inv[9] = -m[0] * m[9] * m[15] +
		m[0] * m[11] * m[13] +
		m[8] * m[1] * m[15] -
		m[8] * m[3] * m[13] -
		m[12] * m[1] * m[11] +
		m[12] * m[3] * m[9];
	
	inv[13] = m[0] * m[9] * m[14] -
		m[0] * m[10] * m[13] -
		m[8] * m[1] * m[14] +
		m[8] * m[2] * m[13] +
		m[12] * m[1] * m[10] -
		m[12] * m[2] * m[9];
	
	inv[2] = m[1] * m[6] * m[15] -
		m[1] * m[7] * m[14] -
		m[5] * m[2] * m[15] +
		m[5] * m[3] * m[14] +
		m[13] * m[2] * m[7] -
		m[13] * m[3] * m[6];
	
	inv[6] = -m[0] * m[6] * m[15] +
		m[0] * m[7] * m[14] +
		m[4] * m[2] * m[15] -
		m[4] * m[3] * m[14] -
		m[12] * m[2] * m[7] +
		m[12] * m[3] * m[6];
	
	inv[10] = m[0] * m[5] * m[15] -
		m[0] * m[7] * m[13] -
		m[4] * m[1] * m[15] +
		m[4] * m[3] * m[13] +
		m[12] * m[1] * m[7] -
		m[12] * m[3] * m[5];
	
	inv[14] = -m[0] * m[5] * m[14] +
		m[0] * m[6] * m[13] +
		m[4] * m[1] * m[14] -
		m[4] * m[2] * m[13] -
		m[12] * m[1] * m[6] +
		m[12] * m[2] * m[5];
	
	inv[3] = -m[1] * m[6] * m[11] +
		m[1] * m[7] * m[10] +
		m[5] * m[2] * m[11] -
		m[5] * m[3] * m[10] -
		m[9] * m[2] * m[7] +
		m[9] * m[3] * m[6];
	
	inv[7] = m[0] * m[6] * m[11] -
		m[0] * m[7] * m[10] -
		m[4] * m[2] * m[11] +
		m[4] * m[3] * m[10] +
		m[8] * m[2] * m[7] -
		m[8] * m[3] * m[6];
	
	inv[11] = -m[0] * m[5] * m[11] +
		m[0] * m[7] * m[9] +
		m[4] * m[1] * m[11] -
		m[4] * m[3] * m[9] -
		m[8] * m[1] * m[7] +
		m[8] * m[3] * m[5];
	
	inv[15] = m[0] * m[5] * m[10] -
		m[0] * m[6] * m[9] -
		m[4] * m[1] * m[10] +
		m[4] * m[2] * m[9] +
		m[8] * m[1] * m[6] -
		m[8] * m[2] * m[5];
	
	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
	
	if (det == 0)
	{
		SetIdentity();
		return;
	}
	
	det = 1.0f / det;
	
	for (i = 0; i < 16; i++)
		inv[i] = inv[i] * det;
}

inline Matrix4x4 Inv(Matrix4x4 m)
{
	m.SetInvert();
	return m;
}

inline Float Trace(const Matrix4x4& m)
{
	Float result = 0.0f;
	for(int i=0; i<4; ++i)
		result += m.e[i][i];
	return result;
}

// +-----
// | Operators
// +-----

Vector4D operator* (const Vector4D& v, const Matrix4x4& m)
{
    return Vector4D(m.e[0][0]*v.x + m.e[0][1]*v.y + m.e[0][2]*v.z + m.e[0][3]*v.w,
					m.e[1][0]*v.x + m.e[1][1]*v.y + m.e[1][2]*v.z + m.e[1][3]*v.w,
					m.e[2][0]*v.x + m.e[2][1]*v.y + m.e[2][2]*v.z + m.e[2][3]*v.w,
					m.e[3][0]*v.x + m.e[3][1]*v.y + m.e[3][2]*v.z + m.e[3][3]*v.w) ;
}

Vector4D operator* (const Matrix4x4& m, const Vector4D& v)
{
    return Vector4D(m.e[0][0]*v.x + m.e[1][0]*v.y + m.e[2][0]*v.z + m.e[3][0]*v.w,
					m.e[0][1]*v.x + m.e[1][1]*v.y + m.e[2][1]*v.z + m.e[3][1]*v.w,
					m.e[0][2]*v.x + m.e[1][2]*v.y + m.e[2][2]*v.z + m.e[3][2]*v.w,
					m.e[0][3]*v.x + m.e[1][3]*v.y + m.e[2][3]*v.z + m.e[3][3]*v.w) ;
}

Vector4D operator* (const Vector3D& v, const Matrix4x4& m)
{
    return Vector4D(m.e[0][0]*v.x + m.e[0][1]*v.y + m.e[0][2]*v.z,
					m.e[1][0]*v.x + m.e[1][1]*v.y + m.e[1][2]*v.z,
					m.e[2][0]*v.x + m.e[2][1]*v.y + m.e[2][2]*v.z,
					m.e[3][0]*v.x + m.e[3][1]*v.y + m.e[3][2]*v.z) ;
}

Vector4D operator* (const Matrix4x4& m, const Vector3D& v)
{
    return Vector4D(m.e[0][0]*v.x + m.e[1][0]*v.y + m.e[2][0]*v.z,
					m.e[0][1]*v.x + m.e[1][1]*v.y + m.e[2][1]*v.z,
					m.e[0][2]*v.x + m.e[1][2]*v.y + m.e[2][2]*v.z,
					m.e[0][3]*v.x + m.e[1][3]*v.y + m.e[2][3]*v.z) ;
}

Vector4D operator* (const Point3D& v, const Matrix4x4& m)
{
    return Vector4D(m.e[0][0]*v.x + m.e[0][1]*v.y + m.e[0][2]*v.z + m.e[0][3],
					m.e[1][0]*v.x + m.e[1][1]*v.y + m.e[1][2]*v.z + m.e[1][3],
					m.e[2][0]*v.x + m.e[2][1]*v.y + m.e[2][2]*v.z + m.e[2][3],
					m.e[3][0]*v.x + m.e[3][1]*v.y + m.e[3][2]*v.z + m.e[3][3]) ;
}

Vector4D operator* (const Matrix4x4& m, const Point3D& v)
{
    return Vector4D(m.e[0][0]*v.x + m.e[1][0]*v.y + m.e[2][0]*v.z + m.e[3][0],
					m.e[0][1]*v.x + m.e[1][1]*v.y + m.e[2][1]*v.z + m.e[3][1],
					m.e[0][2]*v.x + m.e[1][2]*v.y + m.e[2][2]*v.z + m.e[3][2],
					m.e[0][3]*v.x + m.e[1][3]*v.y + m.e[2][3]*v.z + m.e[3][3]);
}


Matrix4x4& Matrix4x4::operator*=(const Float& f)
{
	for(int i=0; i<4; ++i)
	{
		for(int j=0; j<4; ++j)
		{
			e[i][j] *= f;
		}
	}
	return *this;
}

Matrix4x4 operator*(Matrix4x4 m, const Float& f)
{
	m *= f;
	return m;
}

Matrix4x4 operator*(const Float& f, Matrix4x4 m)
{
	m *= f;
	return m;
}



inline Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& crMatrix)
{
	Float a, b,c;
	a = e[0][0]; b = e[1][0]; c=e[2][0];
	e[0][0] = a*crMatrix.e[0][0]+b*crMatrix.e[0][1]+c*crMatrix.e[0][2]+e[3][0]*crMatrix.e[0][3];
	e[1][0] = a*crMatrix.e[1][0]+b*crMatrix.e[1][1]+c*crMatrix.e[1][2]+e[3][0]*crMatrix.e[1][3];
	e[2][0] = a*crMatrix.e[2][0]+b*crMatrix.e[2][1]+c*crMatrix.e[2][2]+e[3][0]*crMatrix.e[2][3];
	e[3][0] = a*crMatrix.e[3][0]+b*crMatrix.e[3][1]+c*crMatrix.e[3][2]+e[3][0]*crMatrix.e[3][3];
	
	a = e[0][1]; b = e[1][1]; c=e[2][1];
	e[0][1] = a*crMatrix.e[0][0]+b*crMatrix.e[0][1]+c*crMatrix.e[0][2]+e[3][1]*crMatrix.e[0][3];
	e[1][1] = a*crMatrix.e[1][0]+b*crMatrix.e[1][1]+c*crMatrix.e[1][2]+e[3][1]*crMatrix.e[1][3];
	e[2][1] = a*crMatrix.e[2][0]+b*crMatrix.e[2][1]+c*crMatrix.e[2][2]+e[3][1]*crMatrix.e[2][3];
	e[3][1] = a*crMatrix.e[3][0]+b*crMatrix.e[3][1]+c*crMatrix.e[3][2]+e[3][1]*crMatrix.e[3][3];
	
	a = e[0][2]; b = e[1][2]; c=e[2][2];
	e[0][2] = a*crMatrix.e[0][0]+b*crMatrix.e[0][1]+c*crMatrix.e[0][2]+e[3][2]*crMatrix.e[0][3];
	e[1][2] = a*crMatrix.e[1][0]+b*crMatrix.e[1][1]+c*crMatrix.e[1][2]+e[3][2]*crMatrix.e[1][3];
	e[2][2] = a*crMatrix.e[2][0]+b*crMatrix.e[2][1]+c*crMatrix.e[2][2]+e[3][2]*crMatrix.e[2][3];
	e[3][2] = a*crMatrix.e[3][0]+b*crMatrix.e[3][1]+c*crMatrix.e[3][2]+e[3][2]*crMatrix.e[3][3];
	
	a = e[0][3]; b = e[1][3]; c=e[2][3];
	e[0][3] = a*crMatrix.e[0][0]+b*crMatrix.e[0][1]+c*crMatrix.e[0][2]+e[3][3]*crMatrix.e[0][3];
	e[1][3] = a*crMatrix.e[1][0]+b*crMatrix.e[1][1]+c*crMatrix.e[1][2]+e[3][3]*crMatrix.e[1][3];
	e[2][3] = a*crMatrix.e[2][0]+b*crMatrix.e[2][1]+c*crMatrix.e[2][2]+e[3][3]*crMatrix.e[2][3];
	e[3][3] = a*crMatrix.e[3][0]+b*crMatrix.e[3][1]+c*crMatrix.e[3][2]+e[3][3]*crMatrix.e[3][3];
	
	return *this;
}

inline Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& crMatrix)
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			e[i][j] += crMatrix.e[i][j];
		}
	}
	return *this;
}

inline Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& crMatrix)
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			e[i][j] -= crMatrix.e[i][j];
		}
	}
	return *this;
}


inline Matrix4x4 operator*(Matrix4x4 left, const Matrix4x4& right)
{
	left *= right;
	return left;
}

inline Matrix4x4 operator+(Matrix4x4 left, const Matrix4x4& right)
{
	left += right;
	return left;
}

inline Matrix4x4 operator-(Matrix4x4 left, const Matrix4x4& right)
{
	left -= right;
	return left;
}

inline void	Matrix4x4::Mult(const Matrix4x4& m1,const Matrix4x4& m2)
{
	float a,b,c,d;
	
	int j;
	for(j=0;j<4;j++)
	{
		a=m1.e[j][0]*m2.e[0][0];
		b=m1.e[j][1]*m2.e[1][0];
		c=m1.e[j][2]*m2.e[2][0];
		d=m1.e[j][3]*m2.e[3][0];
		e[j][0] = a+b+c+d;
		
		a=m1.e[j][0]*m2.e[0][1];
		b=m1.e[j][1]*m2.e[1][1];
		c=m1.e[j][2]*m2.e[2][1];
		d=m1.e[j][3]*m2.e[3][1];
		e[j][1] = a+b+c+d;
		
		a=m1.e[j][0]*m2.e[0][2];
		b=m1.e[j][1]*m2.e[1][2];
		c=m1.e[j][2]*m2.e[2][2];
		d=m1.e[j][3]*m2.e[3][2];
		e[j][2] = a+b+c+d;
		
		a=m1.e[j][0]*m2.e[0][3];
		b=m1.e[j][1]*m2.e[1][3];
		c=m1.e[j][2]*m2.e[2][3];
		d=m1.e[j][3]*m2.e[3][3];
		e[j][3] = a+b+c+d;
	}
	
	
}
