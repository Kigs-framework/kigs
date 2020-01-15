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

IMPLEMENT_CLASS_INFO(OpenGLMaterial)

void OpenGLMaterial::InitModifiable()
{
	Material::InitModifiable();

	if (IsInit())
	{
		uniDiffuseColor = KigsCore::GetInstanceOf("uniformColor", "API3DUniformFloat4");
		uniDiffuseColor->setValue("Name", "MaterialDiffuseColor");
		uniDiffuseColor->setArrayValue("Value", myDiffuseColor[0], myDiffuseColor[1], myDiffuseColor[2], myDiffuseColor[3]);
		uniDiffuseColor->Init();
		addItem((CMSP&)uniDiffuseColor);

		uniSpecularColor = KigsCore::GetInstanceOf("uniSpecularColor", "API3DUniformFloat4");
		uniSpecularColor->setValue("Name", "MaterialSpecularColor");
		uniSpecularColor->setArrayValue("Value", mySpecularColor[0], mySpecularColor[1], mySpecularColor[2], mySpecularColor[3]);
		uniSpecularColor->Init();
		addItem((CMSP&)uniSpecularColor);

		uniShininess = KigsCore::GetInstanceOf("shininessUniform", "API3DUniformFloat");
		uniShininess->setValue("Name", "MaterialShininess");
		uniShininess->setValue("Value", myShininess);
		uniShininess->Init();
		addItem((CMSP&)uniShininess);

		uniAmbiantColor = KigsCore::GetInstanceOf("uniAmbiantColor", "API3DUniformFloat4");
		uniAmbiantColor->setValue("Name", "MaterialAmbiantColor");
		uniAmbiantColor->setArrayValue("Value", myAmbientColor[0], myAmbientColor[1], myAmbientColor[2], myAmbientColor[3]);
		uniAmbiantColor->Init();
		addItem((CMSP&)uniAmbiantColor);

#ifdef KIGS_TOOLS
		uniDiffuseColor->AddDynamicAttribute(BOOL, "NoExport", true);
		uniSpecularColor->AddDynamicAttribute(BOOL, "NoExport", true);
		uniShininess->AddDynamicAttribute(BOOL, "NoExport", true);
		uniAmbiantColor->AddDynamicAttribute(BOOL, "NoExport", true);
#endif

		myDiffuseColor.changeNotificationLevel(Owner);
		myAmbientColor.changeNotificationLevel(Owner);
		myShininess.changeNotificationLevel(Owner);
		mySpecularColor.changeNotificationLevel(Owner);
	}
}

void OpenGLMaterial::NotifyUpdate(unsigned int labelid)
{
	ParentClassType::NotifyUpdate(labelid);
	if (labelid == myDiffuseColor.getID())
	{
		uniDiffuseColor->setArrayValue("Value", myDiffuseColor[0], myDiffuseColor[1], myDiffuseColor[2], myDiffuseColor[3]);
	}
	else if (labelid == myAmbientColor.getID())
	{
		uniAmbiantColor->setArrayValue("Value", myAmbientColor[0], myAmbientColor[1], myAmbientColor[2], myAmbientColor[3]);
	}
	else if (labelid == myShininess.getID())
	{
		uniShininess->setValue("Value", myShininess);
	}
	else if (labelid == mySpecularColor.getID())
	{
		uniSpecularColor->setArrayValue("Value", mySpecularColor[0], mySpecularColor[1], mySpecularColor[2], mySpecularColor[3]);
	}
}

bool	OpenGLMaterial::PreDraw(TravState* travstate)
{
	if (Material::PreDraw(travstate))
	{
		RendererOpenGL* renderer = (RendererOpenGL*)travstate->GetRenderer();
		renderer->SetColorMaterial(RENDERER_FRONT_AND_BACK, RENDERER_AMBIENT_AND_DIFFUSE);			
		renderer->SetColorMaterialMode(myMaterialColorEnabled);

		/*if (myDiffuseColor[3] != 1.0 || m_IsTransparent)
		{
			renderer->SetDepthMaskMode(RENDERER_DEPTH_MASK_OFF);
		}*/

		if (myBlendEnabled || (myDiffuseColor[3] < 1.0f))// || m_IsTransparent)
		{
			RendererBlendFuncMode sourceType;
			RendererBlendFuncMode destType;

			if (myBlendEnabled)
			{
				switch ((int)myBlendFuncSource)
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

				switch ((int)myBlendFuncDest)
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

