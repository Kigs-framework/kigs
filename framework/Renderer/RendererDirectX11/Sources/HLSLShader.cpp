#include "HLSLShader.h"
#include "RendererDX11.h"
#include "HLSLUniform.h"
#include "ModuleSceneGraph.h"
#include "TravState.h"
#include <d3d11.h>
#include <d3dcompiler.h>


HLSLShaderInfo::~HLSLShaderInfo()
{
	if (internalShaderStruct)
		internalShaderStruct->Release();
	internalShaderStruct = nullptr;

	if (blob) ((ID3DBlob*)blob)->Release();
}

HLSLProgramInfo::~HLSLProgramInfo()
{
	for (auto m : m_layouts)
		m.second->Release();
}


IMPLEMENT_CLASS_INFO(API3DShader)

API3DShader::API3DShader(const kstl::string& name, CLASS_NAME_TREE_ARG) : ShaderBase(name, PASS_CLASS_NAME_TREE_ARG)
{
	
}

API3DShader::~API3DShader()
{
	
}

void API3DShader::setLayout()
{
	auto device = RendererDX11::theGlobalRenderer->as<RendererDX11>()->getDXInstance();
	ID3D11InputLayout* input_layout = nullptr;
	auto vbm = (VertexBufferManager*)RendererDX11::theGlobalRenderer->GetVertexBufferManager();
	auto hash = vbm->GetCurrentLayoutHash();
	
	if (hash == 0) return; // Keep the same input layout as previous

	if (GetCurrentShaderProgram<HLSLProgramInfo>()->m_layouts.find(hash) == GetCurrentShaderProgram<HLSLProgramInfo>()->m_layouts.end())
	{
		int descSize;
		D3D11_INPUT_ELEMENT_DESC* layout = vbm->CreateLayoutDesc(descSize);
		if (layout)
		{
			DX::ThrowIfFailed(device->m_device->CreateInputLayout(layout, descSize,
																	((ID3DBlob*)GetCurrentVertexShaderInfo<HLSLShaderInfo>()->blob)->GetBufferPointer(),
																	((ID3DBlob*)GetCurrentVertexShaderInfo<HLSLShaderInfo>()->blob)->GetBufferSize(),
																	&(GetCurrentShaderProgram<HLSLProgramInfo>()->m_layouts[hash])));
			input_layout = GetCurrentShaderProgram<HLSLProgramInfo>()->m_layouts[hash];
			free(layout);
		}
	}
	else
	{
		input_layout = GetCurrentShaderProgram<HLSLProgramInfo>()->m_layouts[hash];
	}
	vbm->ClearCurrentLayout();
	
	if (!input_layout)
	{
		KIGS_ERROR("Could not create input layout", 3);
	}
	//GetCurrentShaderProgram<HLSLProgramInfo>()->mID = 0;
	// Set the vertex input layout.
	device->m_deviceContext->IASetInputLayout(input_layout);
}

ShaderInfo* API3DShader::CreateProgram()
{

	HLSLProgramInfo * toReturn = new HLSLProgramInfo();

	return toReturn;
}

void API3DShader::DoPreDraw(TravState* state)
{
	if (PreDraw(state))
	{

		CoreModifiableAttribute* delayed = getAttribute("DelayedInit");
		if (delayed)
		{
			DelayedInit(state);
			RemoveDynamicAttribute("DelayedInit");
		}

		state->GetRenderer()->pushShader(this, state);

		// then PreDraw for sons
		kstl::vector<ModifiableItemStruct>::const_iterator it;

		for (it = getItems().begin(); it != getItems().end(); ++it)
		{
			if ((*it).myItem->isUserFlagSet(UserFlagDrawable))
			{
				Drawable* drawable = (Drawable*)(*it).myItem.get();
				drawable->CheckPreDraw(state);
			}
		}
	}
}

