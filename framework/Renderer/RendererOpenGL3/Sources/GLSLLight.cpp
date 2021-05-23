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
	: Light(name, PASS_CLASS_NAME_TREE_ARG)

	, mPositionUniform(NULL)
	, mCamPosUniform(NULL)

	, mDiffuseUniform(NULL)
	, mSpecularUniform(NULL)
	, mAmbiantUniform(NULL)

	, mAttenuationUniform(NULL)
	, mSpotDirUniform(NULL)
	, mSpotCutoffUniform(NULL)
	, mSpotExponentUniform(NULL)
{

}

void	API3DLight::InitModifiable()
{
	
	int lLightType = GetTypeOfLight();

	mPositionUniform = KigsCore::GetInstanceOf(getName() + "PositionUniform", "API3DUniformFloat3");
	mPositionUniform->setValue(LABEL_TO_ID(Name), "lightPos");
	mPositionUniform->Init();

	mCamPosUniform = KigsCore::GetInstanceOf(getName() + "CamPosUniform", "API3DUniformFloat3");
	mCamPosUniform->setValue(LABEL_TO_ID(Name), "camPos");
	mCamPosUniform->Init();

	mDiffuseUniform = KigsCore::GetInstanceOf(getName() + "DiffuseUniform", "API3DUniformFloat3");
	mDiffuseUniform->setValue(LABEL_TO_ID(Name), "light_diffuse");
	mDiffuseUniform->setArrayValue(LABEL_TO_ID(Value), mDiffuseColor[0], mDiffuseColor[1], mDiffuseColor[2]);
	mDiffuseUniform->Init();

	mSpecularUniform = KigsCore::GetInstanceOf(getName() + "SpecularUniform", "API3DUniformFloat3");
	mSpecularUniform->setValue(LABEL_TO_ID(Name), "light_specular");
	mSpecularUniform->setArrayValue(LABEL_TO_ID(Value), mSpecularColor[0], mSpecularColor[1], mSpecularColor[2]);
	mSpecularUniform->Init();

	mAmbiantUniform = KigsCore::GetInstanceOf(getName() + "AmbiantUniform", "API3DUniformFloat3");
	mAmbiantUniform->setValue(LABEL_TO_ID(Name), "light_ambiant");
	mAmbiantUniform->setArrayValue(LABEL_TO_ID(Value), mAmbientColor[0], mAmbientColor[1], mAmbientColor[2]);
	mAmbiantUniform->Init();

	// create shader uniform
	if (lLightType == SPOT_LIGHT)
	{
		mSpotDirUniform = KigsCore::GetInstanceOf(getName() + "SpotDirUniform", "API3DUniformFloat3");
		mSpotDirUniform->setValue(LABEL_TO_ID(Name), "spotDir");
		mSpotDirUniform->setArrayValue(LABEL_TO_ID(Value), 0.0f, 0.0f, 0.0f);
		mSpotDirUniform->setValue(LABEL_TO_ID(Normalize), true);
		mSpotDirUniform->Init();

		mSpotCutoffUniform = KigsCore::GetInstanceOf(getName() + "SpotCutoffUniform", "API3DUniformFloat");
		mSpotCutoffUniform->setValue(LABEL_TO_ID(Name), "spotCutoff");
		mSpotCutoffUniform->setValue(LABEL_TO_ID(Value), cosf(mSpotCutOff.const_ref())); // send directly the cos, so don't have to do it at each fragment
		mSpotCutoffUniform->Init();


		mSpotExponentUniform = KigsCore::GetInstanceOf(getName() + "SpotExponentUniform", "API3DUniformFloat");
		mSpotExponentUniform->setValue(LABEL_TO_ID(Name), "spotExponent");
		mSpotExponentUniform->setValue(LABEL_TO_ID(Value), mSpotAttenuation);
		mSpotExponentUniform->Init();
	}

	if (lLightType != DIRECTIONAL_LIGHT)
	{
		mAttenuationUniform = KigsCore::GetInstanceOf(getName() + "AttenuationUniform", "API3DUniformFloat3");
		mAttenuationUniform->setValue(LABEL_TO_ID(Name), "attenuation");
		mAttenuationUniform->setArrayValue(LABEL_TO_ID(Value), mConstAttenuation, mLinAttenuation, mQuadAttenuation);
		mAttenuationUniform->Init();
	}

	ParentClassType::InitModifiable();

	mSpecularColor.changeNotificationLevel(Owner);
	mDiffuseColor.changeNotificationLevel(Owner);
	mAmbientColor.changeNotificationLevel(Owner);
	mSpotAttenuation.changeNotificationLevel(Owner);
	mSpotCutOff.changeNotificationLevel(Owner);
	mConstAttenuation.changeNotificationLevel(Owner);
	mLinAttenuation.changeNotificationLevel(Owner);
	mQuadAttenuation.changeNotificationLevel(Owner);
	mIsOn.changeNotificationLevel(Owner);
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
	if (labelid == mSpecularColor.getLabelID())
	{
		if (mSpecularUniform)
			mSpecularUniform->setArrayValue("Value", mSpecularColor[0], mSpecularColor[1], mSpecularColor[2]);
	}
	else if (labelid == mAmbientColor.getLabelID())
	{
		if (mAmbiantUniform)
			mAmbiantUniform->setArrayValue("Value", mAmbientColor[0], mAmbientColor[1], mAmbientColor[2]);
	}
	else if (labelid == mDiffuseColor.getLabelID())
	{
		if (mDiffuseUniform)
			mDiffuseUniform->setArrayValue("Value", mDiffuseColor[0], mDiffuseColor[1], mDiffuseColor[2]);
	}
	else if (labelid == mSpotCutOff.getLabelID())
	{
		if (mSpotCutoffUniform)
			mSpotCutoffUniform->setValue("Value", cosf(mSpotCutOff.const_ref())); // send directly the cos, so don't have to do it at each fragment
	}
	else if (labelid == mSpotAttenuation.getLabelID())
	{
		if (mSpotExponentUniform)
			mSpotExponentUniform->setValue("Value", mSpotAttenuation);
	}
	else if (labelid == mConstAttenuation.getLabelID() || labelid == mLinAttenuation.getLabelID() || labelid == mQuadAttenuation.getLabelID())
	{
		if (mAttenuationUniform)
			mAttenuationUniform->setArrayValue("Value", mConstAttenuation, mLinAttenuation, mQuadAttenuation);
	}
	else if (labelid == mIsOn.getLabelID())
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
	Vector3D outV;

	SetupNodeIfNeeded();
	const Matrix3x4& lMat = GetLocalToGlobal();

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
		Vector3D dir(0.0f, 0.0f, -1.0f);
		lMat.TransformVector((Vector3D*)&dir.x, &outV);
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
		const Matrix3x4& lMat = GetLocalToGlobal();
		Point3D PosOffset(0,0,0);
		lMat.TransformPoint(&PosOffset, &outP);
		dd::sphere(outP, Vector3D(1, 0, 0), 0.05);
	}
	break;
	case 2:
	{
		Point3D outP;
		Vector3D outDir;
		const Matrix3x4& lMat = GetLocalToGlobal();
		Point3D PosOffset(0, 0, 0);
		lMat.TransformPoint(&PosOffset, &outP);

		Vector3D dir(0.0f,0.0f,-1.0f);
		lMat.TransformVector(&dir, &outDir);
		//dd::cone(outP, outDir*10, Vector3D(1, 0, 0),((1-mSpotCutOff)/PI)*100, 0);
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
		if (mDiffuseUniform)
		{
			mDiffuseUniform->setValue("Name", location);
		}
	}
	else if (uniform == SPECULAR_COLOR)
	{
		if (mSpecularUniform)
		{
			mSpecularUniform->setValue("Name", location);
		}
	}
	else if (uniform == AMBIANT_COLOR)
	{
		if (mAmbiantUniform)
		{
			mAmbiantUniform->setValue("Name", location);
		}
	}
	else if (uniform == POSITION_LIGHT)
	{
		if (mPositionUniform)
		{
			mPositionUniform->setValue("Name", location);
		}
	}
	else if (uniform == ATTENUATION)
	{
		if (mAttenuationUniform)
		{
			mAttenuationUniform->setValue("Name", location);
		}
	}
	else if (uniform == SPOT_CUT_OFF)
	{
		if (mSpotCutoffUniform)
		{
			mSpotCutoffUniform->setValue("Name", location);
		}
	}
	else if (uniform == SPOT_EXPONENT)
	{
		if (mSpotExponentUniform)
		{
			mSpotExponentUniform->setValue("Name", location);
		}
	}
	else if (uniform == SPOT_DIRECTION)
	{
		if (mSpotDirUniform)
		{
			mSpotDirUniform->setValue("Name", location);
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

