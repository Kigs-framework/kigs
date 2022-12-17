#ifndef _GLSLUIShader_H
#define _GLSLUIShader_H

#include "GLSLGenericMeshShader.h"
#include "ModuleRenderer.h"

// ****************************************
// * API3DUIShader class
// * --------------------------------------
/**
 * \file	GLSLUIShader.h
 * \class	API3DUIShader
 * \ingroup Renderer
 * \brief	Specialized GLSL Shader for UI.
 */
 // ****************************************

class API3DUIShader : public API3DGenericMeshShader
{
public:
	friend class RendererOpenGL;

	DECLARE_CLASS_INFO(API3DUIShader, API3DGenericMeshShader, Renderer)
	API3DUIShader(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	virtual ~API3DUIShader();

protected:
	

	std::string GetVertexShader() override;
	std::string GetFragmentShader() override;


	bool mBGR = false;
	unsigned int CustomizeShaderFlag(TravState* state, unsigned int attrib_flag, unsigned int current_shader_flag) override
	{
		if (current_shader_flag & ModuleRenderer::SHADER_FLAGS_USER1)
		{
			mBGR = true;
		}
		else
		{
			mBGR = false;
		}

		return current_shader_flag;
	}

};


#endif //_GLSLUIShader_H

