// **********************************************************************
// * FILE  : Matrix3x4.h
// * GROUP : TecLibs/3D
// *---------------------------------------------------------------------
// * PURPOSE : Matrix 3x4
// * COMMENT : 
// *---------------------------------------------------------------------
// **********************************************************************

#ifndef _Matrix3x4_h_
#define _Matrix3x4_h_

// **************************************************
// * Matrix3x4 class
// *-------------------------------------------------
// * - Matrix 3x4
// *
// *	e[0][0]	e[1][0]	e[2][0]	e[3][0]
// *	e[0][1]	e[1][1]	e[2][1]	e[3][1]
// *	e[0][2]	e[1][2]	e[2][2]	e[3][2]
// *
// **************************************************

struct Matrix3x4
{
	union
	{
		union
		{
			Vector3D Axis[4];
			struct
			{
				Vector3D XAxis;
				Vector3D YAxis;
				Vector3D ZAxis;
				Vector3D Pos;
			};
		};
		Float e[4][3];
	};
	
	
	
	// +---------
	// | Life Cycle
	// +---------
	inline Matrix3x4(){;}
	inline Matrix3x4(const Float* m);
	inline Matrix3x4(const Float& e00, const Float& e01, const Float& e02,
					 const Float& e10, const Float& e11, const Float& e12,
					 const Float& e20, const Float& e21, const Float& e22,
					 const Float& e30, const Float& e31, const Float& e32);
	inline Matrix3x4(const Matrix3x4& crMatrix);
	
	struct Identity {};
	inline Matrix3x4(Identity) { SetIdentity(); }
	inline static Matrix3x4 IdentityMatrix() { return Matrix3x4{ Identity{} }; }
	
	inline explicit Matrix3x4(const Matrix3x3& crMatrix);
	
	inline Matrix3x4(const Quaternion& cr_Quaternion, const Vector3D& cr_Vector);
	inline Matrix3x4(const Quaternion& cr_Quaternion, bool LeftHanded = true);
	inline Matrix3x4(const Vector3D& vx, const Vector3D& vy, const Vector3D& vz, const Point3D& pos)
	{
		Set(vx,vy,vz,pos);
	}
	
	inline void Set(const Vector3D& vx, const Vector3D& vy, const Vector3D& vz, const Point3D& pos);
	
	
	void Set(const Quaternion& q, const Point3D& p);
	void Set(const Quaternion& q);
	void SetFromRightHanded(const Quaternion& q);

	inline static Matrix3x4 LookAt(Point3D eye, Point3D target, Vector3D up);
	// +---------
	// | Pure Matrix Operations [ Matrix operator???(Matrix) ]
	// +---------
	inline void  SetNull();
	inline void  SetIdentity();
	inline friend Matrix3x4 Inv(const Matrix3x4 & cr_Matrix);
	inline friend Float Det(const Matrix3x4 & cr_Matrix);
	inline friend Float Norm(const Matrix3x4 & cr_Matrix);
	inline friend Float NormSquare(const Matrix3x4 & cr_Matrix);
	inline friend Float Trace(const Matrix3x4 & cr_Matrix);
	
	
	inline Matrix3x4 operator+ (const Matrix3x4& crMatrix) const;
	inline Matrix3x4 operator- (const Matrix3x4& crMatrix) const;
	inline Matrix3x4 operator* (const Matrix3x4& crMatrix) const;
	
	inline friend Matrix3x4 operator* (const Matrix3x3& left, const Matrix3x4& right);
	
	
	inline void operator=(const Matrix3x4& crMatrix);
	
	inline Float&	operator[](const unsigned int index){return ((Float*)e)[index];}
	// +---------
	// | Inner operators
	// +---------
	inline Matrix3x4& operator+=(const Matrix3x4& crMatrix);
	inline Matrix3x4& operator-=(const Matrix3x4& crMatrix);
	inline Matrix3x4& operator*=(const Matrix3x4& crMatrix);
	
	
	// +---------
	// | Matrix operators with Vectors
	// +---------
	
	inline Vector3D operator*(const Vector3D& cr_Vector) const; 
	inline Point3D operator*(const Point3D& cr_Point) const ; 
	
	inline void TransformPoints(Point3D* tab, int count) const;
	inline void TransformVectors(Vector3D* tab, int count) const;
	inline void TransformVector(Vector3D* tab) const;
	inline void TransformPoint(Point3D* tab) const;
	inline void TransformVector(const Vector3D* tab,Vector3D* result) const;
	inline void TransformPoint(const Point3D* tab,Point3D* result) const;
	
	// +---------
	// | Matrix operators with Floats
	// +---------
	inline Matrix3x4& operator*=(const Float& crFloat);
	
	inline friend Matrix3x4 operator*(const Float& f, Matrix3x4 m); 
	inline friend Matrix3x4 operator*(Matrix3x4 m,const Float& f); 
	
	inline void SetRotation(const Matrix3x3& rot);
	inline Matrix3x3 GetRotation() const;
	
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
	
	inline void SetScale(const Float& ScaleX, const Float& ScaleY, const Float& ScaleZ);
	inline void PreScale(const Float& ScaleX, const Float& ScaleY, const Float& ScaleZ);
	inline void PostScale(const Float& ScaleX, const Float& ScaleY, const Float& ScaleZ);
	
	inline void SetTranslation(const Vector3D& Translation);
	inline void PreTranslate(const Vector3D& Translation);
	inline void PostTranslate(const Vector3D& Translation);
	
	inline void PreMultiply(const Matrix3x4& m);
	inline void PostMultiply(const Matrix3x4& m);
	
	inline Point3D GetTranslation() const;

	inline void	GetPRS(Point3D& pos,Point3D& rot,float& scale) const;
	
	/*
	inline Point3D *X();
	inline Point3D *Y();
	inline Point3D *Z();
	inline Point3D *P();
	*/
	
	inline bool IsIdentity() const;
};

#endif



