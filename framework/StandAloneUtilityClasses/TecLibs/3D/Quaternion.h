// **********************************************************************
// * FILE  : Quaternion.h
// * GROUP : TecLibs/3D
// *---------------------------------------------------------------------
// * PURPOSE : Quaternion Class 
//       #     #    #    ######  #     #   ###   #     #  #####               
//       #  #  #   # #   #     # ##    #    #    #     # #     #              
//       #  #  #  #   #  #     # # #   #    #    # #   # #                    
//       #  #  # #     # ######  #  #  #    #    #  #  # #  ####              
//       #  #  # ####### #   #   #   # #    #    #   # # #     #              
//       #  #  # #     # #    #  #    ##    #    #    #  #     #              
//        ## ##  #     # #     # #     #   ###   #     #  #####               
// THIS CLASS SUPPOSES THAT THE QUATERNION ARE USED TO REPRESENT ROTATION
// SO THEY ARE CONSIDERED TO BE NORMALIZED USING THIS WITH NOT NORMALIZED QUATERNION
// IS AT YOUR OWN RISK, WE CANNOT BE HELD RESPONSIBLE FOR THE DAMAGE CAUSED
// BY THE USE OF THIS CLASS. 
// **********************************************************************

#pragma once

namespace Kigs
{
	namespace Maths
	{

		struct Quaternion
		{

			union
			{
				struct
				{
					Vector3D V;
					Float w;
				};

				Vector4D v4;
			};


			// +---------
			// | Life-Cycle
			// +---------
			inline Quaternion();
			inline Quaternion(const Quaternion& q);
			inline Quaternion(const Vector3D& V, const Float& w) : V(V), w(w) { ; };
			inline Quaternion(const Float& x, const Float& y, const Float& z, const Float& w);
			inline explicit Quaternion(const Matrix3x3& crMatrix);
			inline explicit Quaternion(const Matrix3x4& crMatrix);

			inline void SetIdentity();
			inline void Set(const Quaternion& q) { *this = q; }
			inline void Set(const Vector3D& tv, const Float& tw) { V = tv; w = tw; };
			inline void Set(const Float& x, const Float& y, const Float& z, const Float& w);
			inline void Set(const Matrix3x3& crMatrix);
			inline void Set(const Matrix3x4& crMatrix);
			inline void SetAngAxis(const Vector3D& axis, const Float& angle);
			inline void GetAngAxis(Vector3D& axis, Float& angle);
			inline void GetEulerAngles(Vector3D& euler) const;

			// +---------
			// | operators
			// +---------
			inline void operator=(const Quaternion& q);

			// Cast To Matrix3x3
			//inline operator const Matrix3x3 (void);
			//inline operator const Matrix3x4 (void);

			inline Matrix3x3 ToMatrix() const;

			// +---------
			// | Utilities
			// +---------
			inline void Normalize();
			inline friend Float Norm(const Quaternion& cr_Quaternion);
			inline friend Float NormSquare(const Quaternion& cr_Quaternion);
			inline friend Float Angle(const Quaternion& cr_Quaternion);

			// +---------
			// | Setting The quaternion to Rotation around axis X,Y,Z or Vector U
			// +---------
			inline void SetRotationX(const Float& cr_Angle);
			inline void SetRotationY(const Float& cr_Angle);
			inline void SetRotationZ(const Float& cr_Angle);
			inline void SetRotationU(const Vector3D& cr_Vector, const Float& cr_Angle);

			// +---------
			// | Unary Operator
			// +---------
			inline Quaternion operator-() const;

			// +---------
			// | Operators
			// +---------
			inline friend Quaternion operator*(Quaternion a, const Quaternion& b);
			inline friend Quaternion operator*(Float& f, const Quaternion q);
			inline friend Quaternion operator*(Quaternion q, const Float& f);


			inline friend Quaternion operator+ (Quaternion q1, const Quaternion& q2);
			inline friend Quaternion operator- (Quaternion q1, const Quaternion& q2);

			inline Quaternion& operator*= (const Quaternion& cr_Quaternion);
			inline Quaternion& operator+= (const Quaternion& cr_Quaternion);
			inline Quaternion& operator-= (const Quaternion& cr_Quaternion);
			inline bool operator== (const Quaternion& cr_Quaternion);
			inline bool operator!= (const Quaternion& cr_Quaternion);

			inline friend Quaternion operator/(Quaternion q, const Float& f);
			inline Quaternion& operator*= (const Float& cr_Float);
			inline Quaternion& operator/= (const Float& cr_Float);

			inline friend Point3D operator*(const Quaternion& q, const Point3D& v);


			inline const Float& operator[](size_t i) const;
			inline Float& operator[](size_t i);
			// +---------
			// | External operators
			// +---------
			inline friend Float      Dot(const Quaternion& cr_Quaternion1, const Quaternion& cr_Quaternion2);
			inline friend Vector3D   Log(const Quaternion& q);
			inline friend Quaternion Exp(const Vector3D& v);
			inline friend Quaternion Inv(const Quaternion& cr_Quaternion);

			// +---------
			// | Interpolation
			// +---------
			// Slerp result = (1-cr_weight)*cr_Quaternion1 + cr_weight*cr_Quaternion2
			inline friend Quaternion Slerp(const Quaternion& cr_Quaternion1, const Quaternion& cr_Quaternion2, const Float& cr_weight);
			inline friend Quaternion SlerpNearest(const Quaternion& cr_Quaternion1, const Quaternion& cr_Quaternion2, const Float& cr_weight);
			inline friend Quaternion Squad(Float t, const Quaternion& p, const Quaternion& q, const Quaternion& a, const Quaternion& b);
			inline friend Quaternion SmoothSplineInterpolation(Float t, const Quaternion& q0, const Quaternion& q1, const Quaternion& q2, const Quaternion& q3);
			inline friend Quaternion SmoothSplineInterpolationNearest(Float t, const Quaternion& tq0, const Quaternion& q1, const Quaternion& tq2, const Quaternion& tq3);
			inline friend Quaternion RotationArc(const Vector3D& v0, const Vector3D& v1); // returns the shortest arc quaternion so that v1 = q * v0; 

			// Blend is the same as Slerp, but Blend is available and as the same prototype for Vector3D, Point2D, Quaternion... so 
			// it can be called in template methods
			friend inline Quaternion Blend(const Quaternion& U, const Quaternion& V, Float t)
			{
				return SlerpNearest(U, V, t);
			}
		};

	}
}



