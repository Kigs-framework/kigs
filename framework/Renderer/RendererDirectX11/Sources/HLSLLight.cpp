#include "HLSLLight.h"
#include "RendererDX11.h"
#include "ModuleSceneGraph.h"
#include "Camera.h"
#include "Node3D.h"
#include "TravState.h"
#include "ModuleRenderer.h"


IMPLEMENT_CLASS_INFO(API3DLight)

API3DLight::API3DLight(const kstl::string& name, CLASS_NAME_TREE_ARG)
	: Light(name, PASS_CLASS_NAME_TREE_ARG)
{
	
}

void	API3DLight::InitModifiable()
{
	int lLightType = GetTypeOfLight();
	
	Light::InitModifiable();

	mySpecularColor.changeNotificationLevel(Owner);
	myDiffuseColor.changeNotificationLevel(Owner);
	myAmbientColor.changeNotificationLevel(Owner);
	mySpotAttenuation.changeNotificationLevel(Owner);
	mySpotCutOff.changeNotificationLevel(Owner);
	myConstAttenuation.changeNotificationLevel(Owner);
	myLinAttenuation.changeNotificationLevel(Owner);
	myQuadAttenuation.changeNotificationLevel(Owner);
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

bool API3DLight::PreRendering(RendererDX11 * renderer, Camera * cam, Point3D & camPos)
{
	if (!myIsOn)
		return false;

	// init blend mode
	renderer->SetAlphaTestMode(RENDERER_ALPHA_TEST_ON);

	return true;
}

void API3DLight::PrepareLightInfo(LightStruct& light_data, Camera* cam)
{
	if (!myIsOn)
		return;

	Point3D outP;
	Vector3D outV;
	SetupNodeIfNeeded();
	const Matrix3x4& lMat = GetLocalToGlobal();
	Point3D PosOffset(0, 0, 0);
	lMat.TransformPoint(&PosOffset, &outP);

	switch (GetTypeOfLight())
	{
	case POINT_LIGHT:
	{
		PointLight light;
		light.position.xyz = outP;
		light.ambient.xyz = (v3f)myAmbientColor;
		light.diffuse.xyz = (v3f)myDiffuseColor;
		light.specular.xyz = (v3f)mySpecularColor;
		light.attenuation.xyz = v3f(myConstAttenuation, myLinAttenuation, myQuadAttenuation);
		light_data.pointlights.push_back(light);
		break;
	}
	case DIRECTIONAL_LIGHT:
	{
		DirLight light;
		light.position.xyz = outP.Normalized();
		light.ambient.xyz = (v3f)myAmbientColor;
		light.diffuse.xyz = (v3f)myDiffuseColor;
		light.specular.xyz = (v3f)mySpecularColor;
		light_data.dirlights.push_back(light);
		break;
	}
	case SPOT_LIGHT:
	{
		SpotLight light;
		light.position.xyz = outP;
		light.ambient.xyz = (v3f)myAmbientColor;
		light.diffuse.xyz = (v3f)myDiffuseColor;
		light.specular.xyz = (v3f)mySpecularColor;
		light.attenuationAndSpotExponent = v4f(myConstAttenuation, myLinAttenuation, myQuadAttenuation, mySpotAttenuation);
		Vector3D dir(0.0f, 0.0f, -1.0f);
		lMat.TransformVector((Vector3D*)&dir.x, &outV);
		light.directionAndCutOff.xyz = outV;
		light.directionAndCutOff.w = cosf(mySpotCutOff); // send cos directly
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
	if (!myIsOn)
		return true;


#ifdef SHOW_DEBUG
	switch (GetTypeOfLight())
	{
	case 0:
	case 1:
	{
		Point3D outP;
		const Matrix3x4& lMat = GetFather()->GetLocalToGlobal();
		Point3D* PosOffset = (Point3D*)myPosOffset.getVector();
		lMat.TransformPoint(PosOffset, &outP);
		dd::sphere(outP, Vector3D(1, 0, 0), 0.05);
	}
	break;
	case 2:
	{
		Point3D outP;
		Vector3D outDir;
		const Matrix3x4& lMat = GetFather()->GetLocalToGlobal();
		Point3D *PosOffset = (Point3D*)myPosOffset.getVector();
		lMat.TransformPoint(PosOffset, &outP);

		Vector3D *dir = (Vector3D*)mySpotDirection.getVector();
		lMat.TransformVector(dir, &outDir);
		//dd::cone(outP, outDir*10, Vector3D(1, 0, 0),((1-mySpotCutOff)/PI)*100, 0);
		dd::sphere(outP, Vector3D(1, 0, 0), 0.05);
		dd::line(outP, outP + outDir, Vector3D(1, 0, 0));
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
	return myLightType;
}