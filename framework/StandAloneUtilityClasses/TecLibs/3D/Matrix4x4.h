// **********************************************************************
// * FILE  : Matrix4x4.h
// * GROUP : TecLibs/3D
// *---------------------------------------------------------------------
// * PURPOSE : Matrix 4x4
// * COMMENT : 
// *---------------------------------------------------------------------
// **********************************************************************

#ifndef _Matrix4x4_h_
#define _Matrix4x4_h_

// **************************************************
// * Matrix4x4 class
// *-------------------------------------------------
// * - Square Matrix 4x4
// *
// *	e[0][0]	e[1][0]	e[2][0]	e[3][0]
// *	e[0][1]	e[1][1]	e[2][1]	e[3][1]
// *	e[0][2]	e[1][2]	e[2][2]	e[3][2]
// *	e[0][3]	e[1][3]	e[2][3]	e[3][3]
// *
// **************************************************

struct Matrix4x4
{
	
	union
	{
		union
		{
			Vector4D Axis[4];
			struct
			{
				Vector4D XAxis;
				Vector4D YAxis;
				Vector4D ZAxis;
				Vector4D Pos;
			};
		};
		Float e[4][4];
	};
	
	// +---------
	// | Life Cycle
	// +---------
	inline Matrix4x4(){;}
	inline explicit Matrix4x4(const Float* data) { Set(data); }
	inline Matrix4x4(const Matrix4x4& mat) { Set(mat); }
	inline explicit Matrix4x4(const Matrix3x4& mat) { Set(mat); }
	inline explicit Matrix4x4(const Matrix3x3& mat) { Set(mat); }	
	inline Matrix4x4(const Quaternion& q, const Vector3D& v) { Set(q, v); }
	inline Matrix4x4(const Vector4D& v0, const Vector4D& v1, const Vector4D& v2, const Vector4D& v3)
	{ Set(v0, v1, v2, v3); }
	inline Matrix4x4(const Float& a11, const Float& a12, const Float& a13, const Float& a14,
					 const Float& a21, const Float& a22, const Float& a23, const Float& a24,
					 const Float& a31, const Float& a32, const Float& a33, const Float& a34,
					 const Float& a41, const Float& a42, const Float& a43, const Float& a44)
	{ Set(a11, a12, a13, a14, a21, a22, a23, a24, a31, a32, a33, a34, a41, a42, a43, a44); }
	
	inline void		Set(const Float* data);
	inline void		Set(const Matrix4x4& mat);
	inline void		Set(const Matrix3x4& mat);
	inline void		Set(const Matrix3x3& mat);
	inline void		Set(const Quaternion& cr_Quaternion, const Vector3D& cr_Vector);
	inline void		Set(const Vector4D& v0, const Vector4D& v1, const Vector4D& v2, const Vector4D& v3);
	inline void		Set(const Float& a11, const Float& a12, const Float& a13, const Float& a14,
					 const Float& a21, const Float& a22, const Float& a23, const Float& a24,
					 const Float& a31, const Float& a32, const Float& a33, const Float& a34,
					 const Float& a41, const Float& a42, const Float& a43, const Float& a44);
	inline void		SetNull();
	inline void		SetIdentity();
	inline void		SetInvert();
	
	inline void		SetNegated(const Matrix4x4& mat); //@Unimplemented
	
	inline Matrix4x4& operator=(const Matrix4x4& m) { Set(m); return *this; }
	inline Matrix4x4& operator=(const Matrix3x4& m) { Set(m); return *this; }
	inline Matrix4x4& operator=(const Matrix3x3& m) { Set(m); return *this; }
	
	inline Float&	operator[](unsigned int index){return ((Float*)e)[index];}
	inline const Float&	operator[](unsigned int index) const {return ((Float*)e)[index];}
	
	// +---------
	// | Pure Matrix Operations [ Matrix operator???(Matrix) ]
	// +---------
	inline friend Matrix4x4 Inv(Matrix4x4 m);
	//inline friend Float Det(); //@Unimplemented
	//inline friend Float Norm(); //@Unimplemented
	//inline friend Float NormSquare(); //@Unimplemented
	inline friend Float Trace(const Matrix4x4& m);
	
	inline friend Matrix4x4 operator+ (Matrix4x4 left, const Matrix4x4& right);
	inline friend Matrix4x4 operator- (Matrix4x4 left, const Matrix4x4& right);
	inline friend Matrix4x4 operator* (Matrix4x4 left, const Matrix4x4& right);
	
	// +---------
	// | Unary Operator
	// +---------
	// inline void operator-(); //@Unimplemented
	
