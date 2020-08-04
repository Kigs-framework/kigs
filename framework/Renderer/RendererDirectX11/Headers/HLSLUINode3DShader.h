#ifndef _GLSLUINode3DShader_H
#define _GLSLUINode3DShader_H

#include "HLSLGenericMeshShader.h"
#include "ModuleRenderer.h"

class API3DUINode3DShader : public API3DGenericMeshShader
{
public:
	friend class RendererOpenGL;

	DECLARE_CLASS_INFO(API3DUINode3DShader, API3DGenericMeshShader, Renderer)

	API3DUINode3DShader(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);


protected:
	virtual ~API3DUINode3DShader();

	std::string GetVertexShader() override;
	std::string GetFragmentShader() override;


	bool bgr = false;
	unsigned int CustomizeShaderFlag(TravState* state, unsigned int attrib_flag, unsigned int current_shader_flag) override
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


#endif //_GLSLUINode3DShader_H

