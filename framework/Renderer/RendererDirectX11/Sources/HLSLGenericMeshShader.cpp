#include "HLSLGenericMeshShader.h"
#include "RendererDX11.h"
#include "TravState.h"
#include "Material.h"
#include "RendererDefines.h"
#include "Camera.h"
#include "Scene3D.h"

#include <d3d11.h>

#ifdef WUP
extern bool gIsHolographic;
#endif


std::string API3DGenericMeshShader::GetDefaultLightingFunctions()
{
	return R"====(
cbuffer LightBuffer : register(b1)
{
	float3 camPos;
#if (myPointLightCount>0)
	struct PointLight
	{
		float3 position;
		float3 attenuation;
		float3 specular;
		float3 diffuse;
		float3 ambiant;
	} pointLights[myPointLightCount];
#endif
#if (myDirLightCount>0)
	struct DirLight
	{
		float3 position;
		float3 diffuse;
		float3 specular;
		float3 ambiant;
	} dirLights[myDirLightCount];
#endif
#if (mySpotLightCount>0)
	struct SpotLight
	{
		float3 position;
		float4 directionAndCutOff;
		float4 attenuationAndSpotExponent;
		float3 diffuse;
		float3 specular;
		float3 ambiant;
	} spotLights[mySpotLightCount];
#endif
};

#if (myPointLightCount>0)
	float3 CalcPointLight(PointLight light, float3 viewDir, float3 normalN, float4 diffuseColor, float4 specularColor, float4 ambientColor, float shininess, float3 FragPos){
		float3 lightDir = normalize(light.position - FragPos);
		float diff = max(dot(normalN, lightDir), 0.0); 
		float diffsign = sign(diff); 
		float3 reflectDir = reflect(-lightDir, normalN); 
		float spec = pow(max(dot(viewDir, reflectDir),0.0), shininess) * diffsign; 
		float distance = length(light.position - FragPos); 
		float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * (distance * distance) + 1.0); 
		float3 diffuse = light.diffuse * diffuseColor.xyz * diff ; 
		float3 specular = light.specular  * specularColor.xyz  * spec; 
		float3 ambiant = light.ambiant * ambientColor.xyz; 
		return (specular + diffuse + ambiant)*attenuation; 
	}
#endif
#if (myDirLightCount>0)
	float3 CalcDirLight(DirLight light, float3 viewDir, float3 normalN, float4 diffuseColor, float4 specularColor, float4 ambientColor, float shininess, float3 FragPos){
		float3 lightDir = (light.position); // already normalized CPU side
		float diff = max(dot(normalN, lightDir), 0.0); 
		float diffsign = sign(diff); 
		float3 reflectDir = reflect(-lightDir, normalN); 
		float spec = pow(max(dot(viewDir, reflectDir),0.0), shininess) * diffsign; 
		float3 diffuse = light.diffuse  * diffuseColor.xyz * diff; 
		float3 specular = light.specular * specularColor.xyz  * spec; 
		float3 ambiant = light.ambiant * ambientColor.xyz; 
		return (ambiant + diffuse + specular); 
	}
#endif
#if (mySpotLightCount>0)
	float3 CalcSpotLight(SpotLight light, float3 viewDir, float3 normalN, float4 diffuseColor, float4 specularColor, float4 ambientColor, float shininess, float3 FragPos){
		float3 lightDir = normalize(light.position - FragPos); 
		float diff = max(dot(normalN, lightDir), 0.0); 
		float diffsign = sign(diff); 
		float3 reflectDir = reflect(-lightDir, normalN);
		float spec = pow(max(dot(viewDir, reflectDir),0.0), shininess) * diffsign; 
		float distance = length(light.position - FragPos); 
		float spot_factor =  dot(normalize(light.directionAndCutOff.xyz), -lightDir);
		float spotEffect = 0.0;
		if(spot_factor >= light.directionAndCutOff.w)
		{
			spotEffect = pow(spot_factor, light.attenuationAndSpotExponent.w);
		}
		float attenuation = spotEffect / (light.attenuationAndSpotExponent.x + light.attenuationAndSpotExponent.y * distance + light.attenuationAndSpotExponent.z * (distance * distance) + 1.0); 
		float3 diffuse = light.diffuse * diffuseColor.xyz * diff; 
		float3 specular = light.specular * specularColor.xyz* spec; 
		float3 ambiant = light.ambiant * ambientColor.xyz; 
		return (specular + diffuse + ambiant)*attenuation; 
	}
