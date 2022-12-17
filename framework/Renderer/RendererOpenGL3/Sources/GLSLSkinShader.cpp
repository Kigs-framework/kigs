#include "GLSLSkinShader.h"


std::string API3DSkinShader::GetVertexShader()
{
	std::string result = API3DGenericMeshShader::GetDefaultVertexShaderAttributes();
	result += R"====(
	attribute vec4 attrib_bone_index;
	attribute vec4 attrib_bone_weight;
	)====";
	result += API3DGenericMeshShader::GetDefaultVertexShaderUniforms();
	result += R"====(
	uniform mat4	bone_matrix[66];
	)====";
	result += API3DGenericMeshShader::GetDefaultVaryings();
	result += R"====(

	void main()
	{
		mat4 actual_model_matrix = model_matrix;
#ifdef CLIENT_STATE_INSTANCED
		actual_model_matrix = mat4(	vec4(attrib_instance_matrix[0].xyz,0), 
								vec4(attrib_instance_matrix[0][3],attrib_instance_matrix[1][0],attrib_instance_matrix[1][1],0),
								vec4(attrib_instance_matrix[1][2],attrib_instance_matrix[1][3],attrib_instance_matrix[2][0],0),
								vec4(attrib_instance_matrix[2][1],attrib_instance_matrix[2][2],attrib_instance_matrix[2][3],1));
#endif

		mat4 bone_transform = bone_matrix[int(attrib_bone_index[0])] * attrib_bone_weight[0];
		bone_transform += bone_matrix[int(attrib_bone_index[1])] * attrib_bone_weight[1];
		bone_transform += bone_matrix[int(attrib_bone_index[2])] * attrib_bone_weight[2];
		bone_transform += bone_matrix[int(attrib_bone_index[3])] * attrib_bone_weight[3];
		actual_model_matrix = actual_model_matrix * bone_transform;
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
	};
)====";


	return result;
}


IMPLEMENT_CLASS_INFO(API3DSkinShader)

