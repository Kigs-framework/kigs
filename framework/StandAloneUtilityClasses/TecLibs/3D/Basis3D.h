// **********************************************************************
// * FILE  : Basis3D.h
// * GROUP : TecLibs/3D
// *---------------------------------------------------------------------
// * PURPOSE : Implentation of a class for 3D basis
// * COMMENT : All operations (+/-/etc.) are defined in Matrix3x3
// **********************************************************************
#pragma once
namespace Kigs
{
    namespace Maths
    {




        // ----------------------------------------------------------------------
        // **************************************************
        // * Basis3D class
        // *-------------------------------------------------
        // * - Implementation of a class for basis
        // * - 
        // **************************************************
        class Basis3D
        {
        public:
            // +---------
            // | Life-cyle
            // +---------
            // Constructors
            inline Basis3D();
            // DO NOTHING! (For immediate creation)
            inline Basis3D(const Vector3D& I, const Vector3D& J, const Vector3D& K);
            // Set vector to I,J,K
            inline Basis3D(const Matrix3x3& M);
            // Set vector to matrix M

        // Assignement
            inline const Basis3D& operator =(const Basis3D& B);
            // Assign the basis B

        // Set (constructors-like)
            inline void Set(const Vector3D& I, const Vector3D& J, const Vector3D& K);
            inline void Set(const Matrix3x3& M);
            inline void Set(const Basis3D& B);

            // Conversions
            inline operator const Matrix3x3& (void);


            // +---------
            // | Utilities
            // +---------
            inline void SetIdentity(void);
            inline void Normalize(void);
            inline void Orthogonalize(void);
            inline void OrthoNormalize(void);


            // +---------
            // | Vectors
            // +---------
            Vector3D    I;
            Vector3D    J;
            Vector3D    K;



        private:
            // +---------
            // | Variables
            // +---------
            // Temporary basis for inline returns
            static Basis3D ms_Basis;
        };

    }
}

