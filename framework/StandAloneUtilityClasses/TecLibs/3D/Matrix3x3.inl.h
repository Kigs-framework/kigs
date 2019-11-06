// ******************************
// * Default constructor
// *-----------------------------
// ******************************
Matrix3x3::Matrix3x3()
{
	;
}


// ******************************
// * Copy constructor
// *-----------------------------
// ******************************
#include <string.h>

// ******************************
// * Constructors
// ******************************

inline Matrix3x3::Matrix3x3(const Matrix3x3& mat)
{
	Set(mat);
}

inline void Matrix3x3::Set(const Vector3D& vx, const Vector3D& vy, const Vector3D& vz)
{
	e[0][0] = vx.x; e[0][1] = vx.y; e[0][2] = vx.z;
	e[1][0] = vy.x; e[1][1] = vy.y; e[1][2] = vy.z;
	e[2][0] = vz.x; e[2][1] = vz.y; e[2][2] = vz.z;
}


inline Matrix3x3::Matrix3x3(const Quaternion& cr_Quaternion)
{
	Set(cr_Quaternion);
}

inline void Matrix3x3::SetIdentity()
{
	e[0][0] = e[1][1] = e[2][2] = KFLOAT_ONE;
	e[1][0] = e[2][0] = e[0][1] = e[2][1] = e[0][2] = e[1][2] = KFLOAT_ZERO;
}

inline void Matrix3x3::Set(const Matrix3x3& mat)
{
	memcpy(e, mat.e, 9*sizeof(Float));
}

inline void Matrix3x3::Set(const Quaternion& cr_Quaternion)
{
	Float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
	
	x2 = cr_Quaternion.V.x + cr_Quaternion.V.x; y2 = cr_Quaternion.V.y + cr_Quaternion.V.y; z2 = cr_Quaternion.V.z + cr_Quaternion.V.z;
	xx = cr_Quaternion.V.x * x2;   xy = cr_Quaternion.V.x * y2;   xz = cr_Quaternion.V.x * z2;
	yy = cr_Quaternion.V.y * y2;   yz = cr_Quaternion.V.y * z2;   zz = cr_Quaternion.V.z * z2;
	wx = cr_Quaternion.w * x2;   wy = cr_Quaternion.w * y2;   wz = cr_Quaternion.w * z2;
	
	e[0][0] = KFLOAT_ONE - (yy + zz);
	e[0][1] = xy - wz;
	e[0][2] = xz + wy;
	
	e[1][0] = xy + wz;
	e[1][1] = KFLOAT_ONE - (xx + zz);
	e[1][2] = yz - wx;
	
	e[2][0] = xz - wy;
	e[2][1] = yz + wx;
	e[2][2] = KFLOAT_ONE - (xx + yy);
}

inline const Matrix3x3& Matrix3x3::operator=(const Matrix3x3& mat)
{
	Set(mat);
	return *this;
}

inline const Matrix3x3& Matrix3x3::operator=(const Quaternion& crQuaternion)
{
	Set(crQuaternion);
	return *this;
}

// ******************************
// * Invert
// ******************************

inline Matrix3x3 Inv(const Matrix3x3 & cr_Matrix)
{
	Matrix3x3 mat;
	Float t4 = cr_Matrix.e[0][0]*cr_Matrix.e[1][1];
	Float t6 = cr_Matrix.e[0][0]*cr_Matrix.e[2][1];
	Float t8 = cr_Matrix.e[1][0]*cr_Matrix.e[0][1];
	Float t10 = cr_Matrix.e[2][0]*cr_Matrix.e[0][1];
	Float t12 = cr_Matrix.e[1][0]*cr_Matrix.e[0][2];
	Float t14 = cr_Matrix.e[2][0]*cr_Matrix.e[0][2];
	Float t17 = KFLOAT_ONE/(t4*cr_Matrix.e[2][2]-t6*cr_Matrix.e[1][2]-t8*cr_Matrix.e[2][2]+t10*cr_Matrix.e[1][2]+t12*cr_Matrix.e[2][1]-t14*cr_Matrix.e[1][1]);
	mat.e[0][0] = (cr_Matrix.e[1][1]*cr_Matrix.e[2][2]-cr_Matrix.e[2][1]*cr_Matrix.e[1][2])*t17;
	mat.e[0][1] = -(cr_Matrix.e[0][1]*cr_Matrix.e[2][2]-cr_Matrix.e[2][1]*cr_Matrix.e[0][2])*t17;
	mat.e[0][2] = (cr_Matrix.e[0][1]*cr_Matrix.e[1][2]-cr_Matrix.e[1][1]*cr_Matrix.e[0][2])*t17;
	mat.e[1][0] = -(cr_Matrix.e[1][0]*cr_Matrix.e[2][2]-cr_Matrix.e[2][0]*cr_Matrix.e[1][2])*t17;
	mat.e[1][1] = (cr_Matrix.e[0][0]*cr_Matrix.e[2][2]-t14)*t17;
	mat.e[1][2] = -(cr_Matrix.e[0][0]*cr_Matrix.e[1][2]-t12)*t17;
	mat.e[2][0] = (cr_Matrix.e[1][0]*cr_Matrix.e[2][1]-cr_Matrix.e[2][0]*cr_Matrix.e[1][1])*t17;
	mat.e[2][1] = -(t6-t10)*t17;
	mat.e[2][2] = (t4-t8)*t17;
	return mat;
}

