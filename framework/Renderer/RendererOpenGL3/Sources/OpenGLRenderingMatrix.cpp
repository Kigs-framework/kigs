#include "PrecompiledHeaders.h"

#include "OpenGLRenderingMatrix.h"
#include "ModuleRenderer.h"
#include "TravState.h"

IMPLEMENT_CLASS_INFO(OpenGLRenderingMatrix)

OpenGLRenderingMatrix::OpenGLRenderingMatrix(const kstl::string& name,CLASS_NAME_TREE_ARG) : RendererMatrix(name,PASS_CLASS_NAME_TREE_ARG)
{
  // myMatrix=new GLfloat[16];
}     

OpenGLRenderingMatrix::~OpenGLRenderingMatrix()
{
//	delete[] myMatrix;
}    


void	OpenGLRenderingMatrix::PushMatrix(TravState* state) const 
{
	state->GetRenderer()->PushAndMultMatrix(MATRIX_MODE_MODEL, myMatrix.getConstArrayBuffer());
}

// don't forget to pop matrix after that
void	OpenGLRenderingMatrix::SetMatrix(TravState* state) const
{
	ModuleSpecificRenderer* renderer=state->GetRenderer();
	renderer->PushMatrix(MATRIX_MODE_MODEL);
	renderer->LoadMatrix(MATRIX_MODE_MODEL, myMatrix.getConstArrayBuffer());
}

void	OpenGLRenderingMatrix::RetrieveFromCurrentRenderingState(TravState* state)
{
	assert(0);
	//ModuleSpecificRenderer* renderer=state->GetRenderer();
	//glGetFloatv(GL_MODELVIEW_MATRIX,myMatrix.getArrayBuffer());
}

void	OpenGLRenderingMatrix::PopMatrix(TravState* state) const
{
	state->GetRenderer()->PopMatrix(MATRIX_MODE_MODEL);
}