// **********************************************************************
// * FILE  : Vector4D.cpp
// * GROUP : TecLibs, section Math
// *---------------------------------------------------------------------
// * PURPOSE : Implementation of Vector4D
// **********************************************************************

namespace Kigs
{
    namespace Maths
    {

        // ----------------------------------------------------------------------
        // +---------
        // | Life-cycle
        // +---------
        // Constructors
        Vector4D::Vector4D()
        {
            ;
        }
        Vector4D::Vector4D(const Float& fValue)
        {
            x = y = z = w = fValue;
        }
        Vector4D::Vector4D(const Float& x, const Float& y, const Float& z, const Float& w)
            :x(x), y(y), z(z), w(w)
        {
            ;
        }
        Vector4D::Vector4D(const Vector4D& V)
            :x(V.x), y(V.y), z(V.z), w(V.w)
        {
            ;
        }
        Vector4D::Vector4D(const Vector3D& V)
            :x(V.x), y(V.y), z(V.z), w(0)
        {
            ;
        }
        Vector4D::Vector4D(const Point3D& P)
            :x(P.x), y(P.y), z(P.z), w(1)
        {
            ;
        }

        // Set (constructors-like)
        void Vector4D::Set(const Float& fValue)
        {
            x = y = z = w = fValue;
        }
        void Vector4D::Set(const Float& x, const Float& y, const Float& z, const Float& w)
        {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
        }
        void Vector4D::Set(const Vector4D& V)
        {
            x = V.x;
            y = V.y;
            z = V.z;
            w = V.w;
        }
        void Vector4D::Set(const Vector3D& V)
        {
            x = V.x;
            y = V.y;
            z = V.z;
            w = 0;
        }
        void Vector4D::Set(const Point3D& P)
        {
            x = P.x;
            y = P.y;
            z = P.z;
            w = 1;
        }

        // Assignement
        const Vector4D& Vector4D::operator =(const Vector4D& V)
        {
            x = V.x;
            y = V.y;
            z = V.z;
            w = V.w;

            return *this;
        }

        // Conversions
        Vector4D::operator const Point3D(void)
        {
            static Point3D s_Point;

            s_Point.x = x;
            s_Point.y = y;
            s_Point.z = z;

            return s_Point;
        }
        Vector4D::operator const Vector3D(void)
        {
            static Vector3D s_Vector;

            s_Vector.x = x;
            s_Vector.y = y;
            s_Vector.z = z;

            return s_Vector;
        }

        // +---------
        // | Acces Operators
        // +---------
        const Float& Vector4D::operator[](Int i) const
        {
            assert(i >= 0 && i < 4);
            return *((&x) + i);
        }

        Float& Vector4D::operator[](Int i)
        {
            assert(i >= 0 && i < 4);
            return *((&x) + i);
        }

        // ----------------------------------------------------------------------
        // +---------
        // | Addition/Substraction
        // +---------
        // With another vector (Internal laws)
        Vector4D operator + (const Vector4D& U, const Vector4D& V)
        {
            return Vector4D(U.x + V.x, U.y + V.y, U.z + V.z, U.w + V.w);
        }
        Vector4D operator - (const Vector4D& U, const Vector4D& V)
        {
            return Vector4D(U.x - V.x, U.y - V.y, U.z - V.z, U.w - V.w);
        }

        // With assignement
        const Vector4D& Vector4D::operator += (const Vector4D& V)
        {
            x += V.x;
            y += V.y;
            z += V.z;
            w += V.w;

            return *this;
        }
        const Vector4D& Vector4D::operator -= (const Vector4D& V)
        {
            x -= V.x;
            y -= V.y;
            z -= V.z;
            w -= V.w;

            return *this;
        }

        // Unary
        Vector4D operator - (const Vector4D& V)
        {
            return Vector4D(-V.x, -V.y, -V.z, -V.w);
        }


