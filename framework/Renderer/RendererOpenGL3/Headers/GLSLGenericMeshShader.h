#ifndef _GLSLGENERICMESHSHADER_H
#define _GLSLGENERICMESHSHADER_H

#include "GLSLShader.h"
// ****************************************
// * API3DGenericMeshShader class
// * --------------------------------------
/**
 * \file	GLSLGenericMeshShader.h
 * \class	API3DGenericMeshShader
 * \ingroup Renderer
 * \brief	OpenGL generic mesh shader.
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
	static std::string GetDefaultFragmentShaderMain(const std::string& inject_before_light_calcs = "");

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

	
	struct shaderInfo
	{
		ShaderInfo *	mVertexShader;
		ShaderInfo *	mFragmentShader;
		ShaderInfo* 	mShaderProgram;
		
	};

};


#endif //_GLSLGENERICMESHSHADER_H
