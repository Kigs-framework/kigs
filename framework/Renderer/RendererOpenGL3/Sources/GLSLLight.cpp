#include "PrecompiledHeaders.h"
#include "GLSLLight.h"
#include "RendererOpenGL.h"
#include "ModuleSceneGraph.h"
#include "Camera.h"
#include "GLSLUniform.h"
#include "Node3D.h"
#include "TravState.h"
#include "ModuleRenderer.h"

using namespace Kigs::Draw;

IMPLEMENT_CLASS_INFO(API3DLight)

API3DLight::API3DLight(const std::string& name, CLASS_NAME_TREE_ARG)
	: Light(name, PASS_CLASS_NAME_TREE_ARG)

	, mPositionUniform(nullptr)
	, mCamPosUniform(nullptr)

	, mDiffuseUniform(nullptr)
	, mSpecularUniform(nullptr)
	, mAmbiantUniform(nullptr)

	, mAttenuationUniform(nullptr)
	, mSpotDirUniform(nullptr)
	, mSpotCutoffUniform(nullptr)
	, mSpotExponentUniform(nullptr)
{

}

void	API3DLight::InitModifiable()
{
	
	int lLightType = GetTypeOfLight();

	mPositionUniform = KigsCore::GetInstanceOf(getName() + "PositionUniform", "API3DUniformFloat3");
	mPositionUniform->setValue("UniformName", "lightPos");
	mPositionUniform->Init();

	mCamPosUniform = KigsCore::GetInstanceOf(getName() + "CamPosUniform", "API3DUniformFloat3");
	mCamPosUniform->setValue("UniformName", "camPos");
	mCamPosUniform->Init();

	mDiffuseUniform = KigsCore::GetInstanceOf(getName() + "DiffuseUniform", "API3DUniformFloat3");
	mDiffuseUniform->setValue("UniformName", "light_diffuse");
	mDiffuseUniform->setArrayValue("Value", mDiffuseColor[0], mDiffuseColor[1], mDiffuseColor[2]);
	mDiffuseUniform->Init();

	mSpecularUniform = KigsCore::GetInstanceOf(getName() + "SpecularUniform", "API3DUniformFloat3");
	mSpecularUniform->setValue("UniformName", "light_specular");
	mSpecularUniform->setArrayValue("Value", mSpecularColor[0], mSpecularColor[1], mSpecularColor[2]);
	mSpecularUniform->Init();

	mAmbiantUniform = KigsCore::GetInstanceOf(getName() + "AmbiantUniform", "API3DUniformFloat3");
	mAmbiantUniform->setValue("UniformName", "light_ambiant");
	mAmbiantUniform->setArrayValue("Value", mAmbientColor[0], mAmbientColor[1], mAmbientColor[2]);
	mAmbiantUniform->Init();

	// create shader uniform
	if (lLightType == SPOT_LIGHT)
	{
		mSpotDirUniform = KigsCore::GetInstanceOf(getName() + "SpotDirUniform", "API3DUniformFloat3");
		mSpotDirUniform->setValue("UniformName", "spotDir");
		mSpotDirUniform->setArrayValue("Value", 0.0f, 0.0f, 0.0f);
		mSpotDirUniform->setValue("Normalize", true);
		mSpotDirUniform->Init();

		mSpotCutoffUniform = KigsCore::GetInstanceOf(getName() + "SpotCutoffUniform", "API3DUniformFloat");
		mSpotCutoffUniform->setValue("UniformName", "spotCutoff");
		mSpotCutoffUniform->setValue("Value", cosf(mSpotCutOff)); // send directly the cos, so don't have to do it at each fragment
		mSpotCutoffUniform->Init();


		mSpotExponentUniform = KigsCore::GetInstanceOf(getName() + "SpotExponentUniform", "API3DUniformFloat");
		mSpotExponentUniform->setValue("UniformName", "spotExponent");
		mSpotExponentUniform->setValue("Value", mSpotAttenuation);
		mSpotExponentUniform->Init();
	}

	if (lLightType != DIRECTIONAL_LIGHT)
	{
		mAttenuationUniform = KigsCore::GetInstanceOf(getName() + "AttenuationUniform", "API3DUniformFloat3");
		mAttenuationUniform->setValue("UniformName", "attenuation");
		mAttenuationUniform->setArrayValue("Value", mConstAttenuation, mLinAttenuation, mQuadAttenuation);
		mAttenuationUniform->Init();
	}

	ParentClassType::InitModifiable();

	setOwnerNotification("SpecularColor", true);
	setOwnerNotification("DiffuseColor", true);
	setOwnerNotification("AmbientColor", true);
	setOwnerNotification("SpotAttenuation", true);
	setOwnerNotification("SpotCutOff", true);
	setOwnerNotification("ConstAttenuation", true);
	setOwnerNotification("LinAttenuation", true);
	setOwnerNotification("QuadAttenuation", true);
	setOwnerNotification("IsOn", true);

}


