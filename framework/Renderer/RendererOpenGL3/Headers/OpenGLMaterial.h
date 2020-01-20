#ifndef _OPENGLMATERIAL_H
#define _OPENGLMATERIAL_H

#include "Material.h"
#include "GLSLUniform.h"
//#include "GLSLUniform.h"
//#include "ModuleSceneGraph.h"


class OpenGLMaterial : public Material
{
public:
	DECLARE_CLASS_INFO(OpenGLMaterial, Material, Renderer)
	DECLARE_INLINE_CONSTRUCTOR(OpenGLMaterial) {}

	bool	PreDraw(TravState*) override;
	bool	PostDraw(TravState* travstate) override;

protected:
	void InitModifiable() override;

	void NotifyUpdate(unsigned int labelid) override;

	SP<API3DUniformBase> uniDiffuseColor = nullptr;
	SP<API3DUniformBase> uniSpecularColor = nullptr;
	SP<API3DUniformBase> uniAmbiantColor = nullptr;
	SP<API3DUniformBase> uniShininess = nullptr;
};

#endif //_OPENGLMATERIAL_H
