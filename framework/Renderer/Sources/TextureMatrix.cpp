#include "PrecompiledHeaders.h"

#include "TextureMatrix.h"

using namespace Kigs::Draw;

IMPLEMENT_CLASS_INFO(TextureMatrix)

TextureMatrix::TextureMatrix(const std::string& name,CLASS_NAME_TREE_ARG) : Drawable(name,PASS_CLASS_NAME_TREE_ARG),
mMatrix(*this,"Matrix")
{
}    

    
TextureMatrix::~TextureMatrix()
{
}    


void	TextureMatrix::Init(const Matrix3x4& matrix)
{
	mMatrix[0]=	matrix.e[0][0];
	mMatrix[1]=	matrix.e[0][1];
	mMatrix[2]=	matrix.e[0][2];

	mMatrix[4]=	matrix.e[1][0];
	mMatrix[5]=	matrix.e[1][1];
	mMatrix[6]=	matrix.e[1][2];

	mMatrix[8]=	matrix.e[2][0];
	mMatrix[9]=	matrix.e[2][1];
	mMatrix[10]=	matrix.e[2][2];

	mMatrix[12]=	matrix.e[3][0];
	mMatrix[13]=	matrix.e[3][1];
	mMatrix[14]=	matrix.e[3][2];

	mMatrix[3]=mMatrix[7]=mMatrix[11]=0.0f;

	mMatrix[15]=1.0f;
}

void	TextureMatrix::Init(const Matrix4x4& matrix)
{
	mMatrix[0]=	matrix.e[0][0];
	mMatrix[1]=	matrix.e[0][1];
	mMatrix[2]=	matrix.e[0][2];
	mMatrix[3]=	matrix.e[0][3];

	mMatrix[4]=	matrix.e[1][0];
	mMatrix[5]=	matrix.e[1][1];
	mMatrix[6]=	matrix.e[1][2];
	mMatrix[7]=	matrix.e[1][3];

	mMatrix[8]=	matrix.e[2][0];
	mMatrix[9]=	matrix.e[2][1];
	mMatrix[10]=	matrix.e[2][2];
	mMatrix[11]=	matrix.e[2][3];

	mMatrix[12]=	matrix.e[3][0];
	mMatrix[13]=	matrix.e[3][1];
	mMatrix[14]=	matrix.e[3][2];
	mMatrix[15]=	matrix.e[3][3];
}

void	TextureMatrix::InitToIdentity()
{
	mMatrix[0]=	1.0f;
	mMatrix[1]=	0.0f;
	mMatrix[2]=	0.0f;

	mMatrix[4]=	0.0f;
	mMatrix[5]=	1.0f;
	mMatrix[6]=	0.0f;

	mMatrix[8]=	0.0f;
	mMatrix[9]=	0.0f;
	mMatrix[10]=	1.0f;

	mMatrix[12]=	0.0f;
	mMatrix[13]=	0.0f;
	mMatrix[14]=	0.0f;

	mMatrix[3]=mMatrix[7]=mMatrix[11]=0.0f;

	mMatrix[15]=1.0f;
}