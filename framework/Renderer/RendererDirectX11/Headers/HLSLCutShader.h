#pragma once
#include "HLSLGenericMeshShader.h"

class API3DCutShader : public API3DGenericMeshShader
{
public:
	DECLARE_CLASS_INFO(API3DCutShader, API3DGenericMeshShader, Renderer)
	DECLARE_CONSTRUCTOR(API3DCutShader);

protected:
	std::string GetVertexShader() override;
	std::string GetFragmentShader() override;

	void NotifyUpdate(unsigned int labelid) override;
	
	maInt PlaneCount = BASE_ATTRIBUTE(PlaneCount, 1);
};