inline Matrix3x3 Transpose(const Matrix3x3 & cr_Matrix)
{
	Matrix3x3 mat;
	mat.e[0][0] = cr_Matrix.e[0][0];
	mat.e[0][1] = cr_Matrix.e[1][0];
	mat.e[0][2] = cr_Matrix.e[2][0];
	mat.e[1][0] = cr_Matrix.e[0][1];
	mat.e[1][1] = cr_Matrix.e[1][1];
	mat.e[1][2] = cr_Matrix.e[2][1];
	mat.e[2][0] = cr_Matrix.e[0][2];
	mat.e[2][1] = cr_Matrix.e[1][2];
	mat.e[2][2] = cr_Matrix.e[2][2];
	return mat;
}

inline Float Det(const Matrix3x3& cr_Matrix)
{
	Float det = cr_Matrix.e[0][0]*cr_Matrix.e[1][1]*cr_Matrix.e[2][2]
		-cr_Matrix.e[0][0]*cr_Matrix.e[2][1]*cr_Matrix.e[1][2]
		-cr_Matrix.e[0][1]*cr_Matrix.e[1][0]*cr_Matrix.e[2][2]
		+cr_Matrix.e[0][1]*cr_Matrix.e[2][0]*cr_Matrix.e[1][2]
		+cr_Matrix.e[0][2]*cr_Matrix.e[1][0]*cr_Matrix.e[2][1]
		-cr_Matrix.e[0][2]*cr_Matrix.e[2][0]*cr_Matrix.e[1][1];
	return det;
}

inline Float Norm(const Matrix3x3 & cr_Matrix)
{
	return sqrtF(NormSquare(cr_Matrix));
}

inline Float NormSquare(const Matrix3x3 & cr_Matrix)
{
	Float ns =  cr_Matrix.e[0][0]*cr_Matrix.e[0][0] + cr_Matrix.e[0][1]*cr_Matrix.e[0][1] + cr_Matrix.e[0][2]*cr_Matrix.e[0][2]
		+ cr_Matrix.e[1][0]*cr_Matrix.e[1][0] + cr_Matrix.e[1][1]*cr_Matrix.e[1][1] + cr_Matrix.e[1][2]*cr_Matrix.e[1][2]
		+ cr_Matrix.e[2][0]*cr_Matrix.e[2][0] + cr_Matrix.e[2][1]*cr_Matrix.e[2][1] + cr_Matrix.e[2][2]*cr_Matrix.e[2][2];
	return ns;
}

inline Float Trace(const Matrix3x3 & cr_Matrix)
{
	Float tr =  cr_Matrix.e[0][0] + cr_Matrix.e[1][1] + cr_Matrix.e[2][2];
	return tr;
}


inline Matrix3x3 Matrix3x3::operator+ (const Matrix3x3& crMatrix)
{
	Matrix3x3 mat;
	mat.e[0][0] = e[0][0] + crMatrix.e[0][0];
	mat.e[0][1] = e[0][1] + crMatrix.e[0][1];
	mat.e[0][2] = e[0][2] + crMatrix.e[0][2];
	mat.e[1][0] = e[1][0] + crMatrix.e[1][0];
	mat.e[1][1] = e[1][1] + crMatrix.e[1][1];
	mat.e[1][2] = e[1][2] + crMatrix.e[1][2];
	mat.e[2][0] = e[2][0] + crMatrix.e[2][0];
	mat.e[2][1] = e[2][1] + crMatrix.e[2][1];
	mat.e[2][2] = e[2][2] + crMatrix.e[2][2];
	return mat;
}

inline Matrix3x3 Matrix3x3::operator- (const Matrix3x3& crMatrix)
{
	Matrix3x3 mat;
	mat.e[0][0] = e[0][0] - crMatrix.e[0][0];
	mat.e[0][1] = e[0][1] - crMatrix.e[0][1];
	mat.e[0][2] = e[0][2] - crMatrix.e[0][2];
	mat.e[1][0] = e[1][0] - crMatrix.e[1][0];
	mat.e[1][1] = e[1][1] - crMatrix.e[1][1];
	mat.e[1][2] = e[1][2] - crMatrix.e[1][2];
	mat.e[2][0] = e[2][0] - crMatrix.e[2][0];
	mat.e[2][1] = e[2][1] - crMatrix.e[2][1];
	mat.e[2][2] = e[2][2] - crMatrix.e[2][2];
	return mat;
}


