#ifndef _HLSLUIShader_H
#define _HLSLUIShader_H

#include "HLSLGenericMeshShader.h"
#include "ModuleRenderer.h"

// ****************************************
// * API3DUIShader class
// * --------------------------------------
/**
 * \file	HLSLUIShader.h
 * \class	API3DUIShader
 * \ingroup Renderer
 * \brief	DX11 UI Shader.
 */
 // ****************************************

class API3DUIShader : public API3DGenericMeshShader
{
public:

	DECLARE_CLASS_INFO(API3DUIShader, API3DGenericMeshShader, Renderer)
	API3DUIShader(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	virtual ~API3DUIShader();

protected:
	std::string GetVertexShader() override;
	std::string GetFragmentShader() override;


	bool mBGR = false;
	virtual unsigned int CustomizeShaderFlag(TravState* state, unsigned int attrib_flag, unsigned int current_shader_flag)
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


#endif //_HLSLUIShader_H

