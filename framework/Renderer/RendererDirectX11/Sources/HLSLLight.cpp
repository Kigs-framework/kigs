#include "HLSLLight.h"
#include "RendererDX11.h"
#include "ModuleSceneGraph.h"
#include "Camera.h"
#include "Node3D.h"
#include "TravState.h"
#include "ModuleRenderer.h"

using namespace Kigs::Draw;

IMPLEMENT_CLASS_INFO(API3DLight)

API3DLight::API3DLight(const std::string& name, CLASS_NAME_TREE_ARG)
	: Light(name, PASS_CLASS_NAME_TREE_ARG)
{
	
}

void	API3DLight::InitModifiable()
{
	int lLightType = GetTypeOfLight();
	
	Light::InitModifiable();

	setOwnerNotification("SpecularColor", true);
	setOwnerNotification("DiffuseColor", true);
	setOwnerNotification("AmbientColor", true);
	setOwnerNotification("SpotAttenuation", true);
	setOwnerNotification("SpotCutOff", true);
	setOwnerNotification("ConstAttenuation", true);
	setOwnerNotification("LinAttenuation", true);
	setOwnerNotification("QuadAttenuation", true);

}


API3DLight::~API3DLight()
{
	// notify scenegraph that I am dead
	ModuleSceneGraph* scenegraph = (ModuleSceneGraph*)KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex);
	scenegraph->NotifyDefferedItemDeath(this);
}

void API3DLight::NotifyUpdate(const unsigned int  labelid)
{
	
}

bool API3DLight::PreRendering(RendererDX11 * renderer, Camera * cam, v3f & camPos)
{
	if (!mIsOn)
		return false;

	// init blend mode
	renderer->SetAlphaTestMode(RENDERER_ALPHA_TEST_ON);

	return true;
}

void API3DLight::PrepareLightInfo(LightStruct& light_data, Camera* cam)
{
	if (!mIsOn)
		return;

	v3f outP;
	v3f outV;
	SetupNodeIfNeeded();
	const mat4& lMat = GetLocalToGlobal();
	v3f PosOffset(0, 0, 0);
	transformPoint3(lMat ,PosOffset, outP);

	switch (GetTypeOfLight())
	{
	case POINT_LIGHT:
	{
		PointLight light;
		light.position = v4f(outP,1.0f);
		light.ambient = v4f(mAmbientColor,0.0);
		light.diffuse = v4f(mDiffuseColor, 0.0);
		light.specular = v4f(mSpecularColor, 0.0);
		light.attenuation = v4f(mConstAttenuation, mLinAttenuation, mQuadAttenuation,0.0);
		light_data.pointlights.push_back(light);
		break;
	}
	case DIRECTIONAL_LIGHT:
	{
		DirLight light;
		light.position = v4f(normalize(outP),1.0f);
		light.ambient = v4f(mAmbientColor, 0.0);
		light.diffuse = v4f(mDiffuseColor, 0.0);
		light.specular = v4f(mSpecularColor, 0.0);
		light_data.dirlights.push_back(light);
		break;
	}
	case SPOT_LIGHT:
	{
		SpotLight light;
		light.position = v4f(outP, 1.0f);
		light.ambient = v4f(mAmbientColor, 0.0);
		light.diffuse = v4f(mDiffuseColor, 0.0);
		light.specular = v4f(mSpecularColor, 0.0);
		light.attenuationAndSpotExponent = v4f(mConstAttenuation, mLinAttenuation, mQuadAttenuation, mSpotAttenuation);
		v3f dir(0.0f, 0.0f, -1.0f);
		transformVector3(lMat,dir, outV);
		light.directionAndCutOff = v4f(outV, cosf(mSpotCutOff));// send cos directly
		light_data.spotlights.push_back(light);
		break;
	}
	}
}

//#define SHOW_DEBUG
#ifdef SHOW_DEBUG
#include "HLSLDebugDraw.h"
#endif

bool	API3DLight::Draw(TravState* state)
{
	if (!mIsOn)
		return true;


#ifdef SHOW_DEBUG
	switch (GetTypeOfLight())
	{
	case 0:
	case 1:
	{
		v3f outP;
		const mat4& lMat = GetFather()->GetLocalToGlobal();
		v3f* PosOffset = (v3f*)myPosOffset.getVector();
		lMat.TransformPoint(PosOffset, &outP);
		dd::sphere(outP, v3f(1, 0, 0), 0.05);
	}
	break;
	case 2:
	{
		v3f outP;
		v3f outDir;
		const mat4& lMat = GetFather()->GetLocalToGlobal();
		v3f *PosOffset = (v3f*)myPosOffset.getVector();
		lMat.TransformPoint(PosOffset, &outP);

		v3f *mDir = (v3f*)mySpotDirection.getVector();
		lMat.TransformVector(mDir, &outDir);
		//dd::cone(outP, outDir*10, v3f(1, 0, 0),((1-mySpotCutOff)/PI)*100, 0);
		dd::sphere(outP, v3f(1, 0, 0), 0.05);
		dd::line(outP, outP + outDir, v3f(1, 0, 0));
	}
	break;
	}

#endif


	return true;
}

void API3DLight::SetUniformLocation(int uniform, const char* location)
{
}

void API3DLight::DrawLight(TravState* travstate)
{
}

void API3DLight::PostDrawLight(TravState* travstate)
{
}

int API3DLight::GetTypeOfLight()
{
	return mLightType;
}