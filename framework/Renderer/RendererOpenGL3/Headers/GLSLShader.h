#ifndef _GLSLSHADER_H
#define _GLSLSHADER_H

#include "Drawable.h"
#include "maReference.h"
#include "GLSLUniform.h"
#include "Shader.h"

class API3DUniformBase;
class Material;

class GLSLShaderInfo : public ShaderInfo
{
public:
	virtual ~GLSLShaderInfo();
};



class API3DShader : public ShaderBase
{
public:
	friend class RendererOpenGL;

	DECLARE_CLASS_INFO(API3DShader, ShaderBase,Renderer)

	API3DShader(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	void	NotifyUpdate(const unsigned int labelid ) override;
	bool	PreDraw(TravState*) override;
	bool	PostDraw(TravState*) override;

	/**
	* \brief	pre draw method
	* \fn 		virtual void	DoPreDraw(TravState*);
	* \param	TravState : camera state
	*/
	void	DoPreDraw(TravState*) override;

	/**
	* \brief	post draw method
	* \fn 		virtual void	DoPostDraw(TravState* travstate);
	* \param	travstate : camera state
	*/
	void	DoPostDraw(TravState* travstate) override;
	
	virtual void PushUniform(CoreModifiable*) override;
	virtual void PopUniform(CoreModifiable*) override;
	

	
	void	PrepareExport(ExportSettings* settings) override;
	void	EndExport(ExportSettings* settings) override;

	virtual void ChooseShader(TravState* state, unsigned int attribFlag) {};
	bool	isOKToUse() 
	{ 
		if(GetCurrentShaderProgram())
			return GetCurrentShaderProgram()->mID != 0xFFFFFFFF; return false;
	}
protected:

	void	DelayedInit(TravState* state);

	//protected callback
	DECLARE_VIRTUAL_METHOD(Reload);
	DECLARE_VIRTUAL_METHOD(Active);

	virtual void	Active(TravState* state, bool resetUniform=false);
	virtual void	Deactive(TravState* state);
	
	virtual void	Dealloc();
	virtual BuildShaderStruct*	Rebuild() override;

	void	InitModifiable()  override;
	void	ProtectedDestroy()  override;

};
#endif //_GLSLSHADER_H