inline Matrix3x3 Matrix3x3::operator* (const Matrix3x3& crMatrix)
{
	Matrix3x3 mat;
	mat.e[0][0] = e[0][0]*crMatrix.e[0][0]+e[1][0]*crMatrix.e[0][1]+e[2][0]*crMatrix.e[0][2];
	mat.e[0][1] = e[0][1]*crMatrix.e[0][0]+e[1][1]*crMatrix.e[0][1]+e[2][1]*crMatrix.e[0][2];
	mat.e[0][2] = e[0][2]*crMatrix.e[0][0]+e[1][2]*crMatrix.e[0][1]+e[2][2]*crMatrix.e[0][2];
	mat.e[1][0] = e[0][0]*crMatrix.e[1][0]+e[1][0]*crMatrix.e[1][1]+e[2][0]*crMatrix.e[1][2];
	mat.e[1][1] = e[0][1]*crMatrix.e[1][0]+e[1][1]*crMatrix.e[1][1]+e[2][1]*crMatrix.e[1][2];
	mat.e[1][2] = e[0][2]*crMatrix.e[1][0]+e[1][2]*crMatrix.e[1][1]+e[2][2]*crMatrix.e[1][2];
	mat.e[2][0] = e[0][0]*crMatrix.e[2][0]+e[1][0]*crMatrix.e[2][1]+e[2][0]*crMatrix.e[2][2];
	mat.e[2][1] = e[0][1]*crMatrix.e[2][0]+e[1][1]*crMatrix.e[2][1]+e[2][1]*crMatrix.e[2][2];
	mat.e[2][2] = e[0][2]*crMatrix.e[2][0]+e[1][2]*crMatrix.e[2][1]+e[2][2]*crMatrix.e[2][2];
	return mat;
}

// +---------
// | Unary Operator
// +---------
inline void Matrix3x3::operator-()
{
	e[0][0] = -e[0][0];
	e[0][1] = -e[1][0];
	e[0][2] = -e[2][0];
	e[1][0] = -e[0][1];
	e[1][1] = -e[1][1];
	e[1][2] = -e[2][1];
	e[2][0] = -e[0][2];
	e[2][1] = -e[1][2];
	e[2][2] = -e[2][2];
}

// +---------
// | Inner operators
// +---------
inline const Matrix3x3& Matrix3x3::operator+=(const Matrix3x3& crMatrix)
{
	e[0][0] += crMatrix.e[0][0];
	e[0][1] += crMatrix.e[1][0];
	e[0][2] += crMatrix.e[2][0];
	e[1][0] += crMatrix.e[0][1];
	e[1][1] += crMatrix.e[1][1];
	e[1][2] += crMatrix.e[2][1];
	e[2][0] += crMatrix.e[0][2];
	e[2][1] += crMatrix.e[1][2];
	e[2][2] += crMatrix.e[2][2];
	
	return *this;
}


inline const Matrix3x3& Matrix3x3::operator-=(const Matrix3x3& crMatrix)
{
	e[0][0] -= crMatrix.e[0][0];
	e[0][1] -= crMatrix.e[1][0];
	e[0][2] -= crMatrix.e[2][0];
	e[1][0] -= crMatrix.e[0][1];
	e[1][1] -= crMatrix.e[1][1];
	e[1][2] -= crMatrix.e[2][1];
	e[2][0] -= crMatrix.e[0][2];
	e[2][1] -= crMatrix.e[1][2];
	e[2][2] -= crMatrix.e[2][2];
	
	return *this;
}

inline const Matrix3x3& Matrix3x3::operator*=(const Matrix3x3& crMatrix)
{
	Float a, b;
	a = e[0][0]; b = e[1][0];
	e[0][0] = a*crMatrix.e[0][0]+b*crMatrix.e[0][1]+e[2][0]*crMatrix.e[0][2];
	e[1][0] = a*crMatrix.e[1][0]+b*crMatrix.e[1][1]+e[2][0]*crMatrix.e[1][2];
	e[2][0] = a*crMatrix.e[2][0]+b*crMatrix.e[2][1]+e[2][0]*crMatrix.e[2][2];
	
	a = e[0][1]; b = e[1][1];
	e[0][1] = a*crMatrix.e[0][0]+b*crMatrix.e[0][1]+e[2][1]*crMatrix.e[0][2];
	e[1][1] = a*crMatrix.e[1][0]+b*crMatrix.e[1][1]+e[2][1]*crMatrix.e[1][2];
	e[2][1] = a*crMatrix.e[2][0]+b*crMatrix.e[2][1]+e[2][1]*crMatrix.e[2][2];
	
	a = e[0][2]; b = e[1][2];
	e[0][2] = a*crMatrix.e[0][0]+b*crMatrix.e[0][1]+e[2][2]*crMatrix.e[0][2];
	e[1][2] = a*crMatrix.e[1][0]+b*crMatrix.e[1][1]+e[2][2]*crMatrix.e[1][2];
	e[2][2] = a*crMatrix.e[2][0]+b*crMatrix.e[2][1]+e[2][2]*crMatrix.e[2][2];
	
	return *this;
}


// +---------
// | Matrix operators with Vectors
// +---------
inline Vector3D Matrix3x3::operator*(const Vector3D& crVector) const
{
	return Vector3D(e[0][0]*crVector.x + e[1][0]*crVector.y + e[2][0]*crVector.z,
					e[0][1]*crVector.x + e[1][1]*crVector.y + e[2][1]*crVector.z,
					e[0][2]*crVector.x + e[1][2]*crVector.y + e[2][2]*crVector.z );
}

inline Point3D Matrix3x3::operator*(const Point3D& crVector) const
{
	return Point3D( e[0][0]*crVector.x + e[1][0]*crVector.y + e[2][0]*crVector.z,
				   e[0][1]*crVector.x + e[1][1]*crVector.y + e[2][1]*crVector.z,
				   e[0][2]*crVector.x + e[1][2]*crVector.y + e[2][2]*crVector.z );
}

inline void Matrix3x3::TransformPoints(Point3D* tab, int count) const
{
	TransformVectors((Vector3D*)tab,count);
}

