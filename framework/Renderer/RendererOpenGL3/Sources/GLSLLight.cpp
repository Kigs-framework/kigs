#include "PrecompiledHeaders.h"
#include "GLSLLight.h"
#include "RendererOpenGL.h"
#include "ModuleSceneGraph.h"
#include "Camera.h"
#include "GLSLUniform.h"
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

	, myPositionUniform(NULL)
	, myCamPosUniform(NULL)

	, myDiffuseUniform(NULL)
	, mySpecularUniform(NULL)
	, myAmbiantUniform(NULL)

	, myAttenuationUniform(NULL)
	, mySpotDirUniform(NULL)
	, mySpotCutoffUniform(NULL)
	, mySpotExponentUniform(NULL)
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

	//No deferred with GLES2
/*#ifndef GL_ES2
	if (myFragmentShaderText.const_ref() == "")
	{
		switch (lLightType)
		{
		case POINT_LIGHT:
			myFragmentShaderText = PointLightShaderFrag;
			myVertexShaderText = ShaderVert;
			break;
		case DIRECTIONAL_LIGHT:
			myFragmentShaderText = DirectionnalLightShaderFrag;
			myVertexShaderText = ShaderVert;
			break;
		case SPOT_LIGHT:
			myFragmentShaderText = SpotLightShaderFrag;
			myVertexShaderText = ShaderVert;
			break;

		default:
			break;
		}
	}
#endif */

	myPositionUniform = KigsCore::GetInstanceOf(getName() + "PositionUniform", "API3DUniformFloat3");
	myPositionUniform->setValue(LABEL_TO_ID(Name), "lightPos");
	myPositionUniform->Init();

	myCamPosUniform = KigsCore::GetInstanceOf(getName() + "CamPosUniform", "API3DUniformFloat3");
	myCamPosUniform->setValue(LABEL_TO_ID(Name), "camPos");
	myCamPosUniform->Init();

	myDiffuseUniform = KigsCore::GetInstanceOf(getName() + "DiffuseUniform", "API3DUniformFloat3");
	myDiffuseUniform->setValue(LABEL_TO_ID(Name), "light_diffuse");
	myDiffuseUniform->setArrayValue(LABEL_TO_ID(Value), myDiffuseColor[0], myDiffuseColor[1], myDiffuseColor[2]);
	myDiffuseUniform->Init();

	mySpecularUniform = KigsCore::GetInstanceOf(getName() + "SpecularUniform", "API3DUniformFloat3");
	mySpecularUniform->setValue(LABEL_TO_ID(Name), "light_specular");
	mySpecularUniform->setArrayValue(LABEL_TO_ID(Value), mySpecularColor[0], mySpecularColor[1], mySpecularColor[2]);
	mySpecularUniform->Init();

	myAmbiantUniform = KigsCore::GetInstanceOf(getName() + "AmbiantUniform", "API3DUniformFloat3");
	myAmbiantUniform->setValue(LABEL_TO_ID(Name), "light_ambiant");
	myAmbiantUniform->setArrayValue(LABEL_TO_ID(Value), myAmbiantColor[0], myAmbiantColor[1], myAmbiantColor[2]);
	myAmbiantUniform->Init();

	// create shader uniform
	if (lLightType == SPOT_LIGHT)
	{
		mySpotDirUniform = KigsCore::GetInstanceOf(getName() + "SpotDirUniform", "API3DUniformFloat3");
		mySpotDirUniform->setValue(LABEL_TO_ID(Name), "spotDir");
		mySpotDirUniform->setArrayValue(LABEL_TO_ID(Value), mySpotDirection[0], mySpotDirection[1], mySpotDirection[2]);
		mySpotDirUniform->setValue(LABEL_TO_ID(Normalize), true);
		mySpotDirUniform->Init();

		mySpotCutoffUniform = KigsCore::GetInstanceOf(getName() + "SpotCutoffUniform", "API3DUniformFloat");
		mySpotCutoffUniform->setValue(LABEL_TO_ID(Name), "spotCutoff");
		mySpotCutoffUniform->setValue(LABEL_TO_ID(Value), cosf(mySpotCutOff.const_ref())); // send directly the cos, so don't have to do it at each fragment
		mySpotCutoffUniform->Init();


		mySpotExponentUniform = KigsCore::GetInstanceOf(getName() + "SpotExponentUniform", "API3DUniformFloat");
		mySpotExponentUniform->setValue(LABEL_TO_ID(Name), "spotExponent");
		mySpotExponentUniform->setValue(LABEL_TO_ID(Value), mySpotAttenuation);
		mySpotExponentUniform->Init();
	}

	if (lLightType != DIRECTIONAL_LIGHT)
	{
		myAttenuationUniform = KigsCore::GetInstanceOf(getName() + "AttenuationUniform", "API3DUniformFloat3");
		myAttenuationUniform->setValue(LABEL_TO_ID(Name), "attenuation");
		myAttenuationUniform->setArrayValue(LABEL_TO_ID(Value), myConstAttenuation, myLinAttenuation, myQuadAttenuation);
		myAttenuationUniform->Init();
	}

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
	

	myPositionUniform = myCamPosUniform = mySpotCutoffUniform = mySpotExponentUniform = myDiffuseUniform = mySpecularUniform = myAmbiantUniform = myAttenuationUniform = mySpotDirUniform = nullptr;

	// notify scenegraph that I am dead
	ModuleSceneGraph* scenegraph = (ModuleSceneGraph*)KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex);
	scenegraph->NotifyDefferedItemDeath(this);
}

