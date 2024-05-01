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

void	TextureMatrix::Init(const mat4& matrix)
{
	mMatrix=	matrix;
}

void	TextureMatrix::InitToIdentity()
{
	mMatrix = mat4(1.0f);
}