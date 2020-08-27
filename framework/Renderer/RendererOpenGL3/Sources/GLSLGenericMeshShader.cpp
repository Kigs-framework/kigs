#include "GLSLGenericMeshShader.h"
#include "RendererOpenGL.h"
#include "TravState.h"
#include "Material.h"
#include "RendererDefines.h"
#include "Camera.h"
#include "Platform/Renderer/OpenGLInclude.h"
#include "Scene3D.h"

#ifdef WUP
extern bool gIsHolographic;
#endif



std::string API3DGenericMeshShader::GetDefaultLightingFunctions()
{
	return R"====(
#if (myPointLightCount>0)
	struct PointLight
	{
		vec3 position;
		vec3 attenuation;
		vec3 specular;
		vec3 diffuse;
		vec3 ambiant;
	};
	uniform PointLight pointLights[myPointLightCount];
	vec3 CalcPointLight(PointLight light, vec3 viewDir, vec3 normalN, vec4 diffuseColor, vec4 specularColor, vec4 ambientColor, float shininess){
		vec3 lightDir = normalize(light.position - FragPos);
		float diff = max(dot(normalN, lightDir), 0.0); 
		float diffsign = sign(diff); 
		vec3 reflectDir = reflect(-lightDir, normalN); 
		float spec = pow(max(dot(viewDir, reflectDir),0.0), shininess) * diffsign; 
		float distance = length(light.position - FragPos); 
		float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * (distance * distance) + 1.0); 
		vec3 diffuse = light.diffuse * diffuseColor.xyz * diff ; 
		vec3 specular = light.specular  * specularColor.xyz  * spec; 
		vec3 ambiant = light.ambiant * ambientColor.xyz; 
		return (specular + diffuse + ambiant)*attenuation; 
	}
#endif
#if (myDirLightCount>0)
	struct DirLight
	{
		vec3 position;
		vec3 diffuse;
		vec3 specular;
		vec3 ambiant;
	};
	uniform DirLight dirLights[myDirLightCount];
	vec3 CalcDirLight(DirLight light, vec3 viewDir, vec3 normalN, vec4 diffuseColor, vec4 specularColor, vec4 ambientColor, float shininess){
		vec3 lightDir = (light.position); // already normalized CPU side
		float diff = max(dot(normalN, lightDir), 0.0); 
		float diffsign = sign(diff); 
		vec3 reflectDir = reflect(-lightDir, normalN); 
		float spec = pow(max(dot(viewDir, reflectDir),0.0), shininess) * diffsign; 
		vec3 diffuse = light.diffuse  * diffuseColor.xyz * diff; 
		vec3 specular = light.specular * specularColor.xyz  * spec; 
		vec3 ambiant = light.ambiant * ambientColor.xyz; 
		return (ambiant + diffuse + specular); 
	}
#endif
#if (mySpotLightCount>0)
	struct SpotLight
	{
		vec3 position;
		vec3 direction;
		float cutOff;
		float spotExponent;
		vec3 attenuation;
		vec3 diffuse;
		vec3 specular;
		vec3 ambiant;
	};
	uniform SpotLight spotLights[mySpotLightCount];
	vec3 CalcSpotLight(SpotLight light, vec3 viewDir, vec3 normalN, vec4 diffuseColor, vec4 specularColor, vec4 ambientColor, float shininess){
		vec3 lightDir = normalize(light.position - FragPos); 
		float diff = max(dot(normalN, lightDir), 0.0); 
		float diffsign = sign(diff); 
		vec3 reflectDir = reflect(-lightDir, normalN);
		float spec = pow(max(dot(viewDir, reflectDir),0.0), shininess) * diffsign; 
		float distance = length(light.position - FragPos); 
		float spot_factor =  dot(normalize(light.direction), -lightDir);
		float spotEffect = 0.0;
		if(spot_factor >= light.cutOff)
		{
			spotEffect = pow(spot_factor, light.spotExponent);
		}
		float attenuation = spotEffect / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * (distance * distance) + 1.0); 
		vec3 diffuse = light.diffuse * diffuseColor.xyz * diff; 
		vec3 specular = light.specular * specularColor.xyz* spec; 
		vec3 ambiant = light.ambiant * ambientColor.xyz; 
		return (specular + diffuse + ambiant)*attenuation; 
	}