void API3DLight::NotifyUpdate(const unsigned int  labelid)
{
	if (labelid == mySpecularColor.getLabelID())
	{
		if (mySpecularUniform)
			mySpecularUniform->setArrayValue("Value", mySpecularColor[0], mySpecularColor[1], mySpecularColor[2]);
	}
	else if (labelid == myAmbiantColor.getLabelID())
	{
		if (myAmbiantUniform)
			myAmbiantUniform->setArrayValue("Value", myAmbiantColor[0], myAmbiantColor[1], myAmbiantColor[2]);
	}
	else if (labelid == myDiffuseColor.getLabelID())
	{
		if (myDiffuseUniform)
			myDiffuseUniform->setArrayValue("Value", myDiffuseColor[0], myDiffuseColor[1], myDiffuseColor[2]);
	}
	else if (labelid == mySpotDirection.getLabelID())
	{
		if (mySpotDirUniform)
			mySpotDirUniform->setArrayValue("Value", mySpotDirection[0], mySpotDirection[1], mySpotDirection[2]);
	}
	else if (labelid == mySpotCutOff.getLabelID())
	{
		if (mySpotCutoffUniform)
			mySpotCutoffUniform->setValue("Value", cosf(mySpotCutOff.const_ref())); // send directly the cos, so don't have to do it at each fragment
	}
	else if (labelid == mySpotAttenuation.getLabelID())
	{
		if (mySpotExponentUniform)
			mySpotExponentUniform->setValue("Value", mySpotAttenuation);
	}
	else if (labelid == myConstAttenuation.getLabelID() || labelid == myLinAttenuation.getLabelID() || labelid == myQuadAttenuation.getLabelID())
	{
		if (myAttenuationUniform)
			myAttenuationUniform->setArrayValue("Value", myConstAttenuation, myLinAttenuation, myQuadAttenuation);
	}
	else if (labelid == myIsDirectional.getLabelID())
	{
		if (myIsDirectional)
		{
			myAttenuationUniform = nullptr;
		}
		else
		{
			myAttenuationUniform = KigsCore::GetInstanceOf(getName() + "AttenuationUniform", "API3DUniformFloat3");
			myAttenuationUniform->setValue(LABEL_TO_ID(Name), "attenuation");
			myAttenuationUniform->setArrayValue(LABEL_TO_ID(Value), myConstAttenuation, myLinAttenuation, myQuadAttenuation);
			myAttenuationUniform->Init();
		}
	}
	else if (labelid == myIsOn.getLabelID())
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

bool API3DLight::PreRendering(RendererOpenGL * renderer, Camera * cam, Point3D & camPos)
{
	if (!myIsOn)
		return false;

	if (((CoreModifiable*)myAttachedCamera) && cam != (CoreModifiable*)myAttachedCamera)
		return false;

	if (!myCamPosUniform)
	{
		return false;
	}

	// init blend mode
	/*renderer->SetBlendMode(RENDERER_BLEND_ON);
	renderer->SetBlendFuncMode( (RendererBlendFuncMode)(int)myBlendTarget, (RendererBlendFuncMode)(int)myBlendSource);*/
	//renderer->SetDepthTestMode(true);
	renderer->SetAlphaTestMode(RENDERER_ALPHA_TEST_ON);

	
	static_cast<API3DUniformFloat3*>(myCamPosUniform.get())->SetValue(&camPos.x);

	Point3D outP;
	Vector3D outV;

	GetFather()->SetupNodeIfNeeded();
	const Matrix3x4& lMat = GetFather()->GetLocalToGlobal();

	Point3D* PosOffset = (Point3D*)myPosOffset.getVector();
	lMat.TransformPoint(PosOffset, &outP);

	if (GetTypeOfLight() == DIRECTIONAL_LIGHT) // for directional light, position gives direction as a normal vector
	{
		outP.Normalize();
	}
	static_cast<API3DUniformFloat3*>(myPositionUniform.get())->SetValue(&outP.x);

	if (mySpotDirUniform)
	{
		lMat.TransformVector((Vector3D*)&mySpotDirection[0], &outV);
		static_cast<API3DUniformFloat3*>(mySpotDirUniform.get())->SetValue(&outV.x);
	}

	return true;
}

//#define SHOW_DEBUG
#ifdef SHOW_DEBUG
#include "GLSLDebugDraw.h"
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
	if (uniform == DIFFUSE_COLOR)
	{
		if (myDiffuseUniform)
		{
			myDiffuseUniform->setValue("Name", location);
		}
	}
	else if (uniform == SPECULAR_COLOR)
	{
		if (mySpecularUniform)
		{
			mySpecularUniform->setValue("Name", location);
		}
	}
	else if (uniform == AMBIANT_COLOR)
	{
		if (myAmbiantUniform)
		{
			myAmbiantUniform->setValue("Name", location);
		}
	}
	else if (uniform == POSITION_LIGHT)
	{
		if (myPositionUniform)
		{
			myPositionUniform->setValue("Name", location);
		}
	}
	else if (uniform == ATTENUATION)
	{
		if (myAttenuationUniform)
		{
			myAttenuationUniform->setValue("Name", location);
		}
	}
	else if (uniform == SPOT_CUT_OFF)
	{
		if (mySpotCutoffUniform)
		{
			mySpotCutoffUniform->setValue("Name", location);
		}
	}
	else if (uniform == SPOT_EXPONENT)
	{
		if (mySpotExponentUniform)
		{
			mySpotExponentUniform->setValue("Name", location);
		}
	}
	else if (uniform == SPOT_DIRECTION)
	{
		if (mySpotDirUniform)
		{
			mySpotDirUniform->setValue("Name", location);
		}
	}
}