void API3DShader::DoPostDraw(TravState* state)
{
	if (PostDraw(state)) // first PostDraw for this
	{
		// then PostDraw for sons
		kstl::vector<ModifiableItemStruct>::const_iterator it;

		for (it = getItems().begin(); it != getItems().end(); ++it)
		{
			if ((*it).myItem->isUserFlagSet(UserFlagDrawable))
			{
				Drawable* drawable = (Drawable*)(*it).myItem.get();
				drawable->CheckPostDraw(state);
			}
		}

		state->GetRenderer()->popShader(this,state);
	}
}

/*
void API3DShader::ActiveSampler()
{
	auto device = RendererDX11::theGlobalRenderer->as<RendererDX11>()->getDXInstance();
	device->m_deviceContext->PSSetSamplers(0,1,&m_sampleState);
}

void API3DShader::DeactiveSampler()
{
	auto device = RendererDX11::theGlobalRenderer->as<RendererDX11>()->getDXInstance();
	device->m_deviceContext->PSSetSamplers(0, 1, nullptr);
}
*/

void API3DShader::Active(TravState* state, bool resetUniform)
{
	if (myCurrentShader)
	{
		auto device = RendererDX11::theGlobalRenderer->as<RendererDX11>()->getDXInstance();
#ifdef KIGS_TOOLS
		++gRendererStats.ShaderSwitch;
#endif
		device->m_deviceContext->VSSetShader((ID3D11VertexShader*)GetCurrentVertexShaderInfo<HLSLShaderInfo>()->internalShaderStruct, nullptr, 0);
		device->m_deviceContext->PSSetShader((ID3D11PixelShader*)(GetCurrentFragmentShaderInfo<HLSLShaderInfo>()->internalShaderStruct), nullptr, 0);

		//ActiveSampler();
		if (myUseGenericLight && state)
		{
			RendererDX11::theGlobalRenderer->as<RendererDX11>()->SendLightsInfo(state);
		}

	}
}

void API3DShader::Deactive(TravState* state)
{

	auto device = RendererDX11::theGlobalRenderer->as<RendererDX11>()->getDXInstance();
	device->m_deviceContext->VSSetShader(nullptr, nullptr, 0);
	device->m_deviceContext->PSSetShader(nullptr, nullptr, 0);

	//DeactiveSampler();

	// Set the vertex input layout.
	device->m_deviceContext->IASetInputLayout(nullptr);
}

