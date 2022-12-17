#include "PrecompiledHeaders.h"

#include "Fog.h"

IMPLEMENT_CLASS_INFO(Fog)

Fog::Fog(const std::string& name,CLASS_NAME_TREE_ARG) : Drawable(name,PASS_CLASS_NAME_TREE_ARG),
mColor(*this,false,"Color"),
mStartDistance(*this,false,"StartDistance",1.0f),
mEndDistance(*this,false,"EndDistance",100.0),
mDensity(*this,false,"Density",1.0f),
mMode(*this,false,"Mode","EXP","EXP2","LINEAR")
{
	mColor[0]=0.1f;
	mColor[1]=0.1f;
	mColor[2]=0.1f;
	mColor[3]=1.0f;

	mMode.setValue("LINEAR");
}    

