#ifndef _DX11MATERIAL_H
#define _DX11MATERIAL_H

#include "Material.h"
//#include "GLSLUniform.h"
//#include "ModuleSceneGraph.h"

struct ID3D11Buffer;

class DX11Material : public Material
{
public:
	DECLARE_CLASS_INFO(DX11Material, Material, Renderer)
	DECLARE_INLINE_CONSTRUCTOR(DX11Material) {}

	bool	PreDraw(TravState*) override;
	bool	PostDraw(TravState* travstate) override;

protected:
	~DX11Material() override;
	
	void NotifyUpdate(unsigned int labelid) override;

	//ID3D11Buffer* m_materialBuffer= nullptr;
	//u32 m_material_buffer_crc = 0;
};

#endif //_DX11MATERIAL_H