inline void Matrix3x3::TransformVectors(Vector3D* tab, int count) const
{
	assert(tab);
	Float a, b;
	Vector3D* endtab = tab + count;
	for (; tab < endtab; tab++)
	{
		a = tab->x; b = tab->y;
		tab->x = a * e[0][0] + b * e[1][0] + tab->z * e[2][0];
		tab->y = a * e[0][1] + b * e[1][1] + tab->z * e[2][1];
		tab->z = a * e[0][2] + b * e[1][2] + tab->z * e[2][2];
	}
}

inline void Matrix3x3::TransformPoints(Point2D* tab, int count) const
{
	assert(tab);
	Float a, b;
	Point2D* endtab = tab + count;
	for (; tab < endtab; tab++)
	{
		a = tab->x; b = tab->y;
		tab->x = a * e[0][0] + b * e[1][0] + e[2][0];
		tab->y = a * e[0][1] + b * e[1][1] + e[2][1];
	}
}

inline void Matrix3x3::TransformVector(Vector3D* tab) const
{
	Float a, b;
	a = tab->x; b = tab->y;
	tab->x = a * e[0][0] + b * e[1][0] + tab->z * e[2][0];
	tab->y = a * e[0][1] + b * e[1][1] + tab->z * e[2][1];
	tab->z = a * e[0][2] + b * e[1][2] + tab->z * e[2][2];
	
}




inline Vector3D operator*(const Vector3D& v, const Matrix3x3& m)
{
	return Vector3D(m.e[0][0]*v.x + m.e[0][1]*v.y + m.e[0][2]*v.z,
					m.e[1][0]*v.x + m.e[1][1]*v.y + m.e[1][2]*v.z,
					m.e[2][0]*v.x + m.e[2][1]*v.y + m.e[2][2]*v.z );
}

inline Point3D  operator*(const Point3D& v, const Matrix3x3& m)
{
	return Point3D(	m.e[0][0]*v.x + m.e[0][1]*v.y + m.e[0][2]*v.z,
				   m.e[1][0]*v.x + m.e[1][1]*v.y + m.e[1][2]*v.z,
				   m.e[2][0]*v.x + m.e[2][1]*v.y + m.e[2][2]*v.z );
}


// +---------
// | Matrix operators with Floats
// +---------
inline const Matrix3x3 Matrix3x3::operator*(const Float& crFloat)
{
	Matrix3x3 mat;
	mat.e[0][0] = e[0][0] * crFloat;
	mat.e[0][1] = e[0][1] * crFloat;
	mat.e[0][2] = e[0][2] * crFloat;
	mat.e[1][0] = e[1][0] * crFloat;
	mat.e[1][1] = e[1][1] * crFloat;
	mat.e[1][2] = e[1][2] * crFloat;
	mat.e[2][0] = e[2][0] * crFloat;
	mat.e[2][1] = e[2][1] * crFloat;
	mat.e[2][2] = e[2][2] * crFloat;
	return mat;
}

inline const Matrix3x3 operator*(const Matrix3x3& crMatrix, const Float& crFloat)
{
	Matrix3x3 mat;
	mat.e[0][0] = crFloat * crMatrix.e[0][0];
	mat.e[0][1] = crFloat * crMatrix.e[1][0];
	mat.e[0][2] = crFloat * crMatrix.e[2][0];
	mat.e[1][0] = crFloat * crMatrix.e[0][1];
	mat.e[1][1] = crFloat * crMatrix.e[1][1];
	mat.e[1][2] = crFloat * crMatrix.e[2][1];
	mat.e[2][0] = crFloat * crMatrix.e[0][2];
	mat.e[2][1] = crFloat * crMatrix.e[1][2];
	mat.e[2][2] = crFloat * crMatrix.e[2][2];
	return mat;
}

inline const Matrix3x3& Matrix3x3::operator*=(const Float& crFloat)
{
	e[0][0] *= crFloat;
	e[0][1] *= crFloat;
	e[0][2] *= crFloat;
	e[1][0] *= crFloat;
	e[1][1] *= crFloat;
	e[1][2] *= crFloat;
	e[2][0] *= crFloat;
	e[2][1] *= crFloat;
	e[2][2] *= crFloat;
	
	return *this;
}

// +---------------------------------------------------
// | Set The Matrix to a Rotation Around X
// |			| 1     0            0	   |
// |			| 0 cos(Angle) -sin(Angle) |
// |			| 0 sin(Angle)  cos(Angle) |
// +---------------------------------------------------
inline void Matrix3x3::SetRotationX(const Float& Angle)
{
	Float COS = cosF(Angle);
	Float SIN = sinF(Angle);
	e[0][0] = KFLOAT_ONE; e[1][0] = KFLOAT_ZERO; e[2][0] = KFLOAT_ZERO;
	e[0][1] = KFLOAT_ZERO; e[1][1] = COS; e[2][1] =-SIN;
	e[0][2] = KFLOAT_ZERO; e[1][2] = SIN; e[2][2] = COS;
}


