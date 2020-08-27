#ifndef _OPENGLRENDERINGMATRIX_H_
#define _OPENGLRENDERINGMATRIX_H_

#include "RendererMatrix.h"

// ****************************************
// * OpenGLRenderingMatrix class
// * --------------------------------------
/**
 * \file	OpenGLRenderingMatrix.h
 * \class	OpenGLRenderingMatrix
 * \ingroup Renderer
 * \brief	OpenGL implementation of RendererMatrix.
 */
 // ****************************************
class OpenGLRenderingMatrix : public RendererMatrix
{
public:        
    DECLARE_CLASS_INFO(OpenGLRenderingMatrix,RendererMatrix,Renderer)    
    
    OpenGLRenderingMatrix(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	void	PushMatrix(TravState* state) const override;
	void	SetMatrix(TravState* state) const override;
	void	PopMatrix(TravState* state) const override;
	void	RetrieveFromCurrentRenderingState(TravState* state)  override;

protected:    
    virtual ~OpenGLRenderingMatrix();
}; 

#endif //_OPENGLRENDERINGMATRIX_H_
