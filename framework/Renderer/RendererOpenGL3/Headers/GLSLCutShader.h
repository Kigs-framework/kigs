#ifndef _GLSLCutShader_H
#define _GLSLCutShader_H

#include "GLSLGenericMeshShader.h"

class API3DCutShader : public API3DGenericMeshShader
{
public:
	DECLARE_CLASS_INFO(API3DCutShader, API3DGenericMeshShader, Renderer)
	DECLARE_CONSTRUCTOR(API3DCutShader);

protected:
	kstl::string GetVertexShader() override;
	kstl::string GetFragmentShader() override;
	void NotifyUpdate(unsigned int labelid) override;
	
	maInt PlaneCount = BASE_ATTRIBUTE(PlaneCount, 1);
};


#endif //_GLSLCutShader_H

