#include "GLSLUIShader.h"


kstl::string API3DUIShader::GetFragmentShader()
{

	std::string result = mBGR ? "#define CLIENT_STATE_BGR_TEXTURE\n" : "";
	result += R"====(
#ifdef CLIENT_STATE_TEXTURE_COORD_ARRAY0
uniform sampler2D colorMap;
varying vec2 vTexcoord;
#endif
#ifdef CLIENT_STATE_COLOR_ARRAY
varying vec4 vColor;
#endif
#ifdef HOLOGRAPHIC
varying float vRenderTargetArrayIndex;
#endif
void main()
{
#ifdef CLIENT_STATE_TEXTURE_COORD_ARRAY0
	vec4 tex = texture2D(colorMap, vTexcoord);
#ifdef CLIENT_STATE_BGR_TEXTURE
	tex.rgb = tex.bgr;
#endif
#else
	vec4 tex = vec4(1);
#endif

#ifdef CLIENT_STATE_COLOR_ARRAY
	vec4 col = vColor;
#else
	vec4 col = vec4(1);
#endif

	gl_FragColor = tex*col;
#ifdef HOLOGRAPHIC
	float index = vRenderTargetArrayIndex;
#endif

#ifdef CLIENT_STATE_ALPHA_TEST_LOW
	if(gl_FragColor.a < 0.01)
		discard;
#endif

#ifdef CLIENT_STATE_ALPHA_TEST_HIGH
	if(gl_FragColor.a < 0.95)
		discard;
#endif
}
)====";
	return result;
}

kstl::string API3DUIShader::GetVertexShader()
{
	return  R"====(
uniform mat4 model_matrix;
attribute vec2 attrib_vertex;
#ifdef HOLOGRAPHIC
uniform mat4 uHolographicViewProjectionMatrix[2]; 
attribute float	aRenderTargetArrayIndex; 
varying float		vRenderTargetArrayIndex; 
#else
uniform mat4	view_matrix;
uniform mat4	proj_matrix;
#endif
#ifdef CLIENT_STATE_TEXTURE_COORD_ARRAY0
attribute vec2 attrib_texcoord;
varying vec2 vTexcoord;
#endif
#ifdef CLIENT_STATE_COLOR_ARRAY
attribute vec4 attrib_color;
varying vec4 vColor;
#endif
void main()
{
#ifdef CLIENT_STATE_TEXTURE_COORD_ARRAY0
	vTexcoord = attrib_texcoord;
#endif
#ifdef CLIENT_STATE_COLOR_ARRAY
	vColor = attrib_color / 255.0;
#endif
#ifdef HOLOGRAPHIC
	int arrayIndex = int(aRenderTargetArrayIndex);
	vRenderTargetArrayIndex = aRenderTargetArrayIndex;
	gl_Position = uHolographicViewProjectionMatrix[arrayIndex] * model_matrix * vec4(attrib_vertex,1.0, 1.0);
#else
	mat4 viewmodel = view_matrix*model_matrix;
	gl_Position = proj_matrix * viewmodel * vec4(attrib_vertex, 1.0, 1.0);
#endif
}
)====";
}

IMPLEMENT_CLASS_INFO(API3DUIShader)

API3DUIShader::API3DUIShader(const kstl::string& name, CLASS_NAME_TREE_ARG) : API3DGenericMeshShader(name, PASS_CLASS_NAME_TREE_ARG)
{
	museGenericLight = false;
}

API3DUIShader::~API3DUIShader()
{
}