API3DLight::~API3DLight()
{
	mPositionUniform = mCamPosUniform = mSpotCutoffUniform = mSpotExponentUniform = mDiffuseUniform = mSpecularUniform = mAmbiantUniform = mAttenuationUniform = mSpotDirUniform = nullptr;

	// notify scenegraph that I am dead
	ModuleSceneGraph* scenegraph = (ModuleSceneGraph*)KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex);
	scenegraph->NotifyDefferedItemDeath(this);
}

void API3DLight::NotifyUpdate(const unsigned int  labelid)
{
	if (labelid == KigsID("SpecularColor")._id)
	{
		if (mSpecularUniform)
			mSpecularUniform->setArrayValue("Value", mSpecularColor[0], mSpecularColor[1], mSpecularColor[2]);
	}
	else if (labelid == KigsID("AmbientColor")._id)
	{
		if (mAmbiantUniform)
			mAmbiantUniform->setArrayValue("Value", mAmbientColor[0], mAmbientColor[1], mAmbientColor[2]);
	}
	else if (labelid == KigsID("DiffuseColor")._id)
	{
		if (mDiffuseUniform)
			mDiffuseUniform->setArrayValue("Value", mDiffuseColor[0], mDiffuseColor[1], mDiffuseColor[2]);
	}
	else if (labelid == KigsID("SpotCutOff")._id)
	{
		if (mSpotCutoffUniform)
			mSpotCutoffUniform->setValue("Value", cosf(mSpotCutOff)); // send directly the cos, so don't have to do it at each fragment
	}
	else if (labelid == KigsID("SpotAttenuation")._id)
	{
		if (mSpotExponentUniform)
			mSpotExponentUniform->setValue("Value", mSpotAttenuation);
	}
	else if (labelid == KigsID("ConstAttenuation")._id || labelid == KigsID("LinAttenuation")._id || labelid == KigsID("QuadAttenuation")._id)
	{
		if (mAttenuationUniform)
			mAttenuationUniform->setArrayValue("Value", mConstAttenuation, mLinAttenuation, mQuadAttenuation);
	}
	else if (labelid == KigsID("IsOn")._id)
	{
		ModuleSceneGraph* scenegraph = (ModuleSceneGraph*)KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex);
		scenegraph->SignalLightChange(this);
	}
}

bool API3DLight::PreRendering(RendererOpenGL * renderer, Camera * cam, Point3D & camPos)
{
	if (!mIsOn)
		return false;


	if (!mCamPosUniform)
	{
		return false;
	}

	// init blend mode
	/*renderer->SetBlendMode(RENDERER_BLEND_ON);
	renderer->SetBlendFuncMode( (RendererBlendFuncMode)(int)mBlendTarget, (RendererBlendFuncMode)(int)mBlendSource);*/
	//renderer->SetDepthTestMode(true);
	renderer->SetAlphaTestMode(RENDERER_ALPHA_TEST_ON);

	
	static_cast<API3DUniformFloat3*>(mCamPosUniform.get())->SetValue(&camPos.x);

	Point3D outP;
	v3f outV;

	SetupNodeIfNeeded();
	const mat3x4& lMat = GetLocalToGlobal();

	Point3D PosOffset(0,0,0);
	lMat.TransformPoint(&PosOffset, &outP);

	if (GetTypeOfLight() == DIRECTIONAL_LIGHT) // for directional light, position gives direction as a normal vector
	{
		outP.Normalize();
	}
	static_cast<API3DUniformFloat3*>(mPositionUniform.get())->SetValue(&outP.x);

	if (mSpotDirUniform)
	{
		// spot direction is minus z axis
		v3f dir(0.0f, 0.0f, -1.0f);
		lMat.TransformVector((v3f*)&dir.x, &outV);
		static_cast<API3DUniformFloat3*>(mSpotDirUniform.get())->SetValue(&outV.x);
	}

	return true;
}

