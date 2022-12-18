#ifndef _HLSLGENERICMESHSHADER_H
#define _HLSLGENERICMESHSHADER_H

#include "HLSLShader.h"


// ****************************************
// * API3DGenericMeshShader class
// * --------------------------------------
/**
 * \file	HLSLGenericMeshShader.h
 * \class	API3DGenericMeshShader
 * \ingroup Renderer
 * \brief	DX11 Generic Mesh Shader.
 */
 // ****************************************
class API3DGenericMeshShader : public API3DShader
{
public:
	friend class RendererOpenGL;

	DECLARE_CLASS_INFO(API3DGenericMeshShader, API3DShader, Renderer)

	API3DGenericMeshShader(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);


	void ChooseShader(TravState* state, unsigned int attribFlag) override;

	static std::string GetDefaultLightingFunctions();
	static std::string GetDefaultVaryings();
	static std::string GetDefaultFragmentShaderUniforms();
	static std::string GetDefaultVertexShaderUniforms();
	static std::string GetDefaultVertexShaderAttributes();
	static std::string GetDefaultVertexShaderMain();
	static std::string GetDefaultFragmentShaderMain(const std::string& inject_before_light_calcs = "", const std::string & inject_top_of_main="");

protected:

	void InitModifiable() override;
	void Dealloc() override;

	// Allow child classes to set the flag correctly for shader caching
	virtual unsigned int CustomizeShaderFlag(TravState* state, unsigned int attrib_flag, unsigned int current_shader_flag)
	{
		return current_shader_flag;
	}

	virtual std::string GetVertexShader();
	virtual std::string GetFragmentShader();
	virtual std::string GetGeometryShader() { return ""; };

};


#endif //_HLSLGENERICMESHSHADER_H
