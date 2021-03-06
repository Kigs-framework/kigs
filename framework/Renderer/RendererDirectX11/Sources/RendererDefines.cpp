#include "PrecompiledHeaders.h"
#include "RendererDefines.h"
#include <d3d11.h> 

// ## GENERIC SHADER ATTRIBUTE LOC
const char * KIGS_MATRIX_PROJ            = "proj_matrix";
const char * KIGS_MATRIX_MODEL           = "model_matrix";
const char * KIGS_MATRIX_VIEW            = "view_matrix";
const char * KIGS_MATRIX_UV				 = "uv_matrix";

const int    KIGS_VERTEX_ATTRIB_VERTEX_ID=0;
const int    KIGS_VERTEX_ATTRIB_NORMAL_ID=1;
const int    KIGS_VERTEX_ATTRIB_COLOR_ID=2;
const int    KIGS_VERTEX_ATTRIB_TEXCOORD_ID=3;
const int    KIGS_VERTEX_ATTRIB_TANGENT_ID=4;
const int    KIGS_VERTEX_ATTRIB_BONE_WEIGHT_ID=5;
const int    KIGS_VERTEX_ATTRIB_BONE_INDEX_ID=6;
const int    KIGS_VERTEX_ATTRIB_INSTANCE_MATRIX_ID = 7; // + 8 and 9

const char * KIGS_VERTEX_ATTRIB_VERTEX   = "POSITION";
const char * KIGS_VERTEX_ATTRIB_NORMAL   = "NORMAL";
const char * KIGS_VERTEX_ATTRIB_COLOR    = "COLOR";
const char * KIGS_VERTEX_ATTRIB_TEXCOORD = "TEXCOORD";
const char * KIGS_VERTEX_ATTRIB_TANGENT  = "TANGENT";
const char * KIGS_VERTEX_ATTRIB_BONE_WEIGHT = "BLENDWEIGHT";
const char * KIGS_VERTEX_ATTRIB_BONE_INDEX = "BLENDINDICES";
const char * KIGS_VERTEX_ATTRIB_INSTANCE_MATRIX = "INSTANCE_TRANSFORM";

const char*	KIGS_VERTEX_ATTRIB[11] = { KIGS_VERTEX_ATTRIB_VERTEX ,KIGS_VERTEX_ATTRIB_NORMAL ,KIGS_VERTEX_ATTRIB_COLOR ,KIGS_VERTEX_ATTRIB_TEXCOORD ,
										KIGS_VERTEX_ATTRIB_TANGENT,KIGS_VERTEX_ATTRIB_BONE_WEIGHT,KIGS_VERTEX_ATTRIB_BONE_INDEX,
										KIGS_VERTEX_ATTRIB_INSTANCE_MATRIX, KIGS_VERTEX_ATTRIB_INSTANCE_MATRIX, KIGS_VERTEX_ATTRIB_INSTANCE_MATRIX, "" };

int			KIGS_VERTEX_ATTRIB_INDEX[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 0 };

// ## BUFFER TARGET
const unsigned int KIGS_BUFFER_TARGET_ARRAY   = D3D11_BIND_VERTEX_BUFFER;
const unsigned int KIGS_BUFFER_TARGET_ELEMENT = D3D11_BIND_INDEX_BUFFER;

// ## BUFFER USAGE
const unsigned int KIGS_BUFFER_USAGE_STATIC  = D3D11_USAGE_IMMUTABLE;
const unsigned int KIGS_BUFFER_USAGE_DYNAMIC = D3D11_USAGE_DYNAMIC;

// ## TYPE
const unsigned int KIGS_UNSIGNED_BYTE  = DXGI_FORMAT_R8_UINT;
const unsigned int KIGS_UNSIGNED_SHORT = DXGI_FORMAT_R16_UINT;
const unsigned int KIGS_UNSIGNED_INT   = DXGI_FORMAT_R32_UINT;
const unsigned int KIGS_BYTE           = DXGI_FORMAT_R8_SINT;
const unsigned int KIGS_SHORT          = DXGI_FORMAT_R16_SINT;
const unsigned int KIGS_FLOAT          = DXGI_FORMAT_R32_FLOAT;

// ## DRAW MODE
const unsigned int KIGS_DRAW_MODE_POINTS         = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
const unsigned int KIGS_DRAW_MODE_LINE_STRIP     = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
const unsigned int KIGS_DRAW_MODE_LINE_LOOP      = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
const unsigned int KIGS_DRAW_MODE_LINES          = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
const unsigned int KIGS_DRAW_MODE_TRIANGLE_STRIP = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
const unsigned int KIGS_DRAW_MODE_TRIANGLE_FAN   = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
const unsigned int KIGS_DRAW_MODE_TRIANGLES      = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;