#endif
)====";
}

std::string API3DGenericMeshShader::GetDefaultVaryings()
{
	return R"====(
struct VS_OUTPUT
{
    float4 Position: SV_POSITION;
#ifdef CLIENT_STATE_TEXTURE_COORD_ARRAY0 
	float2 Texcoord: TEXCOORD0;
#endif
#ifdef CLIENT_STATE_TANGENT_ARRAY 
	float3 Tangent : TANGENT;
#endif
#ifdef CLIENT_STATE_COLOR_ARRAY 
	float4 Color : COLOR;
#endif
#ifdef CLIENT_STATE_NORMAL_ARRAY 
	float3 Normal : NORMAL;
#endif
#ifdef CLIENT_STATE_FOG
	float OneOnFogScale : FOG;
#endif
	float3 FragPos : POSITION;
#ifdef HOLOGRAPHIC 
	uint viewId  : SV_RenderTargetArrayIndex;
#endif
#ifdef CLIENT_ISFRONTFACE
	bool IsFrontFace : SV_IsFrontFace;
#endif

};
)====";
}

std::string API3DGenericMeshShader::GetDefaultFragmentShaderUniforms()
{
	return R"====(
	SamplerState SampleType;
#ifdef CLIENT_STATE_TEXTURE_COORD_ARRAY0 
	Texture2D s0_colorMap;
#endif

#ifdef CLIENT_STATE_TANGENT_ARRAY 
	Texture2D s1_colorMap;
cbuffer TangentBuffer
{
	float tangent_space_lod;
};
#endif
#ifdef CLIENT_STATE_FOG
cbuffer FogBuffer : register(b3)
{
	float4 fog_color;
	float far_plane;
	float fog_scale;
};
#endif
cbuffer MaterialBuffer : register(b2)
{
	float4 MaterialDiffuseColor;
	float4 MaterialSpecularColor; 
	float4 MaterialAmbiantColor; 
	float MaterialShininess; 
};
)====";
}

std::string API3DGenericMeshShader::GetDefaultFragmentShaderMain(const std::string& inject_before_light_calcs, const std::string& inject_top_of_main)
{
	return R"====(	
float4 main(VS_OUTPUT input) : SV_TARGET
{
)====" + inject_top_of_main + R"====(
	float3 normalN;
#ifdef CLIENT_STATE_TANGENT_ARRAY 
	// retreive texture as early as possible
	normalN = s1_colorMap.Sample(SampleType, input.Texcoord).rgb;
#endif
#ifdef CLIENT_STATE_TEXTURE_COORD_ARRAY0 
	// retreive texture as early as possible
	float4 texel = s0_colorMap.Sample(SampleType, input.Texcoord); 
#endif
	float4 result = float4(0, 0, 0, 0);
	float3 viewDir = camPos - input.FragPos;

#ifdef CLIENT_STATE_FOG
	float dist = length(viewDir);
	float fog_coef =  (dist - (far_plane - fog_scale)) * input.OneOnFogScale;
	viewDir = viewDir * (1.0/dist);
#else
	viewDir = normalize(viewDir);
#endif

#ifdef NOLIGHT 
	float4 diffuseColor = float4(1, 1, 1, 1);
	float4 ambientColor = float4(1, 1, 1, 1);
	float4 specularColor = float4(1, 1, 1, 1);
	float shininess =  1.0;
#else
	float4 diffuseColor = MaterialDiffuseColor;
	float4 ambientColor = MaterialAmbiantColor;
	float4 specularColor = MaterialSpecularColor;
	float shininess = MaterialShininess;
#endif

#ifdef CLIENT_STATE_COLOR_ARRAY 
	diffuseColor *= input.Color;
	ambientColor *= input.Color;
#endif
#ifdef CLIENT_STATE_TANGENT_ARRAY 
	float3 nT = normalize(input.Tangent);
	float3 nN = normalize(input.Normal);
	normalN = (normalN * 2.0 - 1.0);
	float3 B = (cross(nN, nT));
	float3x3 TBN = float3x3(nT, B, nN);
	//TBN = transpose(TBN);
	normalN = mul(normalN, TBN);
	//normalN = normalize(lerp(input.Normal, normalN, tangent_space_lod));
#elif defined(CLIENT_STATE_NORMAL_ARRAY)
	normalN = normalize(input.Normal);
#endif
#ifdef CLIENT_STATE_TEXTURE_COORD_ARRAY0 
	diffuseColor *= texel;
	ambientColor *= texel;
#endif
)===="+ inject_before_light_calcs +R"====(
#ifdef NOLIGHT 
	result = diffuseColor;
#else
#if(myDirLightCount>0)
	for (int i = 0; i <myDirLightCount; i++)
		result.xyz += CalcDirLight(dirLights[i], viewDir, normalN, diffuseColor, specularColor, ambientColor, shininess, input.FragPos);
#endif
#if(mySpotLightCount>0)
	for (int i = 0; i <mySpotLightCount; i++)
		result.xyz += CalcSpotLight(spotLights[i], viewDir, normalN, diffuseColor, specularColor, ambientColor, shininess, input.FragPos);
#endif
#if(myPointLightCount>0)
	for (int i = 0; i <myPointLightCount; i++)
		result.xyz += CalcPointLight(pointLights[i], viewDir, normalN, diffuseColor, specularColor, ambientColor, shininess, input.FragPos);
#endif
	result.w = diffuseColor.w;
#endif // NOLIGHT 
#ifdef CLIENT_STATE_ALPHA_TEST_LOW
	if(result.a < 0.01)
		discard;
#endif
#ifdef CLIENT_STATE_ALPHA_TEST_HIGH
	if(result.a < 0.95)
		discard;
#endif
#ifdef CLIENT_STATE_FOG
	result = float4(lerp(result.xyz, fog_color.xyz, clamp(fog_coef, 0.0, 1.0)), result.w);
#endif
#if defined(DRAW_NORMALS) 
#if defined(CLIENT_STATE_NORMAL_ARRAY)
	result = float4((normalN+float3(1,1,1))*0.5, result.w);	
#else
	result = float4(0.5, 0.5, 0.5, result.w);	
#endif
#elif defined(DRAW_UVS)
#if defined(CLIENT_STATE_TEXTURE_COORD_ARRAY0)
	result = float4(input.Texcoord, 0.0, result.w);
#else
	result = float4(0.5, 0.5, 0.5, result.w);
#endif
#endif
    return result;
}
	)====";
}

