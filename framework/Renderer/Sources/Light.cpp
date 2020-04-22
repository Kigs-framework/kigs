#include "PrecompiledHeaders.h"

#include "Light.h"
#include "TravState.h"
#include "Node3D.h"
#include "Camera.h"


IMPLEMENT_CLASS_INFO(Light)

Light::Light(const kstl::string& name,CLASS_NAME_TREE_ARG) : Node3D(name,PASS_CLASS_NAME_TREE_ARG)
,mySpecularColor(*this,false,LABEL_AND_ID(SpecularColor),1,1,1)
,myAmbientColor(*this,false,LABEL_AND_ID(AmbientColor),0,0,0)
,myDiffuseColor(*this,false,LABEL_AND_ID(DiffuseColor),1,1,1)
,mySpotAttenuation(*this,false,LABEL_AND_ID(SpotAttenuation),KFLOAT_CONST(0.0f))
,mySpotCutOff(*this,false,LABEL_AND_ID(SpotCutOff),KFLOAT_CONST(1.0f))
,myConstAttenuation(*this,false,LABEL_AND_ID(ConstAttenuation),KFLOAT_CONST(1.0f))
,myLinAttenuation(*this,false,LABEL_AND_ID(LinAttenuation),KFLOAT_CONST(0.01f))
,myQuadAttenuation(*this,false,LABEL_AND_ID(QuadAttenuation),KFLOAT_CONST(0.0001f))
,myIsOn(*this,false,LABEL_AND_ID(IsOn),true)
,myLightType(*this, true, LABEL_AND_ID(LightType), "POINT", "DIRECTIONAL", "SPOT") 	//0 for point, 1 for directional, 2 for spot
{


}    


Light::~Light()
{
	
}    
