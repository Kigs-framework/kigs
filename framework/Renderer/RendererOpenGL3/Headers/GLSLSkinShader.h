#ifndef _GLSLSKINSHADER_H
#define _GLSLSKINSHADER_H

#include "GLSLGenericMeshShader.h"

// ****************************************
// * API3DSkinShader class
// * --------------------------------------
/**
 * \file	GLSLSkinShader.h
 * \class	API3DSkinShader
 * \ingroup Renderer
 * \brief	Specialized GLSL Shader for skinned mesh.
 */
 // ****************************************

class API3DSkinShader : public API3DGenericMeshShader
{
public:
	DECLARE_CLASS_INFO(API3DSkinShader, API3DGenericMeshShader, Renderer)
	DECLARE_INLINE_CONSTRUCTOR(API3DSkinShader) {}

protected:
	kstl::string GetVertexShader() override;
};


#endif //_GLSLSKINSHADER_H

