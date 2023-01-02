// **********************************************************************
// * FILE  : Basis3D.cpp
// * GROUP : TecLibs/3D
// *---------------------------------------------------------------------
// * PURPOSE : Implentation
// * COMMENT :
// *---------------------------------------------------------------------
// **********************************************************************
namespace Kigs
{
    namespace Maths
    {


        // ----------------------------------------------------------------------
        // +---------
        // | Life-cyle
        // +---------
        // Constructors
        Basis3D::Basis3D()
        {
            ;
        }
        Basis3D::Basis3D(const Vector3D& I, const Vector3D& J, const Vector3D& K)
            :I(I), J(J), K(K)
        {
            ;
        }
        Basis3D::Basis3D(const Matrix3x3& M)
            :I(M.e[0][0], M.e[0][1], M.e[0][2]),
            J(M.e[1][0], M.e[1][1], M.e[1][2]),
            K(M.e[2][0], M.e[2][1], M.e[2][2])
        {
            ;
        }

        // Assignement
        const Basis3D& Basis3D::operator =(const Basis3D& B)
        {
            ms_Basis.I = B.I;
            ms_Basis.J = B.J;
            ms_Basis.K = B.K;

            return ms_Basis;
        }

        // Set
        void Basis3D::Set(const Vector3D& I, const Vector3D& J, const Vector3D& K)
        {
            this->I = I;
            this->J = J;
            this->K = K;
        }
        void Basis3D::Set(const Matrix3x3& M)
        {
            I = Vector3D(M.e[0][0], M.e[0][1], M.e[0][2]);
            J = Vector3D(M.e[1][0], M.e[1][1], M.e[1][2]);
            K = Vector3D(M.e[2][0], M.e[2][1], M.e[2][2]);
        }
        void Basis3D::Set(const Basis3D& B)
        {
            I = B.I;
            J = B.J;
            K = B.K;
        }

        // Conversions
        Basis3D::operator const Matrix3x3& (void)
        {
            static Matrix3x3 s_Matrix;

            s_Matrix.e[0][0] = I.x, s_Matrix.e[0][1] = I.y, s_Matrix.e[0][2] = I.z;
            s_Matrix.e[1][0] = J.x, s_Matrix.e[1][1] = J.y, s_Matrix.e[1][2] = J.z;
            s_Matrix.e[2][0] = K.x, s_Matrix.e[2][1] = K.y, s_Matrix.e[2][2] = K.z;

            return s_Matrix;
        }


        // ----------------------------------------------------------------------
        // +---------
        // | Utilities
        // +---------
        void Basis3D::SetIdentity(void)
        {
            I = Vector3D(1.0f, 0.0f, 0.0f);
            J = Vector3D(0.0f, 1.0f, 0.0f);
            K = Vector3D(0.0f, 0.0f, 1.0f);
        }
        void Basis3D::Normalize(void)
        {
            I.Normalize();
            J.Normalize();
            K.Normalize();
        }
        void Basis3D::Orthogonalize(void)
        {
            K = I ^ J;
            J = K ^ I;
        }
        void Basis3D::OrthoNormalize(void)
        {
            Orthogonalize();
            Normalize();
        }


    }
}