std::string API3DGenericMeshShader::GetDefaultVertexShaderUniforms()
{
	return R"====(
cbuffer MatrixBuffer : register(b0)
{
	row_major float4x4 model_matrix;
	column_major float4x4    uv_matrix;
#ifdef HOLOGRAPHIC
	row_major float4x4 stereo_viewproj[2];
#else
	row_major float4x4 viewproj_matrix;
	//row_major float4x4 view_matrix;
	//row_major float4x4 proj_matrix;
#endif


};
#ifdef CLIENT_STATE_FOG
cbuffer FogBuffer : register(b3)
{
	float4 fog_color;
	float far_plane;
	float fog_scale;
};
#endif
)====";
}

std::string API3DGenericMeshShader::GetDefaultVertexShaderAttributes()
{
	return R"====(
struct VS_INPUT
{
	float3 attrib_vertex : POSITION;
#ifdef CLIENT_STATE_NORMAL_ARRAY 
	float3 attrib_normal : NORMAL;
#endif
#ifdef CLIENT_STATE_COLOR_ARRAY 
	float4 attrib_color : COLOR;
#endif
#ifdef CLIENT_STATE_TEXTURE_COORD_ARRAY0 
	float2 attrib_texcoord : TEXCOORD0;
#endif
#ifdef CLIENT_STATE_TANGENT_ARRAY 
	float3 attrib_tangent : TANGENT;
#endif
#ifdef CLIENT_STATE_INSTANCED
	row_major float3x4 attrib_instance_matrix : INSTANCE_TRANSFORM;
#endif
#ifdef HOLOGRAPHIC
	uint instId  : SV_InstanceID;
#endif
};
)====";
}