// +---------------------------------------------------
// | Set The Matrix to a rotation around Y
// |			| cos(Angle)  0 sin(Angle) |
// |			|     0       1     0      |
// |			| -sin(Angle) 0 cos(Angle) |
// +---------------------------------------------------
inline void Matrix3x3::SetRotationY(const Float& Angle)
{
	Float COS = cosF(Angle);
	Float SIN = sinF(Angle);
	e[0][0] = COS; e[1][0] = KFLOAT_ZERO; e[2][0] = SIN;
	e[0][1] = KFLOAT_ZERO; e[1][1] = KFLOAT_ONE; e[2][1] = KFLOAT_ZERO;
	e[0][2] =-SIN; e[1][2] = KFLOAT_ZERO; e[2][2] = COS;
}


// +---------------------------------------------------
// | Set The Matrix to a rotation around Z
// |			| cos(Angle)  -sin(Angle)  0|
// |			| sin(Angle)  cos(Angle)   0|
// |			| 0           0            1|
// +---------------------------------------------------
inline void Matrix3x3::SetRotationZ(const Float& Angle)
{
	Float COS = cosF(Angle);
	Float SIN = sinF(Angle);
	e[0][0] = COS; e[1][0] =-SIN; e[2][0] = KFLOAT_ZERO;
	e[0][1] = SIN; e[1][1] = COS; e[2][1] = KFLOAT_ZERO;
	e[0][2] = KFLOAT_ZERO; e[1][2] = KFLOAT_ZERO; e[2][2] = KFLOAT_ONE;
}


// +---------
// | PreRotation around X    =    (*this) * Matrix_Rotation_Around_X
// +---------
inline void Matrix3x3::PreRotateX(const Float& Angle)
{
	Float COS = cosF(Angle);
	Float SIN = sinF(Angle);
	Float tmp10 = e[1][0]*COS+e[2][0]*SIN;
	Float tmp11 = e[1][1]*COS+e[2][1]*SIN;
	Float tmp12 = e[1][2]*COS+e[2][2]*SIN;
	e[2][0] = -e[1][0]*SIN+e[2][0]*COS;
	e[2][1] = -e[1][1]*SIN+e[2][1]*COS;
	e[2][2] = -e[1][2]*SIN+e[2][2]*COS;
	e[1][0] = tmp10;
	e[1][1] = tmp11;
	e[1][2] = tmp12;
}


// +---------
// | PreRotation around Y    =    (*this) * Matrix_Rotation_Around_Y
// +---------
inline void Matrix3x3::PreRotateY(const Float& Angle)
{
	Float COS = cosF(Angle);
	Float SIN = sinF(Angle);
	Float tmp00 = e[0][0]*COS-e[2][0]*SIN;
	Float tmp01 = e[0][1]*COS-e[2][1]*SIN;
	Float tmp02 = e[0][2]*COS-e[2][2]*SIN;
	e[2][0] = e[0][0]*SIN+e[2][0]*COS;
	e[2][1] = e[0][1]*SIN+e[2][1]*COS;
	e[2][2] = e[0][2]*SIN+e[2][2]*COS;
	e[0][0] = tmp00;
	e[0][1] = tmp01;
	e[0][2] = tmp02;
}

// +---------
// | PreRotation around Z    =    (*this) * Matrix_Rotation_Around_Z
// +---------
inline void Matrix3x3::PreRotateZ(const Float& Angle)
{
	Float COS = cosF(Angle);
	Float SIN = sinF(Angle);
	Float tmp00 = e[0][0]*COS+e[1][0]*SIN;
	Float tmp01 = e[0][1]*COS+e[1][1]*SIN;
	Float tmp02 = e[0][2]*COS+e[1][2]*SIN;
	e[1][0] = -e[0][0]*SIN+e[1][0]*COS;
	e[1][1] = -e[0][1]*SIN+e[1][1]*COS;
	e[1][2] = -e[0][2]*SIN+e[1][2]*COS;
	e[0][0] = tmp00;
	e[0][1] = tmp01;
	e[0][2] = tmp02;
}


// +---------
// | PostRotation around X    =   Matrix_Rotation_Around_X * (*this)
// +---------
inline void Matrix3x3::PostRotateX(const Float& Angle)
{
	Float COS = cosF(Angle);
	Float SIN = sinF(Angle);
	Float tmp01 = COS*e[0][1]-SIN*e[0][2];
	e[0][2] = SIN*e[0][1]+COS*e[0][2];
	Float tmp11 = COS*e[1][1]-SIN*e[1][2];
	e[1][2] = SIN*e[1][1]+COS*e[1][2];
	Float tmp21 = COS*e[2][1]-SIN*e[2][2];
	e[2][2] = SIN*e[2][1]+COS*e[2][2];
	e[0][1] = tmp01;
	e[1][1] = tmp11;
	e[2][1] = tmp21;
}


// +---------
// | PostRotation around Y    =   Matrix_Rotation_Around_Y * (*this)
// +---------
inline void Matrix3x3::PostRotateY(const Float& Angle)
{
	Float COS = cosF(Angle);
	Float SIN = sinF(Angle);
	Float tmp00 = COS*e[0][0]+SIN*e[0][2];
	e[0][2] = -SIN*e[0][0]+COS*e[0][2];
	Float tmp10 = COS*e[1][0]+SIN*e[1][2];
	e[1][2] = -SIN*e[1][0]+COS*e[1][2];
	Float tmp20 = COS*e[2][0]+SIN*e[2][2];
	e[2][2] = -SIN*e[2][0]+COS*e[2][2];
	e[0][0] = tmp00;
	e[1][0] = tmp10;
	e[2][0] = tmp20;
}