        // ----------------------------------------------------------------------
        // +---------
        // | Multiplication/Division
        // +---------
        // With a scalar (External laws)
        Vector4D operator * (const Float& fValue, const Vector4D& V)
        {
            return Vector4D(fValue * V.x, fValue * V.y, fValue * V.z, fValue * V.w);
        }
        Vector4D operator * (const Vector4D& V, const Float& fValue)
        {
            return Vector4D(fValue * V.x, fValue * V.y, fValue * V.z, fValue * V.w);
        }
        Vector4D operator / (const Float& fValue, const Vector4D& V)
        {
            return Vector4D(fValue / V.x, fValue / V.y, fValue / V.z, fValue / V.w);
        }
        Vector4D operator / (const Vector4D& V, const Float& fValue)
        {
            return Vector4D(V.x / fValue, V.y / fValue, V.z / fValue, V.w / fValue);
        }

        // With assignement
        const Vector4D& Vector4D::operator *= (const Float& fValue)
        {
            x *= fValue;
            y *= fValue;
            z *= fValue;
            w *= fValue;

            return *this;
        }

        const Vector4D& Vector4D::operator *= (const Vector4D& other)
        {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            w *= other.w;

            return *this;
        }

        const Vector4D& Vector4D::operator /= (const Float& fValue)
        {
            Float tmp = ((Float)1.0) / fValue;
            x *= tmp;
            y *= tmp;
            z *= tmp;
            w *= tmp;

            return *this;
        }

        const Vector4D& Vector4D::operator /= (const Vector4D& other)
        {
            x /= other.x;
            y /= other.y;
            z /= other.z;
            w /= other.w;

            return *this;
        }


        // ----------------------------------------------------------------------
        // +---------
        // | Euclidian operations
        // +---------
        Float Dot(const Vector4D& U, const Vector4D& V)
        {
            Float s_Float;

            s_Float = (U.x * V.x) + (U.y * V.y) + (U.z * V.z) + (U.w * V.w);

            return s_Float;
        }
        Float Norm(const Vector4D& V)
        {
            Float s_Float;

            s_Float = sqrtF(sqr(V.x) + sqr(V.y) + sqr(V.z) + sqr(V.w));

            return s_Float;
        }
        Float NormSquare(const Vector4D& V)
        {
            Float s_Float;

            s_Float = sqr(V.x) + sqr(V.y) + sqr(V.z) + sqr(V.w);

            return s_Float;
        }

        // ----------------------------------------------------------------------
        // +---------
        // | Affine operations
        // +---------
        Vector4D Mid(const Vector4D& U, const Vector4D& V)
        {
            return Vector4D((U.x + V.x) * 0.5f, (U.y + V.y) * 0.5f, (U.z + V.z) * 0.5f, (U.w + V.w) * 0.5f);
        }
        Vector4D Bary(const Float& a, const Vector4D& U, const Float& b, const Vector4D& V)
        {
            return Vector4D(((a * U.x) + (b * V.x)), ((a * U.y) + (b * V.y)), ((a * U.z) + (b * V.z)), ((a * U.w) + (b * V.w)));
        }





        bool operator==(const Vector4D& U, const Vector4D& V)
        {
            return (U.x == V.x && U.y == V.y && U.z == V.z && U.w == V.w);
        }

        bool operator!=(const Vector4D& U, const Vector4D& V)
        {
            return (U.x != V.x || U.y != V.y || U.z != V.z || U.w != V.w);
        }


        // ----------------------------------------------------------------------
        // +---------
        // | Utilities
        // +---------
        void Vector4D::Normalize(void)
        {
            Float tmp = Norm(*this);
            if (tmp != (Float)0.0)
            {
                tmp = ((Float)1.0) / tmp;
                x *= tmp;
                y *= tmp;
                z *= tmp;
                w *= tmp;
            }

        }

        inline Vector4D Vector4D::Normalized(void)
        {
            Vector4D result = *this;
            result.Normalize();
            return result;
        }

        inline Vector4D Hadamard(Vector4D a, const Vector4D& b)
        {
            a *= b;
            return a;
        }
    }
}