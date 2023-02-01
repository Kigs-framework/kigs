#include "GLSLCutShader.h"

using namespace Kigs::Draw;

IMPLEMENT_CONSTRUCTOR(API3DCutShader)
{
	setOwnerNotification("PlaneCount", true);
}

void API3DCutShader::NotifyUpdate(unsigned int labelid)
{
	if (labelid == KigsID("PlaneCount")._id)
	{
		Dealloc();
	}
	ParentClassType::NotifyUpdate(labelid);
}


std::string API3DCutShader::GetVertexShader()
{
	std::string result = API3DGenericMeshShader::GetDefaultVertexShaderAttributes();
	result += API3DGenericMeshShader::GetDefaultVertexShaderUniforms();
	result += API3DGenericMeshShader::GetDefaultVaryings();

	result += "void main(){\n";
	
	result += R"====(
#ifdef CLIENT_STATE_TEXTURE_COORD_ARRAY0 
	vTexcoord = attrib_texcoord;
#endif
#ifdef CLIENT_STATE_TANGENT_ARRAY 
	T = normalize(model_matrix * vec4(attrib_tangent, 0.0)).xyz;
#endif
#ifdef CLIENT_STATE_NORMAL_ARRAY 
	N = normalize(model_matrix * vec4(attrib_normal, 0.0)).xyz;
#endif
#ifdef CLIENT_STATE_COLOR_ARRAY 
	vColor = attrib_color;
#endif
	
FragPos = vec3(model_matrix * vec4(attrib_vertex, 1.0));
/*
	vec3 view_vector = (view_matrix * vec4(0,0,-1,0)).xyz;
	if(dot(view_vector, N) < 0.0)
	{
		FragPos -= N*0.0001;
		//N = vec3(0,1,0);
	}
*/

#ifdef HOLOGRAPHIC
	int arrayIndex = int(aRenderTargetArrayIndex);
	vRenderTargetArrayIndex = aRenderTargetArrayIndex;
	gl_Position = uHolographicViewProjectionMatrix[arrayIndex] * vec4(FragPos,1);
#else
	gl_Position = proj_matrix * view_matrix * vec4(FragPos,1);
#endif
)====";


	result += "}\n";
	return result;
}

std::string API3DCutShader::GetFragmentShader()
{
	std::string result = API3DGenericMeshShader::GetDefaultVaryings();
	result += API3DGenericMeshShader::GetDefaultFragmentShaderUniforms();
	result += API3DGenericMeshShader::GetDefaultLightingFunctions();

	STACK_STRING(str, 512, "#define PLANE_COUNT %d\n", (int)mPlaneCount);
	result += str;

	result += R"====(
	uniform vec3 cutPos[PLANE_COUNT];
	uniform vec3 cutNormal[PLANE_COUNT];
	uniform float outlineSize[PLANE_COUNT];
)====";

	result += "void main(){\n";

	result += R"====(
	
	for(int i=0; i<PLANE_COUNT; ++i)
	{
		if(dot(FragPos.xyz - cutPos[i], cutNormal[i])<0.0) discard;
	}
)====";

	result += API3DGenericMeshShader::GetDefaultFragmentShaderMain(R"====(
	if(!gl_FrontFacing)
	{ 
		diffuseColor = vec4(diffuseColor.xyz * 0.8, diffuseColor.w); 
		ambientColor = vec4(ambientColor.xyz * 0.8, ambientColor.w); 
		normalN = -normalN;
	}
	
	for(int i=0; i<PLANE_COUNT; ++i)
	{
		float d = dot(normalN, cutNormal[i]);
		float s = dot(FragPos.xyz - cutPos[i], cutNormal[i])/dot(cutNormal[i],cutNormal[i]);

		if(s < outlineSize[i] && d < 0.5 && d > -0.5)
		{
			diffuseColor = vec4((cutNormal[i] + vec3(1,1,1))/2.0, diffuseColor.w);
			ambientColor = vec4((cutNormal[i] + vec3(1,1,1))/2.0, ambientColor.w);
		}
	}
)====");
	result += "}\n";
	return result;
}



IMPLEMENT_CLASS_INFO(API3DCutShader)
