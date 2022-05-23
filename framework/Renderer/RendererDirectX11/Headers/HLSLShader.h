#ifndef _HLSLSHADER_H
#define _HLSLSHADER_H

#include "Shader.h"

struct ID3D11DeviceChild;
struct ID3D11ShaderReflection;
struct ID3D11InputLayout;
struct ID3D11SamplerState;
struct D3D11_SAMPLER_DESC;

class HLSLShaderInfo : public ShaderInfo
{
public:
	ID3D11DeviceChild *			mInternalShaderStruct = nullptr;
	ID3D11ShaderReflection*		mReflector = nullptr;
	void*						mBlob = nullptr;
	virtual ~HLSLShaderInfo();
};

class HLSLProgramInfo : public ShaderInfo
{
public:
	std::unordered_map<size_t, ID3D11InputLayout*> mLayouts;

	virtual ~HLSLProgramInfo();
};

// ****************************************
// * API3DShader class
// * --------------------------------------
/**
 * \file	HLSLShader.h
 * \class	API3DShader
 * \ingroup Renderer
 * \brief	DX11 Shader.
 */
 // ****************************************
class API3DShader : public ShaderBase
{
public:
	friend class RendererOpenGL;

	DECLARE_CLASS_INFO(API3DShader, ShaderBase, Renderer)

	API3DShader(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	~API3DShader();


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

	bool isOKToUse() 
	{
		if (GetCurrentShaderProgram()) return true;
		return false;
	}
	void ChooseShader(TravState* state, unsigned int attribFlag) override {}


	void PushUniform(CoreModifiable*) override;
	void PopUniform(CoreModifiable*) override;

	void setLayout();

protected:

	virtual ShaderInfo *	CreateProgram() override;


	virtual void	Active(TravState* state, bool resetUniform = false) override;
	virtual void	Deactive(TravState* state) override;


	virtual BuildShaderStruct*	Rebuild() override;
	void	DelayedInit(TravState* state);

	void	InitModifiable()  override;
	

	//virtual void ActiveSampler();
	//virtual void DeactiveSampler();

	//ID3D11SamplerState* CreateSampler(D3D11_SAMPLER_DESC* desc);
	//ID3D11SamplerState* m_sampleState = nullptr;

};
#endif //_HLSLSHADER_H
