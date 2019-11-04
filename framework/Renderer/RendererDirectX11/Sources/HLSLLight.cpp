#include "HLSLLight.h"
#include "RendererDX11.h"
#include "ModuleSceneGraph.h"
#include "Camera.h"
#include "Node3D.h"
#include "TravState.h"
#include "ModuleRenderer.h"


IMPLEMENT_CLASS_INFO(API3DLight)

API3DLight::API3DLight(const kstl::string& name, CLASS_NAME_TREE_ARG)
	: API3DShader(name, PASS_CLASS_NAME_TREE_ARG)
	, myIsOn(*this, false, LABEL_AND_ID(IsOn), true)
	, myPosOffset(*this, false, LABEL_AND_ID(Offset), 0, 0, 0)
	, myDiffuseColor(*this, false, LABEL_AND_ID(DiffuseColor), 1, 1, 1)
	, myAmbiantColor(*this, false, LABEL_AND_ID(AmbiantColor), 0, 0, 0)
	, mySpecularColor(*this, false, LABEL_AND_ID(SpecularColor), 1, 1, 1)
	, myConstAttenuation(*this, false, LABEL_AND_ID(ConstAttenuation), KFLOAT_CONST(0.0f))
	, myLinAttenuation(*this, false, LABEL_AND_ID(LinAttenuation), KFLOAT_CONST(0.0f))
	, myQuadAttenuation(*this, false, LABEL_AND_ID(QuadAttenuation), KFLOAT_CONST(0.0f))
	, mySpotDirection(*this, false, LABEL_AND_ID(SpotDirection), 0, 0, 0)
	, mySpotCutOff(*this, false, LABEL_AND_ID(SpotCutOff), KFLOAT_CONST(0.0f))	
	, mySpotAttenuation(*this, false, LABEL_AND_ID(SpotAttenuation), KFLOAT_CONST(1.0f))
	, myIsDirectional(*this, false, LABEL_AND_ID(IsDirectional), false)
	, myDefferedLight(*this, false, LABEL_AND_ID(DefferedLight), false)
	, myPriority(*this, false, LABEL_AND_ID(Priority), 0)
	, myLightType(*this, false, LABEL_AND_ID(LightType), "POINT", "DIRECTIONAL", "SPOT", "CUSTOM") 	//0 for point, 1 for directional, 2 for spot, 3 for customShader
	, myPositionNode3D(*this, false, LABEL_AND_ID(PositionNode3D))
	, myScene(NULL)
	, myParentNode(NULL)
	, myListUpdateFrame(0)
{
	CONSTRUCT_METHOD(API3DLight, SetDiffuseColor);
	CONSTRUCT_METHOD(API3DLight, SetSpecularColor);
#ifdef USE_ATTFUNCTION
	CONSTRUCT_METHOD(API3DLight, SetAttenuation);
	CONSTRUCT_METHOD(API3DLight, GetAttenuation);
#endif
}

void	API3DLight::InitModifiable()
{
	if (!GetFather())
	{
		UninitModifiable();
		return;
	}

	int lLightType = GetTypeOfLight();
	
	API3DShader::InitModifiable();

	mySpecularColor.changeNotificationLevel(Owner);
	myDiffuseColor.changeNotificationLevel(Owner);
	mySpotDirection.changeNotificationLevel(Owner);
	mySpotAttenuation.changeNotificationLevel(Owner);
	mySpotCutOff.changeNotificationLevel(Owner);
	myIsOn.changeNotificationLevel(Owner);
	myIsDirectional.changeNotificationLevel(Owner);
	myConstAttenuation.changeNotificationLevel(Owner);
	myLinAttenuation.changeNotificationLevel(Owner);
	myQuadAttenuation.changeNotificationLevel(Owner);
}

Node3D* API3DLight::GetFather()
{
	if (myParentNode == NULL)
	{
		const kstl::vector<CoreModifiable*>& instances = GetParents();
		kstl::vector<CoreModifiable*>::const_iterator itr = instances.begin();
		kstl::vector<CoreModifiable*>::const_iterator end = instances.end();
		for (; itr != end; ++itr)
		{
			if ((*itr)->isSubType(Node3D::myClassID))
			{
				myParentNode = static_cast<Node3D*>(*itr);
				break;
			}
		}
	}

	return myParentNode;
}

API3DLight::~API3DLight()
{
	// notify scenegraph that I am dead
	ModuleSceneGraph* scenegraph = (ModuleSceneGraph*)KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex);
	scenegraph->NotifyDefferedItemDeath(this);
}

void API3DLight::NotifyUpdate(const unsigned int  labelid)
{
	if (labelid == myIsOn.getLabelID())
	{
		if (myScene)
		{
			if (myIsOn)
				myScene->addLight(this);
			else
				myScene->removeLight(this);
		}
	}
}

bool API3DLight::PreRendering(RendererDX11 * renderer, Camera * cam, Point3D & camPos)
{
	if (!myIsOn)
		return false;

	if (((CoreModifiable*)myAttachedCamera) && cam != (CoreModifiable*)myAttachedCamera)
		return false;

	// init blend mode
	renderer->SetAlphaTestMode(RENDERER_ALPHA_TEST_ON);

	return true;
}

void API3DLight::PrepareLightInfo(LightStruct& light_data, Camera* cam)
{
	if (!myIsOn || myDefferedLight)
		return;

	if (((CoreModifiable*)myAttachedCamera) && cam != (CoreModifiable*)myAttachedCamera)
		return;

	v3f pos = GetFather()->GetLocalToGlobal() * myPosOffset;

	switch (GetTypeOfLight())
	{
	case POINT_LIGHT:
	{
		PointLight light;
		light.position.xyz = pos;
		light.ambiant.xyz = (v3f)myAmbiantColor;
		light.diffuse.xyz = (v3f)myDiffuseColor;
		light.specular.xyz = (v3f)mySpecularColor;
		light.attenuation.xyz = v3f(myConstAttenuation, myLinAttenuation, myQuadAttenuation);
		light_data.pointlights.push_back(light);
		break;
	}
	case DIRECTIONAL_LIGHT:
	{
		DirLight light;
		light.position.xyz = pos.Normalized();
		light.ambiant.xyz = (v3f)myAmbiantColor;
		light.diffuse.xyz = (v3f)myDiffuseColor;
		light.specular.xyz = (v3f)mySpecularColor;
		light_data.dirlights.push_back(light);
		break;
	}
	case SPOT_LIGHT:
	{
		SpotLight light;
		light.position.xyz = pos;
		light.ambiant.xyz = (v3f)myAmbiantColor;
		light.diffuse.xyz = (v3f)myDiffuseColor;
		light.specular.xyz = (v3f)mySpecularColor;
		light.attenuationAndSpotExponent = v4f(myConstAttenuation, myLinAttenuation, myQuadAttenuation, mySpotAttenuation);
		light.directionAndCutOff.xyz = mySpotDirection;
		light.directionAndCutOff.w = mySpotCutOff;
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
	if (myLightType != 3)
	{
		return myLightType;
	}
	else
	{
		if (myIsDirectional)
		{
			return DIRECTIONAL_LIGHT;
		}
		else
		{
			if (mySpotDirection[0] == 0 && mySpotDirection[1] == 0 && mySpotDirection[2] == 0)
			{
				return POINT_LIGHT;
			}
			else
			{
				return SPOT_LIGHT;
			}
		}
	}
}