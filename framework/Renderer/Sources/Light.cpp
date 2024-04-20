#include "PrecompiledHeaders.h"

#include "Light.h"
#include "TravState.h"
#include "Node3D.h"
#include "Camera.h"

using namespace Kigs::Draw;


IMPLEMENT_CLASS_INFO(Light)

Light::Light(const std::string& name,CLASS_NAME_TREE_ARG) : Node3D(name,PASS_CLASS_NAME_TREE_ARG)
,mLightType(*this,"LightType", "POINT", "DIRECTIONAL", "SPOT") 	//0 for point, 1 for directional, 2 for spot
{
	
}    

void Light::InitModifiable()
{
	ParentClassType::InitModifiable();
	if (getAttribute("Offset"))
	{
		v3f offset = getValue<v3f>("Offset");
		auto m = GetLocal();
		m=column(m,3,glm::vec4(offset,1.0f));
		ChangeMatrix(m);
		RemoveDynamicAttribute("Offset");
	}
}


Light::~Light()
{
	
}    
