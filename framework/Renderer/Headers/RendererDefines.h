#ifndef _RENDERER_DEFINES_H_
#define _RENDERER_DEFINES_H_

// ## GENERIC SHADER ATTRIBUTE LOC
extern const char * KIGS_MATRIX_PROJ;
extern const char * KIGS_MATRIX_MODEL;
extern const char * KIGS_MATRIX_VIEW;
extern const char * KIGS_MATRIX_UV;
extern const char * KIGS_TEXTURE_0;
extern const char * KIGS_TEXTURE_1;
extern const char * KIGS_TEXTURE_2;
extern const char * KIGS_TEXTURE_3;
extern const char * KIGS_MATRIX_BONE;
extern const char * KIGS_TANGENT_SPACE_LOD;
extern const char * KIGS_FAR_PLANE;
extern const char * KIGS_FOG_SCALE;
extern const char * KIGS_FOG_COLOR;

#ifdef WUP // holographics
extern const char * KIGS_HOLO_VIEW_MATRIX_0;
extern const char * KIGS_HOLO_VIEW_MATRIX_1;
extern const char * KIGS_HOLO_VIEW_MATRIX_2;
#endif


extern const int    KIGS_VERTEX_ATTRIB_VERTEX_ID;
extern const char * KIGS_VERTEX_ATTRIB_VERTEX;
extern const int    KIGS_VERTEX_ATTRIB_NORMAL_ID;
extern const char * KIGS_VERTEX_ATTRIB_NORMAL;
extern const int    KIGS_VERTEX_ATTRIB_COLOR_ID;
extern const char * KIGS_VERTEX_ATTRIB_COLOR;
extern const int    KIGS_VERTEX_ATTRIB_TEXCOORD_ID;
extern const char * KIGS_VERTEX_ATTRIB_TEXCOORD;
extern const int    KIGS_VERTEX_ATTRIB_TANGENT_ID;
extern const char * KIGS_VERTEX_ATTRIB_TANGENT;
extern const int    KIGS_VERTEX_ATTRIB_BONE_WEIGHT_ID;
extern const char * KIGS_VERTEX_ATTRIB_BONE_WEIGHT;
extern const int    KIGS_VERTEX_ATTRIB_BONE_INDEX_ID;
extern const char * KIGS_VERTEX_ATTRIB_BONE_INDEX;
extern const int    KIGS_VERTEX_ATTRIB_INSTANCE_MATRIX_ID;
extern const char * KIGS_VERTEX_ATTRIB_INSTANCE_MATRIX;
extern const int    KIGS_VERTEX_ATTRIB_RENDER_TARGET_ARRAY_INDEX_ID;
extern const char * KIGS_VERTEX_ATTRIB_RENDER_TARGET_ARRAY_INDEX;


extern const char*	KIGS_VERTEX_ATTRIB[11];
extern int			KIGS_VERTEX_ATTRIB_INDEX[11];

// ## BUFFER TARGET
extern const unsigned int KIGS_BUFFER_TARGET_ARRAY;
extern const unsigned int KIGS_BUFFER_TARGET_ELEMENT;

// ## BUFFER USAGE
extern const unsigned int KIGS_BUFFER_USAGE_STATIC;
extern const unsigned int KIGS_BUFFER_USAGE_DYNAMIC;
extern const unsigned int KIGS_BUFFER_USAGE_STREAM;

// ## TYPE
extern const unsigned int KIGS_UNSIGNED_BYTE;
extern const unsigned int KIGS_UNSIGNED_SHORT;
extern const unsigned int KIGS_UNSIGNED_INT;
extern const unsigned int KIGS_BYTE;
extern const unsigned int KIGS_SHORT;
extern const unsigned int KIGS_INT;
extern const unsigned int KIGS_FLOAT;

// ## DRAW MODE
extern const unsigned int KIGS_DRAW_MODE_POINTS;
extern const unsigned int KIGS_DRAW_MODE_LINE_STRIP;
extern const unsigned int KIGS_DRAW_MODE_LINE_LOOP;
extern const unsigned int KIGS_DRAW_MODE_LINES;
extern const unsigned int KIGS_DRAW_MODE_TRIANGLE_STRIP;
extern const unsigned int KIGS_DRAW_MODE_TRIANGLE_FAN;
extern const unsigned int KIGS_DRAW_MODE_TRIANGLES;

#endif //_RENDERER_DEFINES_H_