#endif
)====";
}

std::string API3DGenericMeshShader::GetDefaultVaryings()
{
	return R"====(
#ifdef CLIENT_STATE_TEXTURE_COORD_ARRAY0 
	varying vec2 vTexcoord;
#endif
#ifdef CLIENT_STATE_TANGENT_ARRAY 
	varying vec3 T;
#endif
#ifdef CLIENT_STATE_COLOR_ARRAY 
	varying vec4 vColor;
#endif
#ifdef CLIENT_STATE_NORMAL_ARRAY 
	varying vec3 N;
#endif
#ifdef HOLOGRAPHIC 
	varying float vRenderTargetArrayIndex;
#endif
#ifdef CLIENT_STATE_FOG
	varying float vOneOnFogScale;
#endif

	varying vec3 FragPos;
)====";
}

std::string API3DGenericMeshShader::GetDefaultFragmentShaderUniforms()
{
	return R"====(
	#ifdef CLIENT_STATE_TEXTURE_COORD_ARRAY0 
	uniform sampler2D s0_colorMap;
	#endif
	#ifdef CLIENT_STATE_TANGENT_ARRAY 
	uniform sampler2D s1_colorMap;
	uniform float tangent_space_lod;
	#endif
	#ifdef CLIENT_STATE_FOG
	uniform vec4 fog_color;
	uniform float far_plane;
	uniform float fog_scale;
	#endif

	uniform vec4 MaterialDiffuseColor;
	uniform vec4 MaterialSpecularColor; 
	uniform vec4 MaterialAmbiantColor; 
	uniform float MaterialShininess; 
	uniform vec3 camPos;
)====";
}

std::string API3DGenericMeshShader::GetDefaultFragmentShaderMain(const std::string& inject_before_light_calcs)
{
	return R"====(
	
	vec3 normalN;
#ifdef CLIENT_STATE_TANGENT_ARRAY 
	// retreive texture as early as possible
	normalN = texture2D(s1_colorMap, vTexcoord).rgb;
#endif
#ifdef CLIENT_STATE_TEXTURE_COORD_ARRAY0 
	// retreive texture as early as possible
	vec4 texel = texture2D(s0_colorMap, vTexcoord);
#endif
	vec4 result = vec4(0, 0, 0, 0);
	vec3 viewDir = camPos - FragPos;

#ifdef CLIENT_STATE_FOG
	float dist = length(viewDir);
	float fog_coef =  (dist - (far_plane - fog_scale)) *vOneOnFogScale;
	viewDir = viewDir* (1.0/dist);
#else
	viewDir = normalize(viewDir);
#endif

	
#ifdef NOLIGHT 
	vec4 diffuseColor = vec4(1, 1, 1, 1);
	vec4 ambientColor = vec4(1, 1, 1, 1);
	vec4 specularColor = vec4(1, 1, 1, 1);
	float shininess =  1.0;
#else
	vec4 diffuseColor = MaterialDiffuseColor;
	vec4 ambientColor = MaterialAmbiantColor;
	vec4 specularColor = MaterialSpecularColor;
	float shininess = MaterialShininess;
#endif

#ifdef CLIENT_STATE_COLOR_ARRAY 
	diffuseColor *= vColor;
	ambientColor *= vColor;
#endif
#ifdef CLIENT_STATE_TANGENT_ARRAY 
    
	vec3 nT = normalize(T);
	vec3 nN = normalize(N);
	normalN = (normalN * 2.0 - 1.0);
	vec3 B = (cross(nN, nT));
	mat3 TBN = mat3(nT, B, nN);
	normalN = (TBN * normalN);
	//normalN = normalize(mix(N, normalN, tangent_space_lod));
	
#elif defined(CLIENT_STATE_NORMAL_ARRAY)
	normalN = normalize(N);
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
		result.xyz += CalcDirLight(dirLights[i], viewDir, normalN, diffuseColor, specularColor, ambientColor, shininess);
#endif
#if(mySpotLightCount>0)
	for (int i = 0; i <mySpotLightCount; i++)
		result.xyz += CalcSpotLight(spotLights[i], viewDir, normalN, diffuseColor, specularColor, ambientColor, shininess);
#endif
#if(myPointLightCount>0)
	for (int i = 0; i <myPointLightCount; i++)
		result.xyz += CalcPointLight(pointLights[i], viewDir, normalN, diffuseColor, specularColor, ambientColor, shininess);
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
	result = vec4(mix(result.xyz, fog_color.xyz, clamp(fog_coef,0.0,1.0)), result.w);
#endif
#if defined(DRAW_NORMALS) 
#if defined(CLIENT_STATE_NORMAL_ARRAY)
	gl_FragColor = vec4((normalN+vec3(1,1,1))*0.5, result.w);	
#else
	gl_FragColor = vec4(0.5, 0.5, 0.5, result.w);	
#endif
#elif defined(DRAW_UVS)
#if defined(CLIENT_STATE_TEXTURE_COORD_ARRAY0)
	gl_FragColor = vec4(vTexcoord, 0.0, result.w);
#else
	gl_FragColor = vec4(0.5, 0.5, 0.5, result.w);
#endif
#else
	gl_FragColor = result;
#endif
#ifdef HOLOGRAPHIC 
	float index = vRenderTargetArrayIndex;
#endif
	)====";
}

