#include "PrecompiledHeaders.h"

#include "math.h"
#include "ModuleRenderer.h"
#include "OpenGLMaterial.h"
#include "GLSLShader.h"
#include "Texture.h"
#include "MaterialStage.h"

#include "TravState.h"
#include "RendererOpenGL.h"

// include after all other kigs include (for iOS)
#include "Platform/Renderer/OpenGLInclude.h"

using namespace Kigs::Draw;

IMPLEMENT_CLASS_INFO(OpenGLMaterial)

void OpenGLMaterial::InitModifiable()
{
	Material::InitModifiable();

	if (IsInit())
	{
		mUniDiffuseColor = KigsCore::GetInstanceOf("uniformColor", "API3DUniformFloat4");
		mUniDiffuseColor->setValue("Name", "MaterialDiffuseColor");
		mUniDiffuseColor->setArrayValue("Value", mDiffuseColor[0], mDiffuseColor[1], mDiffuseColor[2], mDiffuseColor[3]);
		mUniDiffuseColor->Init();
		addItem(mUniDiffuseColor);

		mUniSpecularColor = KigsCore::GetInstanceOf("uniSpecularColor", "API3DUniformFloat4");
		mUniSpecularColor->setValue("Name", "MaterialSpecularColor");
		mUniSpecularColor->setArrayValue("Value", mSpecularColor[0], mSpecularColor[1], mSpecularColor[2], mSpecularColor[3]);
		mUniSpecularColor->Init();
		addItem(mUniSpecularColor);

		mUniShininess = KigsCore::GetInstanceOf("shininessUniform", "API3DUniformFloat");
		mUniShininess->setValue("Name", "MaterialShininess");
		mUniShininess->setValue("Value", mShininess);
		mUniShininess->Init();
		addItem(mUniShininess);

		mUniAmbiantColor = KigsCore::GetInstanceOf("uniAmbiantColor", "API3DUniformFloat4");
		mUniAmbiantColor->setValue("Name", "MaterialAmbiantColor");
		mUniAmbiantColor->setArrayValue("Value", mAmbientColor[0], mAmbientColor[1], mAmbientColor[2], mAmbientColor[3]);
		mUniAmbiantColor->Init();
		addItem(mUniAmbiantColor);

#ifdef KIGS_TOOLS
		mUniDiffuseColor->AddDynamicAttribute(ATTRIBUTE_TYPE::BOOL, "NoExport", true);
		mUniSpecularColor->AddDynamicAttribute(ATTRIBUTE_TYPE::BOOL, "NoExport", true);
		mUniShininess->AddDynamicAttribute(ATTRIBUTE_TYPE::BOOL, "NoExport", true);
		mUniAmbiantColor->AddDynamicAttribute(ATTRIBUTE_TYPE::BOOL, "NoExport", true);
#endif

		mDiffuseColor.changeNotificationLevel(Owner);
		mAmbientColor.changeNotificationLevel(Owner);
		mShininess.changeNotificationLevel(Owner);
		mSpecularColor.changeNotificationLevel(Owner);
	}
}

void OpenGLMaterial::NotifyUpdate(unsigned int labelid)
{
	ParentClassType::NotifyUpdate(labelid);
	if (labelid == mDiffuseColor.getID())
	{
		mUniDiffuseColor->setArrayValue("Value", mDiffuseColor[0], mDiffuseColor[1], mDiffuseColor[2], mDiffuseColor[3]);
	}
	else if (labelid == mAmbientColor.getID())
	{
		mUniAmbiantColor->setArrayValue("Value", mAmbientColor[0], mAmbientColor[1], mAmbientColor[2], mAmbientColor[3]);
	}
	else if (labelid == mShininess.getID())
	{
		mUniShininess->setValue("Value", mShininess);
	}
	else if (labelid == mSpecularColor.getID())
	{
		mUniSpecularColor->setArrayValue("Value", mSpecularColor[0], mSpecularColor[1], mSpecularColor[2], mSpecularColor[3]);
	}
}

bool	OpenGLMaterial::PreDraw(TravState* travstate)
{
	if (Material::PreDraw(travstate))
	{
		RendererOpenGL* renderer = (RendererOpenGL*)travstate->GetRenderer();
		renderer->SetColorMaterial(RENDERER_FRONT_AND_BACK, RENDERER_AMBIENT_AND_DIFFUSE);			
		renderer->SetColorMaterialMode(mMaterialColorEnabled);

		if (mBlendEnabled || (mDiffuseColor[3] < 1.0f))// || m_IsTransparent)
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

bool	OpenGLMaterial::PostDraw(TravState* travstate)
{
	if (Material::PostDraw(travstate))
	{
		RendererOpenGL* renderer = (RendererOpenGL*)travstate->GetRenderer();

		//renderer->SetDepthMaskMode(RENDERER_DEPTH_MASK_ON);
		renderer->SetBlendMode(RENDERER_BLEND_OFF);
		return true;
	}
	return false;
}