// +---------
// | PostRotation around Z    =  Matrix_Rotation_Around_Z * (*this)
// +---------
inline void Matrix3x3::PostRotateZ(const Float& Angle)
{
	Float COS = cosF(Angle);
	Float SIN = sinF(Angle);
	Float tmp00 = COS*e[0][0]-SIN*e[0][1];
	e[0][1] = SIN*e[0][0]+COS*e[0][1];
	Float tmp10 = COS*e[1][0]-SIN*e[1][1];
	e[1][1] = SIN*e[1][0]+COS*e[1][1];
	Float tmp20 = COS*e[2][0]-SIN*e[2][1];
	e[2][1] = SIN*e[2][0]+COS*e[2][1];
	e[0][0] = tmp00;
	e[1][0] = tmp10;
	e[2][0] = tmp20;
}


// +---------
// | Matrix = Rotation_Around_Z * Rotation_Around_Y * Rotation_Around_X
// +---------
inline void Matrix3x3::SetRotationXYZ(const Float& AngleX,const Float& AngleY,const Float& AngleZ)
{
	Float CX = cosF(AngleX);
	Float SX = sinF(AngleX);
	Float CY = cosF(AngleY);
	Float SY = sinF(AngleY);
	Float CZ = cosF(AngleZ);
	Float SZ = sinF(AngleZ);
	Float t2 = CZ*SY;
	Float t10 = SZ*SY;
	e[0][0] = CZ*CY;
	e[0][1] = t2*SX-SZ*CX;
	e[0][2] = t2*CX+SZ*SX;
	e[1][0] = SZ*CY;
	e[1][1] = t10*SX+CZ*CX;
	e[1][2] = t10*CX-CZ*SX;
	e[2][0] = -SY;
	e[2][1] = CY*SX;
	e[2][2] = CY*CX;
}


// +---------
// | Matrix = Rotation_Around_X * Rotation_Around_Y * Rotation_Around_Z
// +---------
inline void Matrix3x3::SetRotationZYX(const Float& AngleX,const Float& AngleY,const Float& AngleZ)
{
	Float CX = cosF(AngleX);
	Float SX = sinF(AngleX);
	Float CY = cosF(AngleY);
	Float SY = sinF(AngleY);
	Float CZ = cosF(AngleZ);
	Float SZ = sinF(AngleZ);
	Float t4 = CZ*SY;
	Float t8 = SZ*SY;
	e[0][0] = CZ*CY;
	e[0][1] = -SZ*CY;
	e[0][2] = SY;
	e[1][0] = SZ*CX+t4*SX;
	e[1][1] = CZ*CX-t8*SX;
	e[1][2] = -CY*SX;
	e[2][0] = SZ*SX-t4*CX;
	e[2][1] = CZ*SX+t8*CX;
	e[2][2] = CY*CX;
}


// +---------
// | PreRotation around Z, then around Y then around X    =    (*this) * Matrix_Rotation_Around_X * Matrix_Rotation_Around_Y * Matrix_Rotation_Z
// +---------
inline void Matrix3x3::PreRotateZYX(const Float& AngleX,const Float& AngleY,const Float& AngleZ)
{
	Float CX = cosF(AngleX);
	Float SX = sinF(AngleX);
	Float CY = cosF(AngleY);
	Float SY = sinF(AngleY);
	Float CZ = cosF(AngleZ);
	Float SZ = sinF(AngleZ);
	Float t4 = CZ*SY;
	Float t6 = SZ*CX+t4*SX;
	Float t10 = SZ*SX-t4*CX;
	Float t26 = SZ*SY;
	Float t28 = CZ*CX-t26*SX;
	Float t32 = CZ*SX+t26*CX;
	Float tmp00 = e[0][0]*CZ*CY+e[1][0]*t6+e[2][0]*t10;
	Float tmp01 = e[0][1]*CZ*CY+e[1][1]*t6+e[2][1]*t10;
	Float tmp02 = e[0][2]*CZ*CY+e[1][2]*t6+e[2][2]*t10;
	Float tmp10 = -e[0][0]*SZ*CY+e[1][0]*t28+e[2][0]*t32;
	Float tmp11 = -e[0][1]*SZ*CY+e[1][1]*t28+e[2][1]*t32;
	Float tmp12 = -e[0][2]*SZ*CY+e[1][2]*t28+e[2][2]*t32;
	e[2][0] = SY*e[0][0]-e[1][0]*CY*SX+e[2][0]*CY*CX;
	e[2][1] = SY*e[0][1]-e[1][1]*CY*SX+e[2][1]*CY*CX;
	e[2][2] = SY*e[0][2]-e[1][2]*CY*SX+e[2][2]*CY*CX;
	e[0][0] = tmp00;
	e[0][1] = tmp01;
	e[0][2] = tmp02;
	e[1][0] = tmp10;
	e[1][1] = tmp11;
	e[1][2] = tmp12;
}


