#include "HLSLUIShader.h"
#include "RendererDX11.h"

using namespace Kigs::Draw;


std::string API3DUIShader::GetFragmentShader()
{
	return  R"====(
#ifdef CLIENT_STATE_TEXTURE_COORD_ARRAY0
Texture2D shaderTexture;
#endif
SamplerState SampleType;


struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
#ifdef CLIENT_STATE_COLOR_ARRAY
    float4 Color : COLOR;
#endif
#ifdef CLIENT_STATE_TEXTURE_COORD_ARRAY0
    float2 Texcoord : TEXCOORD0;
#endif
#ifdef HOLOGRAPHIC 
	uint viewId  : SV_RenderTargetArrayIndex;
#endif
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
#ifdef CLIENT_STATE_TEXTURE_COORD_ARRAY0
    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
	float4 textureColor = shaderTexture.Sample(SampleType, input.Texcoord);
#else
	float4 textureColor = float4(1.0,1.0,1.0,1.0);
#endif

#ifdef CLIENT_STATE_COLOR_ARRAY
	float4 col = input.Color;
#else
	float4 col = float4(1.0,1.0,1.0,1.0);
#endif 

     col*=textureColor;

#ifdef CLIENT_STATE_ALPHA_TEST_LOW
	if(col.a < 0.01)
		discard;
#endif

#ifdef CLIENT_STATE_ALPHA_TEST_HIGH
	if(col.a < 0.95)
		discard;
#endif


    return col;
}
)====";
}

std::string API3DUIShader::GetVertexShader()
{
	return  R"====(
cbuffer MatrixBuffer : register(b0)
{
	row_major float4x4 model_matrix;
	column_major float4x4 uv_matrix;
#ifdef HOLOGRAPHIC
	row_major float4x4 stereo_viewproj[2];
#else
	row_major float4x4 viewproj_matrix;
	//row_major float4x4 view_matrix;
	//row_major float4x4 proj_matrix;
#endif


};

struct VS_INPUT
{
    float2 Position : POSITION;
#ifdef CLIENT_STATE_COLOR_ARRAY
    float4 Color : COLOR;
#endif
#ifdef CLIENT_STATE_TEXTURE_COORD_ARRAY0
    float2 Texcoord : TEXCOORD0;
#endif
#ifdef HOLOGRAPHIC
	uint instId  : SV_InstanceID;
#endif
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
#ifdef CLIENT_STATE_COLOR_ARRAY
    float4 Color : COLOR;
#endif
#ifdef CLIENT_STATE_TEXTURE_COORD_ARRAY0
    float2 Texcoord : TEXCOORD0;
#endif
#ifdef HOLOGRAPHIC 
	uint viewId  : SV_RenderTargetArrayIndex;
#endif
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

    float4 InputPosition = {input.Position.x,input.Position.y,1.0,1.0};
#ifdef HOLOGRAPHIC
	int idx = input.instId % 2;
	float4x4 finalproj = mul(model_matrix, stereo_viewproj[idx]);
	output.viewId = idx;
#else
    float4x4 finalproj = mul(model_matrix, viewproj_matrix);
	//float4x4 finalproj = mul(model_matrix, mul(view_matrix, proj_matrix));
#endif
	output.Position = mul(InputPosition, finalproj);
#ifdef CLIENT_STATE_COLOR_ARRAY
	output.Color = input.Color;
#endif
#ifdef CLIENT_STATE_TEXTURE_COORD_ARRAY0
    float2 uvtmp = mul(float4(input.Texcoord,1.0,0.0),uv_matrix).xy;
	output.Texcoord = uvtmp;
#endif
	return output;
}
)====";
}

IMPLEMENT_CLASS_INFO(API3DUIShader)

API3DUIShader::API3DUIShader(const std::string& name, CLASS_NAME_TREE_ARG) : API3DGenericMeshShader(name, PASS_CLASS_NAME_TREE_ARG)
{
	museGenericLight = false;
}

API3DUIShader::~API3DUIShader()
{
}

