#include "PrecompiledHeaders.h"

#include "Fog.h"

IMPLEMENT_CLASS_INFO(Fog)

Fog::Fog(const kstl::string& name,CLASS_NAME_TREE_ARG) : Drawable(name,PASS_CLASS_NAME_TREE_ARG),
mColor(*this,false,LABEL_AND_ID(Color)),
mStartDistance(*this,false,LABEL_AND_ID(StartDistance),KFLOAT_CONST(1.0)),
mEndDistance(*this,false,LABEL_AND_ID(EndDistance),KFLOAT_CONST(100.0)),
mDensity(*this,false,LABEL_AND_ID(Density),KFLOAT_CONST(1.0)),
mMode(*this,false,LABEL_AND_ID(Mode),"EXP","EXP2","LINEAR")
{
	mColor[0]=KFLOAT_CONST(0.1f);
	mColor[1]=KFLOAT_CONST(0.1f);
	mColor[2]=KFLOAT_CONST(0.1f);
	mColor[3]=KFLOAT_CONST(1.0f);

	mMode.setValue("LINEAR");
}    