	// +---------
	// | Inner operators
	// +---------
	//inline Matrix4x4& operator+=(const Matrix4x4& crMatrix); //@Unimplemented
	//inline Matrix4x4& operator-=(const Matrix4x4& crMatrix); //@Unimplemented
	
	inline Matrix4x4& operator*=(const Matrix4x4& crMatrix);
	inline Matrix4x4& operator*=(const Float& crFloat);
	inline Matrix4x4& operator+=(const Matrix4x4& crMatrix);
	inline Matrix4x4& operator-=(const Matrix4x4& crMatrix);

	
	// +---------
	// | Matrix operators with Vectors
	// +---------
	inline friend Vector4D	operator*(const Matrix4x4& crMatrix, const Vector3D& crVector); 
	inline friend Vector4D	operator*(const Matrix4x4& crMatrix, const Point3D& crVector); 
	inline friend Vector4D	operator*(const Matrix4x4& crMatrix, const Vector4D& crVector); 
	inline friend Vector4D	operator*(const Vector4D& crVector, const Matrix4x4& crMatrix);
	inline friend Vector4D	operator*(const Vector3D& crVector, const Matrix4x4& crMatrix);
	inline friend Vector4D	operator*(const Point3D& crVector,  const Matrix4x4& crMatrix);
	
	
	// +---------
	// | Matrix operators with Floats
	// +---------
	inline friend Matrix4x4 operator*(Matrix4x4 m, const Float& f); 
	inline friend Matrix4x4 operator*(const Float& f, Matrix4x4 m); 
	
	// +---------
	// | Operators specific to transformation matrices
	// +---------
	/*
	//@Unimplemented //@Unimplemented //@Unimplemented
	inline void SetRotationX(const Float& Angle);
	inline void SetRotationY(const Float& Angle);
	inline void SetRotationZ(const Float& Angle);
	inline void PreRotateX(const Float& Angle);
	inline void PreRotateY(const Float& Angle);
	inline void PreRotateZ(const Float& Angle);
	inline void PostRotateX(const Float& Angle);
	inline void PostRotateY(const Float& Angle);
	inline void PostRotateZ(const Float& Angle);
	inline void SetRotationXYZ(const Float& AngleX,const Float& AngleY,const Float& AngleZ);
	inline void SetRotationZYX(const Float& AngleX,const Float& AngleY,const Float& AngleZ);
	inline void PreRotateXYZ(const Float& AngleX,const Float& AngleY,const Float& AngleZ);
	inline void PreRotateZYX(const Float& AngleX,const Float& AngleY,const Float& AngleZ);
	inline void PostRotateXYZ(const Float& AngleX,const Float& AngleY,const Float& AngleZ);
	inline void PostRotateZYX(const Float& AngleX,const Float& AngleY,const Float& AngleZ);
	inline void SetScale(const Float& ScaleX, const Float &ScaleY, const Float ScaleZ);
	inline void PreScale(const Float& ScaleX, const Float &ScaleY, const Float ScaleZ);
	inline void PostScale(const Float& ScaleX, const Float &ScaleY, const Float ScaleZ);
	inline void SetTranslation(const Vector3D& Translation);
	inline void AddTranslation(const Vector3D& Translation);
	*/
	

	inline void Mult(const Matrix4x4& m1,const Matrix4x4& m2);
};

inline Matrix4x4 Transpose(const Matrix4x4 & cr_Matrix)
{
	Matrix4x4 mat;
	mat.e[0][0] = cr_Matrix.e[0][0];
	mat.e[0][1] = cr_Matrix.e[1][0];
	mat.e[0][2] = cr_Matrix.e[2][0];
	mat.e[0][3] = cr_Matrix.e[3][0];
	mat.e[1][0] = cr_Matrix.e[0][1];
	mat.e[1][1] = cr_Matrix.e[1][1];
	mat.e[1][2] = cr_Matrix.e[2][1];
	mat.e[1][3] = cr_Matrix.e[3][1];
	mat.e[2][0] = cr_Matrix.e[0][2];
	mat.e[2][1] = cr_Matrix.e[1][2];
	mat.e[2][2] = cr_Matrix.e[2][2];
	mat.e[2][3] = cr_Matrix.e[3][2];
	mat.e[3][0] = cr_Matrix.e[0][3];
	mat.e[3][1] = cr_Matrix.e[1][3];
	mat.e[3][2] = cr_Matrix.e[2][3];
	mat.e[3][3] = cr_Matrix.e[3][3];
	return mat;
}


#endif



