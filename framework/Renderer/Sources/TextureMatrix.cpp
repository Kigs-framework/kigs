#include "PrecompiledHeaders.h"

#include "TextureMatrix.h"

IMPLEMENT_CLASS_INFO(TextureMatrix)

TextureMatrix::TextureMatrix(const kstl::string& name,CLASS_NAME_TREE_ARG) : Drawable(name,PASS_CLASS_NAME_TREE_ARG),
myMatrix(*this,false,LABEL_AND_ID(Matrix))
{
}    

    
TextureMatrix::~TextureMatrix()
{
}    


void	TextureMatrix::Init(const Matrix3x4& matrix)
{
	myMatrix[0]=	matrix.e[0][0];
	myMatrix[1]=	matrix.e[0][1];
	myMatrix[2]=	matrix.e[0][2];

	myMatrix[4]=	matrix.e[1][0];
	myMatrix[5]=	matrix.e[1][1];
	myMatrix[6]=	matrix.e[1][2];

	myMatrix[8]=	matrix.e[2][0];
	myMatrix[9]=	matrix.e[2][1];
	myMatrix[10]=	matrix.e[2][2];

	myMatrix[12]=	matrix.e[3][0];
	myMatrix[13]=	matrix.e[3][1];
	myMatrix[14]=	matrix.e[3][2];

	myMatrix[3]=myMatrix[7]=myMatrix[11]=KFLOAT_CONST(0.0f);

	myMatrix[15]=KFLOAT_CONST(1.0f);
}

void	TextureMatrix::Init(const Matrix4x4& matrix)
{
	myMatrix[0]=	matrix.e[0][0];
	myMatrix[1]=	matrix.e[0][1];
	myMatrix[2]=	matrix.e[0][2];
	myMatrix[3]=	matrix.e[0][3];

	myMatrix[4]=	matrix.e[1][0];
	myMatrix[5]=	matrix.e[1][1];
	myMatrix[6]=	matrix.e[1][2];
	myMatrix[7]=	matrix.e[1][3];

	myMatrix[8]=	matrix.e[2][0];
	myMatrix[9]=	matrix.e[2][1];
	myMatrix[10]=	matrix.e[2][2];
	myMatrix[11]=	matrix.e[2][3];

	myMatrix[12]=	matrix.e[3][0];
	myMatrix[13]=	matrix.e[3][1];
	myMatrix[14]=	matrix.e[3][2];
	myMatrix[15]=	matrix.e[3][3];
}

void	TextureMatrix::InitToIdentity()
{
	myMatrix[0]=	KFLOAT_CONST(1.0f);
	myMatrix[1]=	KFLOAT_CONST(0.0f);
	myMatrix[2]=	KFLOAT_CONST(0.0f);

	myMatrix[4]=	KFLOAT_CONST(0.0f);
	myMatrix[5]=	KFLOAT_CONST(1.0f);
	myMatrix[6]=	KFLOAT_CONST(0.0f);

	myMatrix[8]=	KFLOAT_CONST(0.0f);
	myMatrix[9]=	KFLOAT_CONST(0.0f);
	myMatrix[10]=	KFLOAT_CONST(1.0f);

	myMatrix[12]=	KFLOAT_CONST(0.0f);
	myMatrix[13]=	KFLOAT_CONST(0.0f);
	myMatrix[14]=	KFLOAT_CONST(0.0f);

	myMatrix[3]=myMatrix[7]=myMatrix[11]=KFLOAT_CONST(0.0f);

	myMatrix[15]=KFLOAT_CONST(1.0f);
}