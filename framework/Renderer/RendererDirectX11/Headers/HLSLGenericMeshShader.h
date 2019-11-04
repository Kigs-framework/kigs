#ifndef _HLSLGENERICMESHSHADER_H
#define _HLSLGENERICMESHSHADER_H

#include "HLSLShader.h"

class API3DGenericMeshShader : public API3DShader
{
public:
	friend class RendererOpenGL;

	DECLARE_CLASS_INFO(API3DGenericMeshShader, API3DShader, Renderer)

	API3DGenericMeshShader(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);


	void ChooseShader(TravState* state, unsigned int attribFlag) override;

	static kstl::string GetDefaultLightingFunctions();
	static kstl::string GetDefaultVaryings();
	static kstl::string GetDefaultFragmentShaderUniforms();
	static kstl::string GetDefaultVertexShaderUniforms();
	static kstl::string GetDefaultVertexShaderAttributes();
	static kstl::string GetDefaultVertexShaderMain();
	static kstl::string GetDefaultFragmentShaderMain(const kstl::string& inject_before_light_calcs = "", const std::string & inject_top_of_main="");

protected:

	void InitModifiable() override;
	void Dealloc() override;

	// Allow child classes to set the flag correctly for shader caching
	virtual unsigned int CustomizeShaderFlag(TravState* state, unsigned int attrib_flag, unsigned int current_shader_flag)
	{
		return current_shader_flag;
	}

	virtual kstl::string GetVertexShader();
	virtual kstl::string GetFragmentShader();

};


#endif //_HLSLGENERICMESHSHADER_H
