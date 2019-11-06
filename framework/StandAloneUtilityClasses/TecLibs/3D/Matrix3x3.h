// **********************************************************************
// * FILE  : Matrix3x3.h
// * GROUP : TecLibs/3D
// *---------------------------------------------------------------------
// * PURPOSE : Base Classe for all the Matrices
// * COMMENT : 
// *---------------------------------------------------------------------
// **********************************************************************

#ifndef _Matrix3x3_h_
#define _Matrix3x3_h_

// **************************************************
// * Matrix3x3 class
// *-------------------------------------------------
// * - Square Matrix 3x3
// *
// *	e[0][0]	e[1][0]	e[2][0]
// *	e[0][1]	e[1][1]	e[2][1]
// *	e[0][2]	e[1][2]	e[2][2]
// *
// **************************************************


struct Matrix3x3 
{
	union
	{
		union
		{
			Vector3D Axis[3];
			struct
			{
				Vector3D XAxis;
				Vector3D YAxis;
				Vector3D ZAxis;
			};
		};
		Float e[3][3];
	};
	
	
	// +---------
	// | Life Cycle
	// +---------
	inline Matrix3x3();
	inline Matrix3x3(const Vector3D& vx, const Vector3D& vy, const Vector3D& vz)
	{
		Set(vx, vy, vz);
	}
	
	inline void Set(const Vector3D& vx, const Vector3D& vy, const Vector3D& vz);
	inline Matrix3x3(const Matrix3x3& cr_Matrix);
	inline explicit Matrix3x3(const Quaternion& cr_Quaternion);
	inline void Clear()
	{
		XAxis.Set(0, 0, 0);
		YAxis.Set(0, 0, 0);
		ZAxis.Set(0, 0, 0);
	}
	
	inline void SetIdentity();
	inline void Set(const Matrix3x3& cr_Matrix);
	inline void Set(const Quaternion& cr_Quaternion);
	
	inline const Matrix3x3& operator=(const Matrix3x3& crMatrix);
	inline const Matrix3x3& operator=(const Quaternion& crQuaternion);
	
	inline Float&	operator[](const unsigned int index){return ((Float*)e)[index];}
	
	// +---------
	// | Pure Matrix Operations [ Matrix operator???(Matrix) ]
	// +---------
	inline friend Matrix3x3 Inv(const Matrix3x3 & cr_Matrix);
	inline friend Float Det(const Matrix3x3 & cr_Matrix);
	inline friend Float Norm(const Matrix3x3 & cr_Matrix);
	inline friend Float NormSquare(const Matrix3x3 & cr_Matrix);
	inline friend Float Trace(const Matrix3x3 & cr_Matrix);
	inline friend Matrix3x3 Transpose(const Matrix3x3 & cr_Matrix);
	
	
	inline Matrix3x3 operator+ (const Matrix3x3& crMatrix);
	inline Matrix3x3 operator- (const Matrix3x3& crMatrix);
	inline Matrix3x3 operator* (const Matrix3x3& crMatrix);
	
	// +---------
	// | Unary Operator
	// +---------
	inline void operator-();
	
	// +---------
	// | Inner operators
	// +---------
	inline const Matrix3x3& operator+=(const Matrix3x3& crMatrix);
	inline const Matrix3x3& operator-=(const Matrix3x3& crMatrix);
	inline const Matrix3x3& operator*=(const Matrix3x3& crMatrix);
	
	
	// +---------
	// | Matrix operators with Vectors
	// +---------
	inline Vector3D operator*(const Vector3D& crVector) const;
	inline Point3D  operator*(const Point3D& crPoint) const;
	friend inline Vector3D operator*(const Vector3D& crVector, const Matrix3x3& crMatrix);
	friend inline Point3D  operator*(const Point3D& crPoint, const Matrix3x3& crMatrix);
	inline void TransformPoints(Point3D* tab, int count) const;
	inline void TransformVectors(Vector3D* tab, int count) const;
	inline void TransformVector(Vector3D* tab) const;
	
	inline void TransformPoints(Point2D* tab, int count) const;
	
	// +---------
	// | Matrix operators with Floats
	// +---------
	inline const Matrix3x3 operator*(const Float& crFloat); 
	inline friend const Matrix3x3 operator*(const Matrix3x3& crMatrix, const Float& crFloat); 
	inline const Matrix3x3& operator*=(const Float& crFloat);
	
	// +---------
	// | Operators specific to transformation matrices
	// +---------
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
	
	inline void SetScale(const Float& ScaleX, const Float &ScaleY, const Float &ScaleZ);
	inline void PreScale(const Float& ScaleX, const Float &ScaleY, const Float &ScaleZ);
	inline void PostScale(const Float& ScaleX, const Float &ScaleY, const Float &ScaleZ);
	
	inline void PostMultiply(const Matrix3x3& m);
	
	inline void SkewSymmetric(const Vector3D& v);
	
	// Rotation Matrix that align a on b (ignores lengths) : b = mat*a
	inline void AlignRotationMatrix(Vector3D a, Vector3D b);
	
	// +---------
	// | Cast to Quaternion
	// +---------
	//inline explicit operator const Quaternion (void);
	inline Quaternion ToQuaternion() const;
	
	// +---------
	// | Cast To Matrix3x4
	// +---------
	// inline explicit operator const Matrix3x4 (void);
	
	// +---------
	// | Cast To Matrix4x4
	// +---------
	// inline explicit operator const Matrix4x4 (void);
};

#endif



