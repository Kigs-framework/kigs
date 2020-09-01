#ifndef _OPENGLMATERIAL_H
#define _OPENGLMATERIAL_H

#include "Material.h"
#include "GLSLUniform.h"

// ****************************************
// * OpenGLMaterial class
// * --------------------------------------
/**
 * \file	OpenGLMaterial.h
 * \class	OpenGLMaterial
 * \ingroup Renderer
 * \brief	OpenGL implementation of Material.
 */
 // ****************************************

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

	SP<API3DUniformBase> mUniDiffuseColor = nullptr;
	SP<API3DUniformBase> mUniSpecularColor = nullptr;
	SP<API3DUniformBase> mUniAmbiantColor = nullptr;
	SP<API3DUniformBase> mUniShininess = nullptr;
};

#endif //_OPENGLMATERIAL_H
