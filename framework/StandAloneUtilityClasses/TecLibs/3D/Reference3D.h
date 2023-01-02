// **********************************************************************
// * FILE  : Reference3D.h
// * GROUP : TecLibs/3D
// *---------------------------------------------------------------------
// * PURPOSE : Implentation of a class for 3D basis
// * COMMENT : All operations (+/-/etc.) are defined in Matrix3x3
// **********************************************************************
#pragma once

#include "TecLibs/Tec3D.h"
namespace Kigs
{
    namespace Maths
    {
        // ----------------------------------------------------------------------
        // **************************************************
        // * Reference3D class
        // *-------------------------------------------------
        // * - Implementation of a class for basis
        // * - 
        // **************************************************
        class Reference3D
        {
        public:
            // +---------
            // | Life-cyle
            // +---------
            // Constructors
            inline Reference3D();
            // DO NOTHING! (For immediate creation)
            inline Reference3D(const Point3D& O, const Vector3D& I, const Vector3D& J, const Vector3D& K);
            // Set vector to I,J,K and point to O
            inline Reference3D(const Matrix3x3& M, const Vector3D& T);
            // Set vector to matrix M and translation T
            inline Reference3D(const Matrix3x4& M);
            // Set vector to matrix M

        // Assignement
            inline const Reference3D operator =(const Reference3D& R);
            // Assign the reference R

        // Set
            inline void Set(const Point3D& O, const Vector3D& I, const Vector3D& J, const Vector3D& K);
            inline void Set(const Matrix3x3& M, const Vector3D& T);
            inline void Set(const Matrix3x4& M);
            inline void Set(const Reference3D& R);

            // Conversions
            inline operator const Matrix3x4(void);


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
            Point3D     O;
            Vector3D    I;
            Vector3D    J;
            Vector3D    K;



        private:
            // +---------
            // | Variables
            // +---------
            // Temporary reference for inline returns
            static Reference3D ms_Reference;
        };

    }
}


