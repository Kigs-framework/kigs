#ifndef _GLSLLIGHT_H
#define _GLSLLIGHT_H

#include "Drawable.h"
#include "GLSLShader.h"
#include "Scene3D.h"
//#define USE_ATTFUNCTION

class Node3D;
class CoreModifiable;
class Camera;
class RendererOpenGL;

class API3DLight : public API3DShader
{
public:
	friend class RendererOpenGL;

	DECLARE_CLASS_INFO(API3DLight, API3DShader, Renderer)

	API3DLight(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	virtual bool PreRendering(RendererOpenGL * renderer, Camera * cam, Point3D & camPos);

	bool	Draw(TravState* state) override;


	Node3D* GetFather();
	bool	PreDraw(TravState*) override { return false; };
	bool	PostDraw(TravState*)  override { return false; };
	virtual void	DrawLight(TravState*);
	void PostDrawLight(TravState*);

	void SetUniformLocation(int uniform, const char* location);

	int GetTypeOfLight();
	inline int		GetPriority() const { return (int)((unsigned int)myPriority); }
	void NotifyUpdate(const unsigned int  labelid) override;

	inline void setIsOn(bool a_value) { myIsOn = a_value; }
	inline bool getIsOn() { return myIsOn; }

	inline void setIsDeffered(bool a_value) { myDefferedLight= a_value; }
	inline bool getIsDeffered() { return myDefferedLight; }

protected:
	virtual ~API3DLight();
	void	InitModifiable() override;
	
	DECLARE_METHOD(SetDiffuseColor);
	DECLARE_METHOD(SetSpecularColor);

	void SetOffset(float X, float Y, float Z);
	void SetSpotDir(float X, float Y, float Z);


	//#define USE_ATTFUNCTION
#ifdef USE_ATTFUNCTION
	DECLARE_METHOD(SetAttenuation);
	DECLARE_METHOD(GetAttenuation);
	COREMODIFIABLE_METHODS(SetDiffuseColor, SetSpecularColor, SetAttenuation, GetAttenuation);
#else
	COREMODIFIABLE_METHODS(SetDiffuseColor, SetSpecularColor);
#endif

	//! TRUE if the light is on
	maBool		myIsOn;

	//Param for uniforms
	maVect3DF  	myPosOffset;

	maVect3DF	myDiffuseColor;
	maVect3DF	myAmbiantColor;
	maVect3DF	mySpecularColor;

	maFloat		myConstAttenuation;
	maFloat		myLinAttenuation;
	maFloat		myQuadAttenuation;

	maVect3DF	mySpotDirection;
	maFloat		mySpotCutOff;
	maFloat		mySpotAttenuation;
	
	//! TRUE if the light is directional
	maBool		myIsDirectional;
	maBool		myDefferedLight;

	maUInt		myPriority;

	//0 for point, 1 for directional, 2 for spot, 3 for customShader
	maEnum<4>	myLightType;

	maReference	myPositionNode3D;

	Scene3D*			myScene;
	Node3D *			myParentNode;

	CMSP		myPositionUniform;
	CMSP		myCamPosUniform;

	CMSP		myDiffuseUniform;
	CMSP		mySpecularUniform;
	CMSP		myAmbiantUniform;

	CMSP		myAttenuationUniform;
	CMSP		mySpotDirUniform;
	CMSP		mySpotCutoffUniform;
	CMSP		mySpotExponentUniform;
	
	unsigned int		myListUpdateFrame;
};

struct LightPriorityCompare
{
	//! overload operator () for comparison
	bool operator()(const API3DLight * a1, const API3DLight * a2) const
	{
		if (a1->GetPriority() == a2->GetPriority())
			return (a1) < (a2);
		return a1->GetPriority() < a2->GetPriority();
	}
};


#define POINT_LIGHT 0
#define DIRECTIONAL_LIGHT 1
#define SPOT_LIGHT 2
#define CUSTOM_LIGHT 3


#define DIFFUSE_COLOR 0
#define SPECULAR_COLOR 1
#define POSITION_LIGHT 2
#define ATTENUATION 3
#define DIRECTION 4
#define SPOT_CUT_OFF 5
#define SPOT_EXPONENT 6
#define SPOT_DIRECTION 7
#define AMBIANT_COLOR 8


static const kstl::string PointLightShaderFrag = "// generated PointLightShaderFrag\n"
"#version 150\n"
"in vec2 TexCoords;\n"
"uniform vec3	lightPos;\n"
"uniform vec3	camPos;\n"
"uniform vec3   attenuation;\n"
"uniform vec3   light_diffuse;\n"
"uniform vec3   light_specular;\n"
"uniform sampler2D s1_ColorMap;\n"
"uniform sampler2D s2_NormalMap;\n"
"uniform sampler2D s3_PositionMap;\n"
"uniform sampler2D s4_SpecularMap;\n"
"uniform sampler2D s5_AmbiantMap;\n"

"void main()\n"
"{\n"
"vec4 Color = texture(s1_ColorMap, TexCoords.st);\n"
"vec4 UniNormal = texture(s2_NormalMap, TexCoords.st);\n"
"vec4 Position = texture(s3_PositionMap, TexCoords.st);\n\n"

"vec3 lightDir = lightPos - Position.xyz;\n"
"float dist = length(lightDir);\n"
"vec3 lightDirN = lightDir / dist;\n\n"

"float diff = max(dot(UniNormal.xyz, lightDirN), 0.0); \n"
"if (diff <= 0.0)\n"
"	discard;\n\n"

"vec4 UniSpecular = texture(s4_SpecularMap, TexCoords.st); \n"
"vec4 AmbiantColor = texture(s5_AmbiantMap, TexCoords.st);\n"

"vec3 reflectDir = normalize(reflect(-lightDirN, UniNormal.xyz)); \n"
"vec3 viewDir = normalize(camPos - Position.xyz); \n"
"float spec = max(pow(dot(viewDir, reflectDir), UniSpecular.w * 255),0); \n"
"float Attenuation = 1.0f / (attenuation.x + attenuation.y * dist + attenuation.z * (dist * dist) + 1); \n\n"
"vec3 diffuse = light_diffuse * diff * Color.xyz * Attenuation; \n"
"vec3 ambiant = light_diffuse * AmbiantColor.xyz * Attenuation; \n"
"vec3 specular = light_specular * spec * UniSpecular.xyz * Attenuation;\n\n"

"gl_FragColor = vec4((diffuse + specular + ambiant),1); \n"

"}\n";

static const kstl::string ShaderVert = ""
"#version 150\n"
"in vec3 position;\n"
"in vec2 texCoords;\n"

"out vec2 TexCoords;\n"

"void main()\n"
"{\n"
"gl_Position = vec4(position, 1.0f);\n"
"TexCoords = texCoords;\n"
"}\n";


static const kstl::string DirectionnalLightShaderFrag = ""
"#version 150\n"
"in vec2 TexCoords;\n"
"uniform vec3	lightPos;\n"
"uniform vec3	camPos;\n"
"uniform vec3    light_diffuse;\n"
"uniform vec3    light_specular;\n"
"uniform sampler2D ColorMap;\n"
"uniform sampler2D NormalMap;\n"
"uniform sampler2D PositionMap;\n"
"uniform sampler2D SpecularMap;\n"
"struct LIGHTING_RESULTS\n"
"{\n"
"	vec3 ldiffuse, lspecular;\n"
"};\n"
"void main()\n"
"{\n"
"vec4 Color = texture(ColorMap, TexCoords.st);\n"

"vec4 UniNormal = texture(NormalMap, TexCoords.st);\n"

"vec4 Position = texture(PositionMap, TexCoords.st);\n"
"vec3 lightDir = normalize(lightPos);\n"
"vec4 UniSpecular = texture(SpecularMap, TexCoords.st);\n"
"vec3 Normal = UniNormal.xyz;\n"
"LIGHTING_RESULTS results;\n"
"results.ldiffuse = vec3(0.0);\n"
"results.lspecular = vec3(0.0);\n"
"float NdotL = max(0.0, dot(Normal, lightDir));\n"
"results.ldiffuse.xyz = NdotL * light_diffuse;	\n"
"if (NdotL>0.0)\n"
"{\n"
"vec3 H = normalize(lightDir + normalize(camPos - Position.xyz));\n"
"results.lspecular = max(0.0, pow( dot(Normal, H), UniSpecular.w * 255)) * UniSpecular.xyz * light_specular;\n"
"}\n"
"gl_FragColor.xyz = Color.xyz * clamp(results.ldiffuse + results.lspecular, 0.0, 1.0);\n"
"gl_FragColor.a = 1;\n"
"}\n";


static const kstl::string SpotLightShaderFrag = ""
"in vec2 TexCoords;\n"
"uniform vec3	camPos;\n"
"uniform vec3	lightPos;\n"
"uniform vec3	spotDir;\n"
"uniform vec3   attenuation;\n"
"uniform float	spotCutoff;\n"
"uniform float	spotExponent;\n"
"uniform vec3   light_diffuse;\n"
"uniform vec3   light_specular;\n"
"uniform sampler2D s1_ColorMap;\n"
"uniform sampler2D s2_NormalMap;\n"
"uniform sampler2D s3_PositionMap;\n"
"uniform sampler2D s4_SpecularMap;\n"
"uniform sampler2D s5_AmbiantMap;\n"
"void main()\n"
"{\n"
"vec4 Color = texture(s1_ColorMap, TexCoords.st);\n"
"if (Color.a == 0)\n"
"	discard;\n\n"

"vec4 UniNormal = texture(s2_NormalMap, TexCoords.st);\n"
"if (UniNormal.a == 0)\n"
"	discard;\n\n"

"vec4 Position = texture(s3_PositionMap, TexCoords.st);\n\n"

"vec3 lightDir = lightPos - Position.xyz;\n"
"float dist = length(lightDir);\n"
"vec3 lightDirN = lightDir / dist;\n\n"

"float theta = dot(spotDir, -lightDirN); \n"
"if (theta<cos(spotCutoff))\n"
"	discard;\n\n"

"float diff = max(dot(UniNormal.xyz, lightDirN), 0.0); \n"		

"vec4 specularColor = texture(s4_SpecularMap, TexCoords.st);\n"
"vec4 AmbiantColor = texture(s5_AmbiantMap, TexCoords.st);\n"

"vec3 reflectDir = reflect(-lightDirN, UniNormal.xyz); \n"
"vec3 viewDir = normalize(camPos- Position.xyz); \n"
"float spec = max(pow(dot(viewDir, reflectDir), specularColor.w * 255), 0.0); \n"
"float distance = length(lightPos - Position.xyz); \n"
"float spotEffect = pow(theta, spotExponent); \n"
"float Attenuation = spotEffect / (attenuation.x + attenuation.y * distance + attenuation.z * (distance * distance) + 1); \n"

"vec3 diffuse = light_diffuse * diff * Color.xyz * Attenuation; \n"
"vec3 ambiant = light_diffuse * diff * AmbiantColor.xyz * Attenuation; \n"
"vec3 specular = light_specular * spec * specularColor.xyz * Attenuation; \n"
"gl_FragColor = vec4((diffuse + specular + ambiant),1); \n"
"}\n";




#endif //_GLSLLIGHT_H
