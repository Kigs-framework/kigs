#ifndef _GLSLUIShader_H
#define _GLSLUIShader_H

#include "GLSLGenericMeshShader.h"
#include "ModuleRenderer.h"

class API3DUIShader : public API3DGenericMeshShader
{
public:
	friend class RendererOpenGL;

	DECLARE_CLASS_INFO(API3DUIShader, API3DGenericMeshShader, Renderer)

	API3DUIShader(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);


protected:
	virtual ~API3DUIShader();

	kstl::string GetVertexShader() override;
	kstl::string GetFragmentShader() override;


	bool bgr = false;
	virtual unsigned int CustomizeShaderFlag(TravState* state, unsigned int attrib_flag, unsigned int current_shader_flag)
	{
		if (current_shader_flag & ModuleRenderer::SHADER_FLAGS_USER1)
		{
			bgr = true;
		}
		else
		{
			bgr = false;
		}

		return current_shader_flag;
	}

};


#endif //_GLSLUIShader_H