// +---------
// | PreRotation around X, then around Y then around Z    =    (*this) * Matrix_Rotation_Around_Z * Matrix_Rotation_Around_Y * Matrix_Rotation_X
// +---------
inline void Matrix3x3::PreRotateXYZ(const Float& AngleX,const Float& AngleY,const Float& AngleZ)
{
	Float CX = cosF(AngleX);
	Float SX = sinF(AngleX);
	Float CY = cosF(AngleY);
	Float SY = sinF(AngleY);
	Float CZ = cosF(AngleZ);
	Float SZ = sinF(AngleZ);
	Float t19 = CZ*SY;
	Float t22 = t19*SX-SZ*CX;
	Float t24 = SZ*SY;
	Float t27 = t24*SX+CZ*CX;
	Float t29 = e[2][0]*CY;
	Float t34 = e[2][1]*CY;
	Float t39 = e[2][2]*CY;
	Float t44 = t19*CX+SZ*SX;
	Float t48 = t24*CX-CZ*SX;
	Float tmp00 = e[0][0]*CZ*CY+e[1][0]*SZ*CY-e[2][0]*SY;
	Float tmp01 = e[0][1]*CZ*CY+e[1][1]*SZ*CY-e[2][1]*SY;
	Float tmp02 = e[0][2]*CZ*CY+e[1][2]*SZ*CY-e[2][2]*SY;
	Float tmp10 = e[0][0]*t22+e[1][0]*t27+t29*SX;
	Float tmp11 = e[0][1]*t22+e[1][1]*t27+t34*SX;
	Float tmp12 = e[0][2]*t22+e[1][2]*t27+t39*SX;
	e[2][0] = e[0][0]*t44+e[1][0]*t48+t29*CX;
	e[2][1] = e[0][1]*t44+e[1][1]*t48+t34*CX;
	e[2][2] = e[0][2]*t44+e[1][2]*t48+t39*CX;
	e[0][0] = tmp00;
	e[0][1] = tmp01;
	e[0][2] = tmp02;
	e[1][0] = tmp10;
	e[1][1] = tmp11;
	e[1][2] = tmp12;
}



// +---------
// | PostRotation around Z, then around Y then around X    =    Matrix_Rotation_Around_X * Matrix_Rotation_Around_Y * Matrix_Rotation_Z * (*this)
// +---------
inline void Matrix3x3::PostRotateZYX(const Float& AngleX,const Float& AngleY,const Float& AngleZ)
{
	Float CX = cosF(AngleX);
	Float SX = sinF(AngleX);
	Float CY = cosF(AngleY);
	Float SY = sinF(AngleY);
	Float CZ = cosF(AngleZ);
	Float SZ = sinF(AngleZ);
	Float t3 = CZ*e[0][0]-SZ*e[0][1];
	Float t9 = SZ*e[0][0]+CZ*e[0][1];
	Float t13 = -SY*t3+CY*e[0][2];
	Float t21 = CZ*e[1][0]-SZ*e[1][1];
	Float t27 = SZ*e[1][0]+CZ*e[1][1];
	Float t31 = -SY*t21+CY*e[1][2];
	Float t39 = CZ*e[2][0]-SZ*e[2][1];
	Float t45 = SZ*e[2][0]+CZ*e[2][1];
	Float t49 = -SY*t39+e[2][2]*CY;
	e[0][0] = CY*t3+SY*e[0][2];
	e[0][1] = CX*t9-SX*t13;
	e[0][2] = SX*t9+CX*t13;
	e[1][0] = CY*t21+SY*e[1][2];
	e[1][1] = CX*t27-SX*t31;
	e[1][2] = SX*t27+CX*t31;
	e[2][0] = CY*t39+e[2][2]*SY;
	e[2][1] = CX*t45-SX*t49;
	e[2][2] = SX*t45+CX*t49;
}


// +---------
// | PostRotation around X, then around Y then around Z    =  Matrix_Rotation_Around_Z * Matrix_Rotation_Around_Y * Matrix_Rotation_X * (*this)
// +---------
inline void Matrix3x3::PostRotateXYZ(const Float& AngleX,const Float& AngleY,const Float& AngleZ)
{
	Float CX = cosF(AngleX);
	Float SX = sinF(AngleX);
	Float CY = cosF(AngleY);
	Float SY = sinF(AngleY);
	Float CZ = cosF(AngleZ);
	Float SZ = sinF(AngleZ);
	Float t4 = SX*e[0][1]+CX*e[0][2];
	Float t6 = CY*e[0][0]+SY*t4;
	Float t10 = CX*e[0][1]-SX*e[0][2];
	Float t22 = SX*e[1][1]+CX*e[1][2];
	Float t24 = e[1][0]*CY+SY*t22;
	Float t28 = CX*e[1][1]-SX*e[1][2];
	Float t40 = SX*e[2][1]+CX*e[2][2];
	Float t42 = e[2][0]*CY+SY*t40;
	Float t46 = CX*e[2][1]-SX*e[2][2];
	
	e[0][2] = -SY*e[0][0]+CY*t4;
	e[0][0] = CZ*t6-SZ*t10;
	e[0][1] = SZ*t6+CZ*t10;
	
	e[1][2] = -e[1][0]*SY+CY*t22;
	e[1][0] = CZ*t24-SZ*t28;
	e[1][1] = SZ*t24+CZ*t28;
	
	e[2][2] = -e[2][0]*SY+CY*t40;
	e[2][0] = CZ*t42-SZ*t46;
	e[2][1] = SZ*t42+CZ*t46;
}

