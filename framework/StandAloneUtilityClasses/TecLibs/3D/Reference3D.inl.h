// **********************************************************************
// * FILE  : Reference3D.cpp
// * GROUP : TecLibs/3D
// *---------------------------------------------------------------------
// * PURPOSE : Implentation
// * COMMENT :
// **********************************************************************

// ----------------------------------------------------------------------
// +---------
// | Life-cyle
// +---------
// Constructors
Reference3D::Reference3D()
{
    ;
}
Reference3D::Reference3D( const Point3D& O , const Vector3D& I , const Vector3D& J , const Vector3D& K )
:O(O),I(I),J(J),K(K)
{
    ;
}
Reference3D::Reference3D( const Matrix3x3& M , const Vector3D& T )
:O(T),
I(M.e[0][0],M.e[0][1],M.e[0][2]),
J(M.e[1][0],M.e[1][1],M.e[1][2]),
K(M.e[2][0],M.e[2][1],M.e[2][2])
{
    ;
}
Reference3D::Reference3D( const Matrix3x4& M )
:O(M.e[3][0],M.e[3][1],M.e[3][2]),
I(M.e[0][0],M.e[0][1],M.e[0][2]),
J(M.e[1][0],M.e[1][1],M.e[1][2]),
K(M.e[2][0],M.e[2][1],M.e[2][2])
{
    ;
}

// Assignement
const Reference3D Reference3D::operator =( const Reference3D& R )
{
    ms_Reference.O = R.O;
    ms_Reference.I = R.I;
    ms_Reference.J = R.J;
    ms_Reference.K = R.K;

    return ms_Reference;
}

// Set
void Reference3D::Set( const Point3D& O , const Vector3D& I , const Vector3D& J , const Vector3D& K )
{
    this->O = O;
    this->I = I;
    this->J = J;
    this->K = K;
}
void Reference3D::Set( const Matrix3x3& M , const Vector3D& T )
{
    O = Point3D(T);
    I = Vector3D(M.e[0][0],M.e[0][1],M.e[0][2]);
    J = Vector3D(M.e[1][0],M.e[1][1],M.e[1][2]);
    K = Vector3D(M.e[2][0],M.e[2][1],M.e[2][2]);
}
void Reference3D::Set( const Matrix3x4& M )
{
    O = Point3D(M.e[3][0],M.e[3][1],M.e[3][2]);
    I = Vector3D(M.e[0][0],M.e[0][1],M.e[0][2]);
    J = Vector3D(M.e[1][0],M.e[1][1],M.e[1][2]);
    K = Vector3D(M.e[2][0],M.e[2][1],M.e[2][2]);
}
void Reference3D::Set( const Reference3D& R )
{
    O = R.O;
    I = R.I;
    J = R.J;
    K = R.K;
}

// Conversions
Reference3D::operator const Matrix3x4 ( void )
{
    static Matrix3x4 s_Matrix;

    s_Matrix.e[3][0] = O.x ,s_Matrix.e[3][1] = O.y ,s_Matrix.e[3][2] = O.z;
    s_Matrix.e[0][0] = I.x ,s_Matrix.e[0][1] = I.y ,s_Matrix.e[0][2] = I.z;
    s_Matrix.e[1][0] = J.x ,s_Matrix.e[1][1] = J.y ,s_Matrix.e[1][2] = J.z;
    s_Matrix.e[2][0] = K.x ,s_Matrix.e[2][1] = K.y ,s_Matrix.e[2][2] = K.z;

    return s_Matrix;
}


// ----------------------------------------------------------------------
// +---------
// | Utilities
// +---------
void Reference3D::SetIdentity( void )
{
    O = Point3D(0,0,0);
    I = Vector3D(1,0,0);
    J = Vector3D(0,1,0);
    K = Vector3D(0,0,1);
}
void Reference3D::Normalize( void )
{
    I.Normalize();
    J.Normalize();
    K.Normalize();
}
void Reference3D::Orthogonalize( void )
{
    K = I ^ J;
    J = K ^ I;
}
void Reference3D::OrthoNormalize( void )
{
    Orthogonalize();
    Normalize();
}


