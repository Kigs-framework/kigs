#include "PrecompiledHeaders.h"

#include "RendererMatrix.h"

using namespace Kigs::Draw;

IMPLEMENT_CLASS_INFO(RendererMatrix)

RendererMatrix::RendererMatrix(const std::string& name,CLASS_NAME_TREE_ARG) : Drawable(name,PASS_CLASS_NAME_TREE_ARG)
{
	InitToIdentity();
}    
    
RendererMatrix::~RendererMatrix()
{
}    

void	RendererMatrix::Init(const mat3x4& matrix)
{
	mMatrix[0] = { matrix[0] , 0.0f };
	mMatrix[1] = { matrix[1] , 0.0f };
	mMatrix[2] = { matrix[2] , 0.0f };
	mMatrix[3] = { matrix[3] , 1.0f };
}

void	RendererMatrix::Init(const mat4& matrix)
{
	mMatrix = matrix;
}

void	RendererMatrix::InitToIdentity()
{
	mMatrix.SetIdentity();
}