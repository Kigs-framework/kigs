#include "PrecompiledHeaders.h"

#include "Fog.h"

IMPLEMENT_CLASS_INFO(Fog)

Fog::Fog(const kstl::string& name,CLASS_NAME_TREE_ARG) : Drawable(name,PASS_CLASS_NAME_TREE_ARG),
myColor(*this,false,LABEL_AND_ID(Color)),
myStartDist(*this,false,LABEL_AND_ID(StartDistance),KFLOAT_CONST(1.0)),
myEndDist(*this,false,LABEL_AND_ID(EndDistance),KFLOAT_CONST(100.0)),
myDensity(*this,false,LABEL_AND_ID(Density),KFLOAT_CONST(1.0)),
myMode(*this,false,LABEL_AND_ID(Mode),"EXP","EXP2","LINEAR")
{
	myColor[0]=KFLOAT_CONST(0.1f);
	myColor[1]=KFLOAT_CONST(0.1f);
	myColor[2]=KFLOAT_CONST(0.1f);
	myColor[3]=KFLOAT_CONST(1.0f);

	myMode.setValue("LINEAR");
}    