std::string API3DGenericMeshShader::GetDefaultVertexShaderUniforms()
{
	return R"====(
	uniform mat4	model_matrix;
	#ifdef HOLOGRAPHIC
	uniform mat4 uHolographicViewProjectionMatrix[2]; 
	#else
	uniform mat4	view_matrix;
	uniform mat4	proj_matrix;
	#endif
	uniform vec3 camPos;
	#ifdef CLIENT_STATE_FOG
	uniform float far_plane;
	uniform float fog_scale;
	#endif
)====";
}

std::string API3DGenericMeshShader::GetDefaultVertexShaderAttributes()
{
	return R"====(
	attribute vec3 attrib_vertex;
#ifdef HOLOGRAPHIC
	attribute float	aRenderTargetArrayIndex;
#endif
#ifdef CLIENT_STATE_NORMAL_ARRAY 
	attribute vec3 attrib_normal;
#endif
#ifdef CLIENT_STATE_COLOR_ARRAY 
	attribute vec4 attrib_color;
#endif
#ifdef CLIENT_STATE_TEXTURE_COORD_ARRAY0 
	attribute vec2 attrib_texcoord;
#endif
#ifdef CLIENT_STATE_TANGENT_ARRAY 
	attribute vec3 attrib_tangent;
#endif
#ifdef CLIENT_STATE_INSTANCED
	attribute mat4 attrib_instance_matrix;
#endif
)====";
}

