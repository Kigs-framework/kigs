#include "GLSLSkinShader.h"


kstl::string API3DSkinShader::GetVertexShader()
{
	kstl::string result = API3DGenericMeshShader::GetDefaultVertexShaderAttributes();
	result += API3DGenericMeshShader::GetDefaultVertexShaderUniforms();
	result += API3DGenericMeshShader::GetDefaultVaryings();


	result += R"====(
	attribute vec4 attrib_bone_index;
	attribute vec4 attrib_bone_weight;
	uniform mat4	bone_matrix[66];

	void main()
	{
		mat4 final_model_matrix = model_matrix;
		mat4 bone_transform = bone_matrix[int(attrib_bone_index[0])] * attrib_bone_weight[0];
		bone_transform += bone_matrix[int(attrib_bone_index[1])] * attrib_bone_weight[1];
		bone_transform += bone_matrix[int(attrib_bone_index[2])] * attrib_bone_weight[2];
		bone_transform += bone_matrix[int(attrib_bone_index[3])] * attrib_bone_weight[3];
		final_model_matrix = model_matrix * bone_transform;
		#ifdef CLIENT_STATE_TEXTURE_COORD_ARRAY0 
		vTexcoord = attrib_texcoord; 
		#endif
		#ifdef CLIENT_STATE_TANGENT_ARRAY 
		T = normalize(final_model_matrix * vec4(attrib_tangent, 0.0)).xyz;
		#endif
		#ifdef CLIENT_STATE_NORMAL_ARRAY 
		N = normalize(final_model_matrix * vec4(attrib_normal, 0.0)).xyz;
		#endif
		#ifdef CLIENT_STATE_COLOR_ARRAY 
		vColor = attrib_color;
		#endif
		mat4 viewmodel = view_matrix*final_model_matrix;
		FragPos = vec3(final_model_matrix * vec4(attrib_vertex, 1.0));
		gl_Position = proj_matrix * viewmodel * vec4(attrib_vertex, 1.0);
	};
)====";


	return result;
}


IMPLEMENT_CLASS_INFO(API3DSkinShader)

