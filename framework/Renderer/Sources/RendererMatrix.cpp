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

void	RendererMatrix::Init(const mat4& matrix)
{
	mMatrix =  matrix;
}

void	RendererMatrix::InitToIdentity()
{
	mMatrix = mat4(1.0f);
}