BuildShaderStruct*	API3DShader::Rebuild()
{
	HRESULT result;
	ID3DBlob* errorMessage;
	ID3DBlob* vertexShaderBuffer = nullptr;
	ID3DBlob* pixelShaderBuffer = nullptr;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	
	int flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	const D3D_SHADER_MACRO defines[] =
	{
		NULL, NULL
	};

	std::string str;
	myVertexShaderText.getValue(str);

	std::string name = getName();
	name.append("vertex");

	// Compile the vertex shader code.
	result = D3DCompile(str.c_str(), str.length(), name.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", flags
		, 0, &vertexShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			auto ptr = errorMessage->GetBufferPointer();
			kigsprintf("%s\n", str.c_str());
			kigsprintf("%s\n", (char*)ptr);
		}

		return nullptr;
	}

	myFragmentShaderText.getValue(str);

	name = getName();
	name.append("frag");

	// Compile the pixel shader code.
	result = D3DCompile(str.c_str(), str.length(), name.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", flags
		, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			auto ptr = errorMessage->GetBufferPointer();
			kigsprintf("%s\n", str.c_str());
			kigsprintf("%s\n", (char*)ptr);
		}

		return nullptr;
	}
		
	// create shader object from bitcode
	auto device = RendererDX11::theGlobalRenderer->as<RendererDX11>()->getDXInstance();

	BuildShaderStruct* toReturn = new BuildShaderStruct();
	
	toReturn->myVertexShader = new HLSLShaderInfo();
	toReturn->myFragmentShader = new HLSLShaderInfo();

	static_cast<HLSLShaderInfo*>(toReturn->myVertexShader)->blob = vertexShaderBuffer;
	static_cast<HLSLShaderInfo*>(toReturn->myFragmentShader)->blob = pixelShaderBuffer;

	ID3D11VertexShader* m_pVertexShader;
	DX::ThrowIfFailed(device->m_device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &m_pVertexShader));
	((HLSLShaderInfo*)toReturn->myVertexShader)->internalShaderStruct = m_pVertexShader;
	D3DReflect(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), IID_ID3D11ShaderReflection, (void**) & ((HLSLShaderInfo*)toReturn->myVertexShader)->reflector);
	

	ID3D11PixelShader* m_pPixelShader;
	DX::ThrowIfFailed(device->m_device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &m_pPixelShader));
	((HLSLShaderInfo*)toReturn->myFragmentShader)->internalShaderStruct = m_pPixelShader;
	D3DReflect(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), IID_ID3D11ShaderReflection, (void**) & ((HLSLShaderInfo*)toReturn->myFragmentShader)->reflector);

	toReturn->myShaderProgram=CreateProgram();

	//toReturn->myShaderProgram->mID = 1; // say it's ok for m_layout init

	
	if (!toReturn->myUniforms)
	{
		toReturn->myUniforms = new kstl::map<UNIFORM_NAME_TYPE, UniformList*>();
	}

	return toReturn;
}
/*
ID3D11SamplerState* API3DShader::CreateSampler(D3D11_SAMPLER_DESC* desc)
{
	auto device = RendererDX11::theGlobalRenderer->as<RendererDX11>()->getDXInstance();
	// Create the texture sampler state.
	ID3D11SamplerState* sampler;
	HRESULT result = device->m_device->CreateSamplerState(desc, &sampler);
	if (FAILED(result))
	{
		return nullptr;
	}

	return sampler;
}
*/
void API3DShader::PushUniform(CoreModifiable* uni) 
{
	if (!isOKToUse())
	{
		return;
	}

	auto uniform = static_cast<API3DUniformBase*>(uni);

	auto itfind = myCurrentShader->myUniforms->find(uniform->Get_ID());
	UniformList* ul = nullptr;
	if (itfind == myCurrentShader->myUniforms->end())
	{
		auto uni_name = uniform->Get_Name();
		ul = new UniformList(uni_name);
		(*myCurrentShader->myUniforms)[uniform->Get_ID()] = ul;

		auto vs = GetCurrentVertexShaderInfo<HLSLShaderInfo>()->reflector;
		auto ps = GetCurrentFragmentShaderInfo<HLSLShaderInfo>()->reflector;

		D3D11_SHADER_INPUT_BIND_DESC sib_desc;
		HRESULT result;
		result = vs->GetResourceBindingDescByName(uni_name.c_str(), &sib_desc);
		if (!FAILED(result))
		{
			ul->Location = sib_desc.BindPoint;
		}
		result = ps->GetResourceBindingDescByName(uni_name.c_str(), &sib_desc);
		if (!FAILED(result))
		{
			ul->LocationFragment = sib_desc.BindPoint;
		}
	}

	else
	{
		ul = itfind->second;
	}

	if (ul->Location == 0xffffffff && ul->LocationFragment == 0xffffffff)
	{
		kigsprintf("Failed to bind uniform: %s\n", uniform->Get_Name().c_str());
		return;
	}

	ul->Push(uniform);
	uniform->Activate(ul);
}

void API3DShader::PopUniform(CoreModifiable* uni)
{
	if (!isOKToUse())
	{
		return;
	}

	auto uniform = static_cast<API3DUniformBase*>(uni);
	auto itfind = myCurrentShader->myUniforms->find(uniform->Get_ID());
	if (itfind == myCurrentShader->myUniforms->end()) return;
	if (itfind->second->Location == 0xffffffff) return;

	itfind->second->Pop();

	API3DUniformBase* un = itfind->second->Back();
	if (un)
	{
		un->Activate(itfind->second);
	}

}