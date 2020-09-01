#include "PrecompiledHeaders.h"

#include "Light.h"
#include "TravState.h"
#include "Node3D.h"
#include "Camera.h"


IMPLEMENT_CLASS_INFO(Light)

Light::Light(const kstl::string& name,CLASS_NAME_TREE_ARG) : Node3D(name,PASS_CLASS_NAME_TREE_ARG)
,mSpecularColor(*this,false,LABEL_AND_ID(SpecularColor),1,1,1)
,mAmbientColor(*this,false,LABEL_AND_ID(AmbientColor),0,0,0)
,mDiffuseColor(*this,false,LABEL_AND_ID(DiffuseColor),1,1,1)
,mSpotAttenuation(*this,false,LABEL_AND_ID(SpotAttenuation),KFLOAT_CONST(0.0f))
,mSpotCutOff(*this,false,LABEL_AND_ID(SpotCutOff),KFLOAT_CONST(1.0f))
,mConstAttenuation(*this,false,LABEL_AND_ID(ConstAttenuation),KFLOAT_CONST(1.0f))
,mLinAttenuation(*this,false,LABEL_AND_ID(LinAttenuation),KFLOAT_CONST(0.01f))
,mQuadAttenuation(*this,false,LABEL_AND_ID(QuadAttenuation),KFLOAT_CONST(0.0001f))
,mIsOn(*this,false,LABEL_AND_ID(IsOn),true)
,mLightType(*this, true, LABEL_AND_ID(LightType), "POINT", "DIRECTIONAL", "SPOT") 	//0 for point, 1 for directional, 2 for spot
{
	
}    

void Light::InitModifiable()
{
	ParentClassType::InitModifiable();
	if (getAttribute("Offset"))
	{
		v3f offset = getValue<v3f>("Offset");
		auto m = GetLocal();
		m.Pos = offset;
		ChangeMatrix(m);
		RemoveDynamicAttribute("Offset");
	}
}


Light::~Light()
{
	
}    
