#include "PrecompiledHeaders.h"

#include "TextureMatrix.h"

using namespace Kigs::Draw;

IMPLEMENT_CLASS_INFO(TextureMatrix)

TextureMatrix::TextureMatrix(const std::string& name,CLASS_NAME_TREE_ARG) : Drawable(name,PASS_CLASS_NAME_TREE_ARG)
{
}    

    
TextureMatrix::~TextureMatrix()
{
}    


void	TextureMatrix::Init(const Matrix3x4& matrix)
{
	mMatrix[0] = { matrix[0] , 0.0f };
	mMatrix[1] = { matrix[1] , 0.0f };
	mMatrix[2] = { matrix[2] , 0.0f };
	mMatrix[3] = { matrix[3] , 1.0f };
}

void	TextureMatrix::Init(const Matrix4x4& matrix)
{
	mMatrix=	matrix;
}

void	TextureMatrix::InitToIdentity()
{
	mMatrix.SetIdentity();
}