#include "HLSLCutShader.h"


IMPLEMENT_CONSTRUCTOR(API3DCutShader)
{
	mPlaneCount.changeNotificationLevel(Owner);
}

void API3DCutShader::NotifyUpdate(unsigned int labelid)
{
	if (labelid == mPlaneCount.getID())
	{
		Dealloc();
	}
	ParentClassType::NotifyUpdate(labelid);
}


std::string API3DCutShader::GetVertexShader()
{
	return ParentClassType::GetVertexShader();
}

std::string API3DCutShader::GetFragmentShader()
{
	std::string result;
	
	result += "#define CLIENT_ISFRONTFACE\n"; 
	result +=API3DGenericMeshShader::GetDefaultVaryings();
	result += API3DGenericMeshShader::GetDefaultFragmentShaderUniforms();
	result += API3DGenericMeshShader::GetDefaultLightingFunctions();


	result += R"====(
cbuffer cutData
{
	float4 cutPos[6];
	float4 cutNormal[6];
	float4 outlineSize[2];
};
)====";

	result += API3DGenericMeshShader::GetDefaultFragmentShaderMain(R"====(
	if(!input.IsFrontFace)
	{ 
		diffuseColor = float4(diffuseColor.xyz * 0.8, diffuseColor.w); 
		ambientColor = float4(ambientColor.xyz * 0.8, ambientColor.w); 
		normalN = -normalN;
	}
	
	[unroll] for(int i = 0; i < 6; ++i)
	{
		float d = dot(normalN, cutNormal[i].xyz);
		float s = dot(input.FragPos.xyz - cutPos[i].xyz, cutNormal[i].xyz)/dot(cutNormal[i].xyz, cutNormal[i].xyz);

		if(s < outlineSize[i>>2][i&3] && d < 0.5 && d > -0.5)
		{
			diffuseColor = float4((cutNormal[i].xyz + float3(1,1,1))/2.0, diffuseColor.w);
			ambientColor = float4((cutNormal[i].xyz + float3(1,1,1))/2.0, ambientColor.w);
		}
	}
)====",
R"====(
	[unroll] for(int i = 0; i < 6; ++i)
	{
		if(dot(input.FragPos.xyz - cutPos[i].xyz, cutNormal[i].xyz)<0.0) discard;
	}
)====");

	return result;
}



IMPLEMENT_CLASS_INFO(API3DCutShader)