inline void Matrix3x3::SetScale(const Float& ScaleX, const Float &ScaleY, const Float &ScaleZ)
{
	e[0][0] = ScaleX;
	e[1][1] = ScaleY;
	e[2][2] = ScaleZ;
	e[0][1] = e[0][2] = e[1][0] = e[1][2] = e[2][0] = e[2][1] = KFLOAT_ZERO;
}


// +---------
// | PreScale   =  (*this) * Scale_Matrix
// +---------
inline void Matrix3x3::PreScale(const Float& ScaleX, const Float &ScaleY, const Float &ScaleZ)
{
	e[0][0] = e[0][0]*ScaleX;
	e[0][1] = e[0][1]*ScaleX;
	e[0][2] = e[0][2]*ScaleX;
	e[1][0] = e[1][0]*ScaleY;
	e[1][1] = e[1][1]*ScaleY;
	e[1][2] = e[1][2]*ScaleY;
	e[2][0] = e[2][0]*ScaleZ;
	e[2][1] = e[2][1]*ScaleZ;
	e[2][2] = e[2][2]*ScaleZ;
}


// +---------
// | PostScale   =  Scale_Matrix * (*this)
// +---------
inline void Matrix3x3::PostScale(const Float& ScaleX, const Float &ScaleY, const Float &ScaleZ)
{
	e[0][0] = e[0][0]*ScaleX;
	e[0][1] = e[0][1]*ScaleY;
	e[0][2] = ScaleZ*e[0][2];
	e[1][0] = ScaleX*e[1][0];
	e[1][1] = e[1][1]*ScaleY;
	e[1][2] = ScaleZ*e[1][2];
	e[2][0] = ScaleX*e[2][0];
	e[2][1] = e[2][1]*ScaleY;
	e[2][2] = e[2][2]*ScaleZ;
}



// +---------
// | Cast from Quaternion
// +---------

/*
inline Matrix3x3::operator const Quaternion (void)
{
 return Quaternion(*this);
}
*/
inline Quaternion Matrix3x3::ToQuaternion() const
{
	Quaternion result;
	result.Set(*this);
	return result;
}


/*
inline Matrix3x3::operator const Matrix3x4 (void)
{
 Matrix3x4 m;
 memcpy(m.e,e,9*sizeof(Float));
 m.e[3][0] = KFLOAT_ZERO;
 m.e[3][1] = KFLOAT_ZERO;
 m.e[3][2] = KFLOAT_ZERO;
 return m;
}

inline Matrix3x3::operator const Matrix4x4 (void)
{
 Matrix4x4 result;
 
 result.e[0][0] = e[0][0];
 result.e[0][1] = e[0][1];
 result.e[0][2] = e[0][2];
 result.e[0][3] = KFLOAT_ZERO;
 result.e[1][0] = e[1][0];
 result.e[1][1] = e[1][1];
 result.e[1][2] = e[1][2];
 result.e[1][3] = KFLOAT_ZERO;
 result.e[2][0] = e[2][0];
 result.e[2][1] = e[2][1];
 result.e[2][2] = e[2][2];
 result.e[2][3] = KFLOAT_ZERO;
 result.e[3][0] = KFLOAT_ZERO;
 result.e[3][1] = KFLOAT_ZERO;
 result.e[3][2] = KFLOAT_ZERO;
 result.e[3][3] = KFLOAT_ONE;
 return result;
}
*/
inline void Matrix3x3::PostMultiply(const Matrix3x3& m)
{
	Float a, b;
	a = e[0][0]; b = e[0][1];
	e[0][0] = m.e[0][0] * a + m.e[1][0] * b + m.e[2][0] * e[0][2];
	e[0][1] = m.e[0][1] * a + m.e[1][1] * b + m.e[2][1] * e[0][2];
	e[0][2] = m.e[0][2] * a + m.e[1][2] * b + m.e[2][2] * e[0][2];
	a = e[1][0]; b = e[1][1];
	e[1][0] = m.e[0][0] * a + m.e[1][0] * b + m.e[2][0] * e[1][2];
	e[1][1] = m.e[0][1] * a + m.e[1][1] * b + m.e[2][1] * e[1][2];
	e[1][2] = m.e[0][2] * a + m.e[1][2] * b + m.e[2][2] * e[1][2];
	a = e[2][0]; b = e[2][1];
	e[2][0] = m.e[0][0] * a + m.e[1][0] * b + m.e[2][0] * e[2][2];
	e[2][1] = m.e[0][1] * a + m.e[1][1] * b + m.e[2][1] * e[2][2];
	e[2][2] = m.e[0][2] * a + m.e[1][2] * b + m.e[2][2] * e[2][2];
}



inline void Matrix3x3::SkewSymmetric(const Vector3D& v)
{
	e[0][0] = 0; 		e[0][1] = -v.z; 	e[0][2] = v.y;
	e[1][0] = v.z; 		e[1][1] = 0; 		e[1][2] = -v.x;
	e[2][0] = -v.y; 	e[2][1] = v.x; 		e[2][2] = 0;
}


inline void Matrix3x3::AlignRotationMatrix(Vector3D a, Vector3D b)
{
	a.Normalize();
	b.Normalize();
	Vector3D v;
	v.CrossProduct(a, b);
	float s = Norm(v);
	float c = Dot(a, b);
	
	Matrix3x3 skew;
	skew.SkewSymmetric(v);
	
	SetIdentity();
	
	*this += skew + skew*skew*((1.0f - c) / (s*s));
}