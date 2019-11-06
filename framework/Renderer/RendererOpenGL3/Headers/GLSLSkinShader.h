#ifndef _GLSLSKINSHADER_H
#define _GLSLSKINSHADER_H

#include "GLSLGenericMeshShader.h"

class API3DSkinShader : public API3DGenericMeshShader
{
public:
	DECLARE_CLASS_INFO(API3DSkinShader, API3DGenericMeshShader, Renderer)
	DECLARE_INLINE_CONSTRUCTOR(API3DSkinShader) {}

protected:
	kstl::string GetVertexShader() override;
};


#endif //_GLSLSKINSHADER_H

