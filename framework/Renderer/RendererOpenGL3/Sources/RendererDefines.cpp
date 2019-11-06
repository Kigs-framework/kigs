#include "RendererDefines.h"
#include "Platform/Renderer/OpenGLInclude.h"

// ## GENERIC SHADER ATTRIBUTE LOC
const char * KIGS_MATRIX_PROJ            = "proj_matrix";
const char * KIGS_MATRIX_MODEL           = "model_matrix";
const char * KIGS_MATRIX_VIEW            = "view_matrix";
const char * KIGS_TEXTURE_0		 		 = "s0_colorMap";
const char * KIGS_TEXTURE_1				 = "s1_colorMap";
const char * KIGS_TEXTURE_2				 = "s2_colorMap";
const char * KIGS_TEXTURE_3				 = "s3_colorMap";
const char * KIGS_MATRIX_BONE			 = "bone_matrix";
const char * KIGS_TANGENT_SPACE_LOD		 = "tangent_space_lod";
const char * KIGS_FAR_PLANE				 = "far_plane";
const char * KIGS_FOG_SCALE				 = "fog_scale";
const char * KIGS_FOG_COLOR				 = "fog_color";

#ifdef WUP // holographics
const char * KIGS_HOLO_VIEW_MATRIX_0 = "uHolographicViewProjectionMatrix[0]";
const char * KIGS_HOLO_VIEW_MATRIX_1 = "uHolographicViewProjectionMatrix[1]";
const char * KIGS_HOLO_VIEW_MATRIX_2 = "uHolographicViewProjectionMatrix";
#endif

const int    KIGS_VERTEX_ATTRIB_VERTEX_ID = 0;
const int    KIGS_VERTEX_ATTRIB_NORMAL_ID = 1;
const int    KIGS_VERTEX_ATTRIB_COLOR_ID = 2;
const int    KIGS_VERTEX_ATTRIB_TEXCOORD_ID = 3;
const int    KIGS_VERTEX_ATTRIB_TANGENT_ID = 4;
const int    KIGS_VERTEX_ATTRIB_BONE_WEIGHT_ID = 5;
const int    KIGS_VERTEX_ATTRIB_BONE_INDEX_ID = 6;
const int    KIGS_VERTEX_ATTRIB_INSTANCE_MATRIX_ID = 7;
const int    KIGS_VERTEX_ATTRIB_RENDER_TARGET_ARRAY_INDEX_ID = 10;

const char * KIGS_VERTEX_ATTRIB_VERTEX   = "attrib_vertex";
const char * KIGS_VERTEX_ATTRIB_NORMAL   = "attrib_normal";
const char * KIGS_VERTEX_ATTRIB_COLOR    = "attrib_color";
const char * KIGS_VERTEX_ATTRIB_TEXCOORD = "attrib_texcoord";
const char * KIGS_VERTEX_ATTRIB_TANGENT  = "attrib_tangent";
const char * KIGS_VERTEX_ATTRIB_BONE_WEIGHT = "attrib_bone_weight";
const char * KIGS_VERTEX_ATTRIB_BONE_INDEX = "attrib_bone_index";
const char * KIGS_VERTEX_ATTRIB_INSTANCE_MATRIX = "attrib_instance_matrix";
const char * KIGS_VERTEX_ATTRIB_RENDER_TARGET_ARRAY_INDEX = "aRenderTargetArrayIndex";

const char*	KIGS_VERTEX_ATTRIB[11] = { KIGS_VERTEX_ATTRIB_VERTEX ,KIGS_VERTEX_ATTRIB_NORMAL ,KIGS_VERTEX_ATTRIB_COLOR ,KIGS_VERTEX_ATTRIB_TEXCOORD ,
										KIGS_VERTEX_ATTRIB_TANGENT,KIGS_VERTEX_ATTRIB_BONE_WEIGHT,KIGS_VERTEX_ATTRIB_BONE_INDEX, 
										KIGS_VERTEX_ATTRIB_INSTANCE_MATRIX, KIGS_VERTEX_ATTRIB_INSTANCE_MATRIX, KIGS_VERTEX_ATTRIB_INSTANCE_MATRIX,
										KIGS_VERTEX_ATTRIB_RENDER_TARGET_ARRAY_INDEX };

// ## BUFFER TARGET
const unsigned int KIGS_BUFFER_TARGET_ARRAY   = GL_ARRAY_BUFFER;
const unsigned int KIGS_BUFFER_TARGET_ELEMENT = GL_ELEMENT_ARRAY_BUFFER;

// ## BUFFER USAGE
const unsigned int KIGS_BUFFER_USAGE_STATIC  = GL_STATIC_DRAW;
const unsigned int KIGS_BUFFER_USAGE_DYNAMIC = GL_DYNAMIC_DRAW;
const unsigned int KIGS_BUFFER_USAGE_STREAM  = GL_STREAM_DRAW;

// ## TYPE
const unsigned int KIGS_UNSIGNED_BYTE  = GL_UNSIGNED_BYTE;
const unsigned int KIGS_UNSIGNED_SHORT = GL_UNSIGNED_SHORT;
const unsigned int KIGS_UNSIGNED_INT   = GL_UNSIGNED_INT;
const unsigned int KIGS_BYTE           = GL_BYTE;
const unsigned int KIGS_SHORT          = GL_SHORT;
const unsigned int KIGS_INT            = GL_INT;
const unsigned int KIGS_FLOAT          = GL_FLOAT;

// ## DRAW MODE
const unsigned int KIGS_DRAW_MODE_POINTS         = GL_POINTS;
const unsigned int KIGS_DRAW_MODE_LINE_STRIP     = GL_LINE_STRIP;
const unsigned int KIGS_DRAW_MODE_LINE_LOOP      = GL_LINE_LOOP;
const unsigned int KIGS_DRAW_MODE_LINES          = GL_LINES;
const unsigned int KIGS_DRAW_MODE_TRIANGLE_STRIP = GL_TRIANGLE_STRIP;
const unsigned int KIGS_DRAW_MODE_TRIANGLE_FAN   = GL_TRIANGLE_FAN;
const unsigned int KIGS_DRAW_MODE_TRIANGLES      = GL_TRIANGLES;