
#include "DX11Material.h"
#include "RendererDX11.h"
//#include "DX11MaterialStage.h"

#include "Shader.h"
#include "Texture.h"
#include "MaterialStage.h"
#include "TravState.h"

#include "Crc32.h"

#include "math.h"

IMPLEMENT_CLASS_INFO(DX11Material)



DX11Material::~DX11Material()
{
//	if (mMaterialBuffer) mMaterialBuffer->Release();
}

void DX11Material::NotifyUpdate(unsigned int labelid)
{
	ParentClassType::NotifyUpdate(labelid);
}

bool	DX11Material::PreDraw(TravState* travstate)
{
	if (Material::PreDraw(travstate))
	{
		RendererDX11* renderer = (RendererDX11*)travstate->GetRenderer();
		
		renderer->SetColorMaterial(RENDERER_FRONT_AND_BACK, RENDERER_AMBIENT_AND_DIFFUSE);			
		renderer->SetColorMaterialMode(mMaterialColorEnabled);
		
		auto dxinstance = renderer->getDXInstance();
		
		MaterialStruct data;
		data.MaterialAmbiantColor = mAmbientColor;
		data.MaterialDiffuseColor = mDiffuseColor;
		data.MaterialSpecularColor = mSpecularColor;
		data.MaterialShininess = mShininess;
		//size_t hash = 0;
		//hash_combine(hash, V4F_EXP(data.MaterialAmbiantColor), V4F_EXP(data.MaterialDiffuseColor), V4F_EXP(data.MaterialSpecularColor), data.MaterialShininess);

		if (!dxinstance->mMaterialBuffer)
		{
			D3D11_BUFFER_DESC materialBufferDesc;
			materialBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			materialBufferDesc.ByteWidth = sizeof(MaterialStruct);
			if (materialBufferDesc.ByteWidth % 16 != 0) materialBufferDesc.ByteWidth = (materialBufferDesc.ByteWidth / 16 + 1) * 16;

			materialBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			materialBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			materialBufferDesc.MiscFlags = 0;
			materialBufferDesc.StructureByteStride = 0;
			HRESULT result = dxinstance->mDevice->CreateBuffer(&materialBufferDesc, NULL, &dxinstance->mMaterialBuffer);
		}

		if (!(dxinstance->mCurrentMaterial == data))
		{
			dxinstance->mCurrentMaterial = data;
			HRESULT result;
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			result = dxinstance->mDeviceContext->Map(dxinstance->mMaterialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			*(MaterialStruct*)mappedResource.pData = data;
			dxinstance->mDeviceContext->Unmap(dxinstance->mMaterialBuffer, 0);
		}
		dxinstance->mDeviceContext->PSSetConstantBuffers(DX11_MATERIAL_SLOT, 1, &dxinstance->mMaterialBuffer);

		/*if (mDiffuseColor[3] != 1.0 || m_IsTransparent)
		{
			renderer->SetDepthMaskMode(false);
		}*/

		if (mBlendEnabled || (mDiffuseColor[3] < 1.0f)) // || m_IsTransparent)
		{
			RendererBlendFuncMode sourceType;
			RendererBlendFuncMode destType;

			if (mBlendEnabled)
			{
				switch ((int)mBlendFuncSource)
				{
				case	S_ZERO:
				{
					sourceType = RENDERER_BLEND_ZERO;
					break;
				}
				case	S_ONE:
				{
					sourceType = RENDERER_BLEND_ONE;
					break;
				}
				case	S_DST_COLOR:
				{
					sourceType = RENDERER_BLEND_DST_COLOR;
					break;
				}
				case	S_ONE_MINUS_DST_COLOR:
				{
					sourceType = RENDERER_BLEND_ONE_MINUS_DST_COLOR;
					break;
				}
				case	S_SRC_ALPHA:
				{
					sourceType = RENDERER_BLEND_SRC_ALPHA;
					break;
				}
				case	S_ONE_MINUS_SRC_ALPHA:
				{
					sourceType = RENDERER_BLEND_ONE_MINUS_SRC_ALPHA;
					break;
				}
				case	S_DST_ALPHA:
				{
					sourceType = RENDERER_BLEND_DST_ALPHA;
					break;
				}
				case	S_ONE_MINUS_DST_ALPHA:
				{
					sourceType = RENDERER_BLEND_ONE_MINUS_DST_ALPHA;
					break;
				}
				case	S_SRC_ALPHA_SATURATE:
				{
					sourceType = RENDERER_BLEND_GL_SRC_ALPHA_SATURATE;
					break;
				}
				//default:
				//	KIGS_ERROR("Blend Enabled but no source func defined", 3);
				}

				switch ((int)mBlendFuncDest)
				{
				case	D_ZERO:
				{
					destType = RENDERER_BLEND_ZERO;
					break;
				}
				case	D_ONE:
				{
					destType = RENDERER_BLEND_ONE;
					break;
				}
				case	D_SRC_COLOR:
				{
					destType = RENDERER_BLEND_SRC_COLOR;
					break;
				}
				case	D_ONE_MINUS_SRC_COLOR:
				{
					destType = RENDERER_BLEND_ONE_MINUS_SRC_COLOR;
					break;
				}
				case	D_SRC_ALPHA:
				{
					destType = RENDERER_BLEND_SRC_ALPHA;
					break;
				}
				case	D_ONE_MINUS_SRC_ALPHA:
				{
					destType = RENDERER_BLEND_ONE_MINUS_SRC_ALPHA;
					break;
				}
				case	D_DST_ALPHA:
				{
					destType = RENDERER_BLEND_DST_ALPHA;
					break;
				}
				case	D_ONE_MINUS_DST_ALPHA:
				{
					destType = RENDERER_BLEND_ONE_MINUS_DST_ALPHA;
					break;
				}
				//default:
				//	KIGS_ERROR("Blend Enabled but no destination func defined", 3);
				}
			}
			else
			{
				sourceType = RENDERER_BLEND_SRC_ALPHA;
				destType = RENDERER_BLEND_ONE_MINUS_SRC_ALPHA;
			}

			renderer->SetBlendMode(RENDERER_BLEND_ON);
			renderer->SetBlendFuncMode(sourceType, destType);
		}
		else
		{
			renderer->SetBlendMode(RENDERER_BLEND_OFF);
		}

		return true;
	}
	return false;
}

bool	DX11Material::PostDraw(TravState* travstate)
{
	if (Material::PostDraw(travstate))
	{
		RendererDX11* renderer = (RendererDX11*)travstate->GetRenderer();

		//renderer->SetDepthMaskMode(true);
		renderer->SetBlendMode(RENDERER_BLEND_OFF);
		return true;
	}
	return false;
}

