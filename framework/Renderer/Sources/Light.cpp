#include "PrecompiledHeaders.h"

#include "Light.h"
#include "TravState.h"
#include "Node3D.h"
#include "Camera.h"


IMPLEMENT_CLASS_INFO(Light)

Light::Light(const std::string& name,CLASS_NAME_TREE_ARG) : Node3D(name,PASS_CLASS_NAME_TREE_ARG)
,mSpecularColor(*this,false,"SpecularColor",1,1,1)
,mAmbientColor(*this,false,"AmbientColor",0,0,0)
,mDiffuseColor(*this,false,"DiffuseColor",1,1,1)
,mSpotAttenuation(*this,false,"SpotAttenuation",0.0f)
,mSpotCutOff(*this,false,"SpotCutOff",1.0f)
,mConstAttenuation(*this,false,"ConstAttenuation",1.0f)
,mLinAttenuation(*this,false,"LinAttenuation",0.01f)
,mQuadAttenuation(*this,false,"QuadAttenuation",0.0001f)
,mIsOn(*this,false,"IsOn",true)
,mLightType(*this, true, "LightType", "POINT", "DIRECTIONAL", "SPOT") 	//0 for point, 1 for directional, 2 for spot
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