void API3DLight::DrawLight(TravState* travstate)
{
	if (!myIsOn)
		return;
	if (myPositionUniform)
		((SP<API3DUniformBase>&)myPositionUniform)->DoPreDraw(travstate);
	if (myCamPosUniform)
		((SP < API3DUniformBase>&)myCamPosUniform)->DoPreDraw(travstate);
	if (myDiffuseUniform)
		((SP < API3DUniformBase>&)myDiffuseUniform)->DoPreDraw(travstate);
	if (mySpecularUniform)
		((SP < API3DUniformBase>&)mySpecularUniform)->DoPreDraw(travstate);
	if (myAmbiantUniform)
		((SP < API3DUniformBase>&)myAmbiantUniform)->DoPreDraw(travstate);
	if (mySpotDirUniform)
		((SP < API3DUniformBase>&)mySpotDirUniform)->DoPreDraw(travstate);
	if (myAttenuationUniform)
		((SP < API3DUniformBase>&)myAttenuationUniform)->DoPreDraw(travstate);
	if (mySpotCutoffUniform)
		((SP < API3DUniformBase>&)mySpotCutoffUniform)->DoPreDraw(travstate);
	if (mySpotExponentUniform)
		((SP < API3DUniformBase>&)mySpotExponentUniform)->DoPreDraw(travstate);
}

void API3DLight::PostDrawLight(TravState* travstate)
{
	if (!myIsOn)
		return;
	if (myPositionUniform)
		((SP < API3DUniformBase>&)myPositionUniform)->DoPostDraw(travstate);
	if (myCamPosUniform)
		((SP < API3DUniformBase>&)myCamPosUniform)->DoPostDraw(travstate);
	if (myDiffuseUniform)
		((SP < API3DUniformBase>&)myDiffuseUniform)->DoPostDraw(travstate);
	if (mySpecularUniform)
		((SP < API3DUniformBase>&)mySpecularUniform)->DoPostDraw(travstate);
	if (myAmbiantUniform)
		((SP < API3DUniformBase>&)myAmbiantUniform)->DoPostDraw(travstate);
	if (myAttenuationUniform)
		((SP < API3DUniformBase>&)myAttenuationUniform)->DoPostDraw(travstate);
	if (mySpotDirUniform)
		((SP < API3DUniformBase>&)mySpotDirUniform)->DoPostDraw(travstate);
	if (mySpotCutoffUniform)
		((SP < API3DUniformBase>&)mySpotCutoffUniform)->DoPostDraw(travstate);
	if (mySpotExponentUniform)
		((SP < API3DUniformBase>&)mySpotExponentUniform)->DoPostDraw(travstate);
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

DEFINE_METHOD(API3DLight, SetDiffuseColor)
{
	if (!myDiffuseUniform)
	{
		printf("No diffuse for '%s'\n", getName().c_str());
		return false;
	}

	float * lColor = (float*)privateParams;

	myDiffuseUniform->setArrayValue(LABEL_TO_ID(Value), lColor, 3);
	return false;
}

DEFINE_METHOD(API3DLight, SetSpecularColor)
{
	if (!mySpecularUniform)
	{
		printf("No specular for '%s'\n", getName().c_str());
		return false;
	}

	float * lColor = (float*)privateParams;

	mySpecularUniform->setArrayValue(LABEL_TO_ID(Value), lColor, 3);
	return false;
}

#ifdef USE_ATTFUNCTION
DEFINE_METHOD(API3DLight, SetAttenuation)
{
	float * lVal = (float*)privateParams;
	myConstAttenuation = lVal[0];
	myLinAttenuation = lVal[1];
	myQuadAttenuation = lVal[2];



	//myAttenuationUniform->setArrayValue(LABEL_TO_ID(Value), lVal, 3);
	return false;
}

DEFINE_METHOD(API3DLight, GetAttenuation)
{
	if (!myAttenuationUniform)
		return false;

	float * lVal = (float*)privateParams;

	myAttenuationUniform->getArrayValue(LABEL_TO_ID(Value), lVal, 3);
	return false;
}
#endif