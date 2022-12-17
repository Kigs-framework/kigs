#ifndef _GLSLCutShader_H
#define _GLSLCutShader_H

#include "GLSLGenericMeshShader.h"


// ****************************************
// * API3DCutShader class
// * --------------------------------------
/**
 * \file	GLSLCutShader.h
 * \class	API3DCutShader
 * \ingroup Renderer
 * \brief	Specialized GLSL Shader for Cut planes.
 */
 // ****************************************
class API3DCutShader : public API3DGenericMeshShader
{
public:
	DECLARE_CLASS_INFO(API3DCutShader, API3DGenericMeshShader, Renderer)
	DECLARE_CONSTRUCTOR(API3DCutShader);

protected:
	std::string GetVertexShader() override;
	std::string GetFragmentShader() override;
	void NotifyUpdate(unsigned int labelid) override;
	
	maInt mPlaneCount = BASE_ATTRIBUTE(PlaneCount, 1);
};


#endif //_GLSLCutShader_H