std::string API3DGenericMeshShader::GetDefaultVertexShaderMain()
{
	return R"====(
VS_OUTPUT main(VS_INPUT input) 
{
	VS_OUTPUT output;
#ifdef CLIENT_STATE_INSTANCED
	float4x4 actual_model_matrix =	float4x4(	float4(input.attrib_instance_matrix[0].xyz, 0), 
										float4(input.attrib_instance_matrix[0][3], input.attrib_instance_matrix[1][0], input.attrib_instance_matrix[1][1], 0),
										float4(input.attrib_instance_matrix[1][2], input.attrib_instance_matrix[1][3], input.attrib_instance_matrix[2][0], 0),
										float4(input.attrib_instance_matrix[2][1], input.attrib_instance_matrix[2][2], input.attrib_instance_matrix[2][3], 1));
#else
	float4x4 actual_model_matrix = model_matrix;
#endif
#ifdef CLIENT_STATE_FOG
	output.OneOnFogScale=1.0/fog_scale;
#endif
#ifdef CLIENT_STATE_TEXTURE_COORD_ARRAY0 
    float2 uvtmp = mul(float4(input.attrib_texcoord,1.0,0.0),uv_matrix).xy;
	output.Texcoord = uvtmp.xy;
#endif
#ifdef CLIENT_STATE_TANGENT_ARRAY 
	output.Tangent = normalize(mul(float4(input.attrib_tangent, 0.0), actual_model_matrix)).xyz;
#endif
#ifdef CLIENT_STATE_NORMAL_ARRAY 
	output.Normal = normalize(mul(float4(input.attrib_normal, 0.0), actual_model_matrix)).xyz;
#endif
#ifdef CLIENT_STATE_COLOR_ARRAY 
	output.Color = input.attrib_color;
#endif
	float4 model_transformed_position = mul(float4(input.attrib_vertex, 1.0), actual_model_matrix);
	output.FragPos = model_transformed_position.xyz;
#ifdef HOLOGRAPHIC
	int idx = input.instId % 2;
	output.Position = mul(model_transformed_position, stereo_viewproj[idx]);
	output.viewId = idx;
#else
	output.Position = mul(model_transformed_position, viewproj_matrix);
	//output.Position = mul(model_transformed_position, mul(view_matrix, proj_matrix));
#endif
    return output;
}
)====";
}

std::string API3DGenericMeshShader::GetFragmentShader()
{
	std::string result = API3DGenericMeshShader::GetDefaultVaryings();
	result += API3DGenericMeshShader::GetDefaultFragmentShaderUniforms();
	result += API3DGenericMeshShader::GetDefaultLightingFunctions();

	//result += "void main(){\n";
	result += API3DGenericMeshShader::GetDefaultFragmentShaderMain();
	//result += "}\n";
	return result;
}

std::string API3DGenericMeshShader::GetVertexShader()
{
	std::string result = API3DGenericMeshShader::GetDefaultVertexShaderAttributes();
	result += API3DGenericMeshShader::GetDefaultVertexShaderUniforms();
	result += API3DGenericMeshShader::GetDefaultVaryings();

	//result += "void main(){\n";
	result += API3DGenericMeshShader::GetDefaultVertexShaderMain();
	//result += "}\n";
	return result;
}

IMPLEMENT_CLASS_INFO(API3DGenericMeshShader)

API3DGenericMeshShader::API3DGenericMeshShader(const std::string& name, CLASS_NAME_TREE_ARG) : API3DShader(name, PASS_CLASS_NAME_TREE_ARG)
{
	misGeneric = true;
	museGenericLight = true;
}

void	API3DGenericMeshShader::InitModifiable()
{
	if (!IsInit())
	{
		API3DShader::InitModifiable();
	}
}