//#define SHOW_DEBUG
#ifdef SHOW_DEBUG
#include "GLSLDebugDraw.h"
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
		Point3D outP;
		const mat3x4& lMat = GetLocalToGlobal();
		Point3D PosOffset(0,0,0);
		lMat.TransformPoint(&PosOffset, &outP);
		dd::sphere(outP, v3f(1, 0, 0), 0.05);
	}
	break;
	case 2:
	{
		Point3D outP;
		v3f outDir;
		const mat3x4& lMat = GetLocalToGlobal();
		Point3D PosOffset(0, 0, 0);
		lMat.TransformPoint(&PosOffset, &outP);

		v3f dir(0.0f,0.0f,-1.0f);
		lMat.TransformVector(&dir, &outDir);
		//dd::cone(outP, outDir*10, v3f(1, 0, 0),((1-mSpotCutOff)/PI)*100, 0);
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
	if (uniform == DIFFUSE_COLOR)
	{
		if (mDiffuseUniform)
		{
			mDiffuseUniform->setValue("UniformName", location);
		}
	}
	else if (uniform == SPECULAR_COLOR)
	{
		if (mSpecularUniform)
		{
			mSpecularUniform->setValue("UniformName", location);
		}
	}
	else if (uniform == AMBIANT_COLOR)
	{
		if (mAmbiantUniform)
		{
			mAmbiantUniform->setValue("UniformName", location);
		}
	}
	else if (uniform == POSITION_LIGHT)
	{
		if (mPositionUniform)
		{
			mPositionUniform->setValue("UniformName", location);
		}
	}
	else if (uniform == ATTENUATION)
	{
		if (mAttenuationUniform)
		{
			mAttenuationUniform->setValue("UniformName", location);
		}
	}
	else if (uniform == SPOT_CUT_OFF)
	{
		if (mSpotCutoffUniform)
		{
			mSpotCutoffUniform->setValue("UniformName", location);
		}
	}
	else if (uniform == SPOT_EXPONENT)
	{
		if (mSpotExponentUniform)
		{
			mSpotExponentUniform->setValue("UniformName", location);
		}
	}
	else if (uniform == SPOT_DIRECTION)
	{
		if (mSpotDirUniform)
		{
			mSpotDirUniform->setValue("UniformName", location);
		}
	}
}

void API3DLight::DrawLight(TravState* travstate)
{
	if (!mIsOn)
		return;
	if (mPositionUniform)
		mPositionUniform->as<Drawable>()->DoPreDraw(travstate);
	if (mCamPosUniform)
		mCamPosUniform->as<Drawable>()->DoPreDraw(travstate);
	if (mDiffuseUniform)
		mDiffuseUniform->as<Drawable>()->DoPreDraw(travstate);
	if (mSpecularUniform)
		mSpecularUniform->as<Drawable>()->DoPreDraw(travstate);
	if (mAmbiantUniform)
		mAmbiantUniform->as<Drawable>()->DoPreDraw(travstate);
	if (mSpotDirUniform)
		mSpotDirUniform->as<Drawable>()->DoPreDraw(travstate);
	if (mAttenuationUniform)
		mAttenuationUniform->as<Drawable>()->DoPreDraw(travstate);
	if (mSpotCutoffUniform)
		mSpotCutoffUniform->as<Drawable>()->DoPreDraw(travstate);
	if (mSpotExponentUniform)
		mSpotExponentUniform->as<Drawable>()->DoPreDraw(travstate);
}

void API3DLight::PostDrawLight(TravState* travstate)
{
	if (!mIsOn)
		return;
	if (mPositionUniform)
		mPositionUniform->as<Drawable>()->DoPostDraw(travstate);
	if (mCamPosUniform)
		mCamPosUniform->as<Drawable>()->DoPostDraw(travstate);
	if (mDiffuseUniform)
		mDiffuseUniform->as<Drawable>()->DoPostDraw(travstate);
	if (mSpecularUniform)
		mSpecularUniform->as<Drawable>()->DoPostDraw(travstate);
	if (mAmbiantUniform)
		mAmbiantUniform->as<Drawable>()->DoPostDraw(travstate);
	if (mAttenuationUniform)
		mAttenuationUniform->as<Drawable>()->DoPostDraw(travstate);
	if (mSpotDirUniform)
		mSpotDirUniform->as<Drawable>()->DoPostDraw(travstate);
	if (mSpotCutoffUniform)
		mSpotCutoffUniform->as<Drawable>()->DoPostDraw(travstate);
	if (mSpotExponentUniform)
		mSpotExponentUniform->as<Drawable>()->DoPostDraw(travstate);
}

int API3DLight::GetTypeOfLight()
{
	return mLightType;
}

