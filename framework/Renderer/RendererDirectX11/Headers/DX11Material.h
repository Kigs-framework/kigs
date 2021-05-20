#ifndef _DX11MATERIAL_H
#define _DX11MATERIAL_H

#include "Material.h"
//#include "GLSLUniform.h"
//#include "ModuleSceneGraph.h"

struct ID3D11Buffer;
// ****************************************
// * DX11Material class
// * --------------------------------------
/**
 * \file	DX11Material.h
 * \class	DX11Material
 * \ingroup Renderer
 * \brief	DX11 implementation of Material.
 *
 */
 // ****************************************
class DX11Material : public Material
{
public:
	DECLARE_CLASS_INFO(DX11Material, Material, Renderer)
	DECLARE_INLINE_CONSTRUCTOR(DX11Material) {}
	~DX11Material() override;

	bool	PreDraw(TravState*) override;
	bool	PostDraw(TravState* travstate) override;

protected:
	void NotifyUpdate(unsigned int labelid) override;

};

#endif //_DX11MATERIAL_H
