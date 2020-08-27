#include "PrecompiledHeaders.h"

#include "RendererMatrix.h"

IMPLEMENT_CLASS_INFO(RendererMatrix)

RendererMatrix::RendererMatrix(const kstl::string& name,CLASS_NAME_TREE_ARG) : Drawable(name,PASS_CLASS_NAME_TREE_ARG),
mMatrix(*this,false,LABEL_AND_ID(Matrix))
{
	InitToIdentity();
}    
    
RendererMatrix::~RendererMatrix()
{
}    

void	RendererMatrix::Init(const Matrix3x4& matrix)
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

	mMatrix[3]=mMatrix[7]=mMatrix[11]=KFLOAT_CONST(0.0f);

	mMatrix[15]=KFLOAT_CONST(1.0f);
}

void	RendererMatrix::Init(const Matrix4x4& matrix)
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

void	RendererMatrix::InitToIdentity()
{
	mMatrix[0]=	KFLOAT_CONST(1.0f);
	mMatrix[1]=	KFLOAT_CONST(0.0f);
	mMatrix[2]=	KFLOAT_CONST(0.0f);

	mMatrix[4]=	KFLOAT_CONST(0.0f);
	mMatrix[5]=	KFLOAT_CONST(1.0f);
	mMatrix[6]=	KFLOAT_CONST(0.0f);

	mMatrix[8]=	KFLOAT_CONST(0.0f);
	mMatrix[9]=	KFLOAT_CONST(0.0f);
	mMatrix[10]=	KFLOAT_CONST(1.0f);

	mMatrix[12]=	KFLOAT_CONST(0.0f);
	mMatrix[13]=	KFLOAT_CONST(0.0f);
	mMatrix[14]=	KFLOAT_CONST(0.0f);

	mMatrix[3]=mMatrix[7]=mMatrix[11]=KFLOAT_CONST(0.0f);

	mMatrix[15]=KFLOAT_CONST(1.0f);
}