std::string API3DGenericMeshShader::GetDefaultVertexShaderMain()
{
	return R"====(
	mat4 actual_model_matrix = model_matrix;
#ifdef CLIENT_STATE_INSTANCED
	actual_model_matrix = mat4(	vec4(attrib_instance_matrix[0].xyz,0), 
								vec4(attrib_instance_matrix[0][3],attrib_instance_matrix[1][0],attrib_instance_matrix[1][1],0),
								vec4(attrib_instance_matrix[1][2],attrib_instance_matrix[1][3],attrib_instance_matrix[2][0],0),
								vec4(attrib_instance_matrix[2][1],attrib_instance_matrix[2][2],attrib_instance_matrix[2][3],1));
#endif
#ifdef CLIENT_STATE_FOG
	vOneOnFogScale=1.0/fog_scale;
#endif
#ifdef CLIENT_STATE_TEXTURE_COORD_ARRAY0 
	vTexcoord = attrib_texcoord;
#endif
#ifdef CLIENT_STATE_TANGENT_ARRAY 
	T = normalize(actual_model_matrix * vec4(attrib_tangent, 0.0)).xyz;
#endif
#ifdef CLIENT_STATE_NORMAL_ARRAY 
	N = normalize(actual_model_matrix * vec4(attrib_normal, 0.0)).xyz;
#endif
#ifdef CLIENT_STATE_COLOR_ARRAY 
	vColor = attrib_color;
#endif
	FragPos = vec3(actual_model_matrix * vec4(attrib_vertex, 1.0));
#ifdef HOLOGRAPHIC
	int arrayIndex = int(aRenderTargetArrayIndex);
	vRenderTargetArrayIndex = aRenderTargetArrayIndex;
	gl_Position = uHolographicViewProjectionMatrix[arrayIndex] * actual_model_matrix * vec4(attrib_vertex, 1.0);
#else
	mat4 viewmodel = view_matrix*actual_model_matrix;
	gl_Position = proj_matrix * viewmodel * vec4(attrib_vertex, 1.0);
#endif
)====";
}

std::string API3DGenericMeshShader::GetFragmentShader()
{
	std::string result = API3DGenericMeshShader::GetDefaultVaryings();
	result += API3DGenericMeshShader::GetDefaultFragmentShaderUniforms();
	result += API3DGenericMeshShader::GetDefaultLightingFunctions();

	result += "void main(){\n";
	result += API3DGenericMeshShader::GetDefaultFragmentShaderMain();
	result += "}\n";
	return result;
}

std::string API3DGenericMeshShader::GetVertexShader()
{
	std::string result = API3DGenericMeshShader::GetDefaultVertexShaderAttributes();
	result += API3DGenericMeshShader::GetDefaultVertexShaderUniforms();
	result += API3DGenericMeshShader::GetDefaultVaryings();

	result += "void main(){\n";
	result += API3DGenericMeshShader::GetDefaultVertexShaderMain();
	result += "}\n";
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
	RendererOpenGL* renderer = (RendererOpenGL*)state->GetRenderer();
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


#ifdef WUP
	if (state->GetHolographicMode())
		flag |= ModuleRenderer::ShaderFlags::HOLOGRAPHIC;
#endif

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

	BuildShaderStruct*	choosed = nullptr;

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

		defines += R"====(
#ifdef GL_ES
#ifdef GL_FRAGMENT_PRECISION_HIGH
	precision highp float;
	precision highp int;
	precision mediump  sampler2D;
#else
	precision mediump float;
	precision mediump int;
	precision mediump  sampler2D;
#endif
#endif
)====";


		mVertexShaderText = defines + GetVertexShader();
		mFragmentShaderText = defines + GetFragmentShader();

		BuildShaderStruct* toAdd = Rebuild();

		mVertexShaderText = "";
		mFragmentShaderText = "";

		insertBuildShader(flag, toAdd);

		setCurrentBuildShader(flag);

		Active(state);
		// add child uniform as default uniform
		std::vector<CMSP> instances;
		CoreModifiable::GetSonInstancesByType("API3DUniformBase", instances);

		if (instances.size())
		{
			std::vector<CMSP>::iterator itr = instances.begin();
			std::vector<CMSP>::iterator end = instances.end();
			for (; itr != end; ++itr)
			{
				PushUniform(static_cast<API3DUniformBase*>((*itr).get()));
			}
		}
		Deactive(0);
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

		if (choosed && (choosed->mShaderProgram->mID != (unsigned int)-1))
		{
			setCurrentBuildShader(flag);
		}

		renderer->pushShader(this, state);
	}
}

void	API3DGenericMeshShader::Dealloc()
{

}