#include "PrecompiledHeaders.h"

#include "Light.h"
#include "TravState.h"
#include "Node3D.h"
#include "Camera.h"

bool	Light::myAvailableLightArray[8]={true,true,true,true,true,true,true,true};

IMPLEMENT_CLASS_INFO(Light)

Light::Light(const kstl::string& name,CLASS_NAME_TREE_ARG) : Drawable(name,PASS_CLASS_NAME_TREE_ARG),
mySpecularColor(*this,false,LABEL_AND_ID(SpecularColor)),
myAmbientColor(*this,false,LABEL_AND_ID(AmbientColor)),
myDiffuseColor(*this,false,LABEL_AND_ID(DiffuseColor)),
myPosition(*this,false,LABEL_AND_ID(Position)),
mySpotDirection(*this,false,LABEL_AND_ID(SpotDirection)),
mySpotAttentuation(*this,false,LABEL_AND_ID(SpotAttenuation),KFLOAT_CONST(0.0f)),
mySpotCutOff(*this,false,LABEL_AND_ID(SpotCutOff),KFLOAT_CONST(1.0f)),
myConstAttenuation(*this,false,LABEL_AND_ID(ConstAttenuation),KFLOAT_CONST(1.0f)),
myLinAttenuation(*this,false,LABEL_AND_ID(LinAttenuation),KFLOAT_CONST(0.01f)),
myQuadAttenuation(*this,false,LABEL_AND_ID(QuadAttenuation),KFLOAT_CONST(0.0001f)),
myIsOn(*this,false,LABEL_AND_ID(IsOn),true),
myIsDirectional(*this,false,LABEL_AND_ID(IsDirectional),false),
myPositionNode3D(*this,false,LABEL_AND_ID(PositionNode3D))
{
	myLightIndex=(unsigned int)-1;

	SetSpecularColor(KFLOAT_CONST(1.0f),KFLOAT_CONST(1.0f),KFLOAT_CONST(1.0f),KFLOAT_CONST(1.0f));
	SetAmbientColor(KFLOAT_CONST(0.0f),KFLOAT_CONST(0.0f),KFLOAT_CONST(0.0f),KFLOAT_CONST(1.0f));
	SetDiffuseColor(KFLOAT_CONST(1.0f),KFLOAT_CONST(1.0f),KFLOAT_CONST(1.0f),KFLOAT_CONST(1.0f));
	SetPosition(KFLOAT_CONST(0.0f),KFLOAT_CONST(0.0f),KFLOAT_CONST(0.0f));
	myPosition[3]=KFLOAT_CONST(1.0f);

	mySpotDirection[0]=KFLOAT_CONST(0.0f);
	mySpotDirection[1]=KFLOAT_CONST(0.0f);
	mySpotDirection[2]=KFLOAT_CONST(0.0f);

}    

bool	Light::PreDraw(TravState* travstate)
{
	kstl::string cam;
	if(getValue(LABEL_TO_ID(Camera),cam))
	{
		if(travstate->GetCurrentCamera()->getName() != cam)
			return false;
	}


	if(Drawable::PreDraw(travstate))
	{
		if(myIsOn)
		{
			// update position and orientation if needed
			// will work only if light is attached to a Node with identity local to global
			if((CoreModifiable*)myPositionNode3D)
			{
				Node3D* attachedNode=(Node3D*)((CoreModifiable*)myPositionNode3D);

				attachedNode->SetupNodeIfNeeded();
				const Matrix3x4& lMat = attachedNode->GetLocalToGlobal();
				
				Point3D lPos(0.0,0.0,0.0);
				lMat.TransformPoints(&lPos,1);

				SetPosition(lPos.x,lPos.y,lPos.z);
				SetSpotDirection(lMat.e[0][0],lMat.e[0][1],lMat.e[0][2]);
			}

			myLightIndex=(unsigned int)-1;
			int i;
			
			for(i=0;i<8;i++)
			{
				if(myAvailableLightArray[i]==true)
				{
					myLightIndex=(unsigned int)i;
					myAvailableLightArray[i]=false;
					break;
				}
			}

			if(myLightIndex != (unsigned int)-1)
			{
				return true;
			}
		}
	}
	return false;
}

bool	Light::PostDraw(TravState* travstate)
{
	if(Drawable::PostDraw(travstate))
	{
		if(myIsOn)
		{
			if(myLightIndex != (unsigned int)-1)
			{
				myAvailableLightArray[myLightIndex]=true;
				return true;
			}
		}
	}
	return false;
}
    
Light::~Light()
{
	if(myLightIndex != (unsigned int)-1)
	{
		myAvailableLightArray[myLightIndex]=true;
	}
}    