void	API3DGenericMeshShader::ChooseShader(TravState* state, unsigned int attribFlag)
{
	auto renderer = state->GetRenderer();
	ShaderBase*	active = renderer->GetActiveShader();

	if (active)
	{
		if (!active->isGeneric()) // not a generic shader, just return
			return;
	}

	// get shader flag
	unsigned int flag = attribFlag;

	auto cam = state->GetCurrentCamera();
	bool fog = false;
	if (cam && cam->getValue("UseFog", fog) && fog)
	{
		flag |= ModuleRenderer::ShaderFlags::FOG;
	}

	if (state->mDrawingInstances)
	{
		flag |= ModuleRenderer::ShaderFlags::INSTANCED;
	}

	if (/*gIsHolographic && */state->GetHolographicMode())
		flag |= ModuleRenderer::ShaderFlags::HOLOGRAPHIC;

	// then suppose 4 bits per light type for light count
	auto scene = state->GetScene();
	int mySpotLightCount = scene ? scene->getSpotLightCount() : 0;
	int myDirLightCount = scene ? scene->getDirLightCount() : 0;
	int myPointLightCount = scene ? scene->getPointLightCount() : 0;

	flag = CustomizeShaderFlag(state, attribFlag, flag);

	if ((flag&ModuleRenderer::NO_LIGHT_MASK) != ModuleRenderer::NO_LIGHT_MASK)
	{
		flag |= mySpotLightCount << 20;
		flag |= myDirLightCount << 24;
		flag |= myPointLightCount << 28;
	}

	BuildShaderStruct *	choosed=nullptr;

	// check for already compiled shader
	std::map<unsigned int, BuildShaderStruct*>::iterator itchoosed = mShaderSourceMap.find(flag);
	if (itchoosed != mShaderSourceMap.end())
	{
		choosed = (itchoosed->second);
	}
	else // build a new one
	{
		std::string defines;
		defines.reserve(1024);

		if (flag & ModuleRenderer::ShaderFlags::FOG)
			defines += "#define CLIENT_STATE_FOG\n";

		if (flag & ModuleRenderer::ShaderFlags::HOLOGRAPHIC)
			defines += "#define HOLOGRAPHIC\n";

		if (flag & ModuleRenderer::DRAW_NORMALS)
			defines += "#define DRAW_NORMALS\n";
		else if(flag & ModuleRenderer::DRAW_UVS)
			defines += "#define DRAW_UVS\n";

		if (flag&ModuleRenderer::NO_LIGHT_MASK)
		{
			defines += "#define NOLIGHT\n";
			defines += "#define mySpotLightCount 0\n";
			defines += "#define myDirLightCount 0\n";
			defines += "#define myPointLightCount 0\n";
		}
		else
		{
			char printbuffer[512];
			sprintf(printbuffer, "#define mySpotLightCount %d\n #define myDirLightCount %d\n #define myPointLightCount %d\n", mySpotLightCount, myDirLightCount, myPointLightCount);
			defines += printbuffer;
		}

		if (flag & ModuleRenderer::INSTANCED)
			defines += "#define CLIENT_STATE_INSTANCED\n";

		if (flag&ModuleRenderer::TEXCOORD_ARRAY_MASK)
			defines += "#define CLIENT_STATE_TEXTURE_COORD_ARRAY0\n";

		if (flag&ModuleRenderer::TEXCOORD_ARRAY_MASK1)
			defines += "#define CLIENT_STATE_TEXTURE_COORD_ARRAY1\n";

		if (flag&ModuleRenderer::TEXCOORD_ARRAY_MASK2)
			defines += "#define CLIENT_STATE_TEXTURE_COORD_ARRAY2\n";

		if (flag&ModuleRenderer::TEXCOORD_ARRAY_MASK3)
			defines += "#define CLIENT_STATE_TEXTURE_COORD_ARRAY3\n";

		if (flag&ModuleRenderer::TANGENT_ARRAY_MASK)
			defines += "#define CLIENT_STATE_TANGENT_ARRAY\n";

		if (flag&ModuleRenderer::COLOR_ARRAY_MASK)
			defines += "#define CLIENT_STATE_COLOR_ARRAY\n";

		if (flag&ModuleRenderer::NORMAL_ARRAY_MASK)
			defines += "#define CLIENT_STATE_NORMAL_ARRAY\n";

		if (flag&ModuleRenderer::ALPHA_TEST_LOW)
			defines += "#define CLIENT_STATE_ALPHA_TEST_LOW\n";
		else if (flag&ModuleRenderer::ALPHA_TEST_HIGH)
			defines += "#define CLIENT_STATE_ALPHA_TEST_HIGH\n";
		
		mVertexShaderText = defines + GetVertexShader();
		mFragmentShaderText = defines + GetFragmentShader();
		auto geo_shader_txt = GetGeometryShader();
		if (geo_shader_txt.size())
			mGeometryShaderText = defines + geo_shader_txt;
		
		BuildShaderStruct* toAdd = Rebuild();

		mVertexShaderText = "";
		mFragmentShaderText = "";
		mGeometryShaderText = "";

		insertBuildShader(flag, toAdd);
		
		setCurrentBuildShader(flag);
		/*	if(renderer->GetActiveShader()==this)
			Active(state);*/
	}

	if (choosed && (choosed->mShaderProgram == active->GetCurrentShaderProgram()))
	{
		// nothing else to do
	}
	else
	{
		if (active)
		{
			renderer->popShader(active, state);
		}

		if (choosed/* && (((HLSLProgramInfo*)choosed->mShaderProgram)->mID != 1)*/)
		{
			setCurrentBuildShader(flag);
		}

		renderer->pushShader(this, state);
	}
}

void	API3DGenericMeshShader::Dealloc()
{
}