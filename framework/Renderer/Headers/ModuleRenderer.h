#ifndef _MODULERENDERER_H_
#define _MODULERENDERER_H_

#include "ModuleBase.h"

#include "FixedSizeStack.h"
#include "UIVerticesInfo.h"
#include "Shader.h"

#include "CoreSTL.h"
#include "TecLibs/TecHash.h"

/*! \defgroup Renderer Renderer module
 *  manage rendering
 */

#ifdef KIGS_TOOLS
struct RendererStats
{
	int ShaderSwitch = 0;
	int DrawCalls = 0;
	int DrawCallsTriangleCount = 0;
	int OcclusionQueriesRequested = 0;
	int OcclusionQueriesStarted = 0;
	int AllocatedBuffers = 0;
};
extern RendererStats gRendererStats;
#endif

ModuleBase*	PlatformRendererModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);

class ModuleSpecificRenderer;
class MaterialStage;
class Scene3D;
class TravState;
class ShaderBase;

//! number max of stage
#define MAX_MATERIAL_STAGE	4

enum RendererCullMode
{
	RENDERER_CULL_NONE = 0,
	RENDERER_CULL_BACK = 1,
	RENDERER_CULL_FRONT = 2,
	RENDERER_CULL_FRONT_AND_BACK = 3
};

enum RendererLightMode
{
	RENDERER_LIGHT_OFF = 0,
	RENDERER_LIGHT_ON,
};

enum RendererAlphaTestMode
{
	RENDERER_ALPHA_TEST_OFF = 0,
	RENDERER_ALPHA_TEST_ON,
};

enum RendererAlphaMode
{//GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, and GL_ALWAYS
	RENDERER_ALPHA_NEVER = 0,
	RENDERER_ALPHA_LESS = 1,
	RENDERER_ALPHA_EQUAL = 2,
	RENDERER_ALPHA_LEQUAL = 3,
	RENDERER_ALPHA_GREATER = 4,
	RENDERER_ALPHA_NOTEQUAL = 5,
	RENDERER_ALPHA_GEQUAL = 6,
	RENDERER_ALPHA_ALWAYS = 7
};

enum RendererStencilMode
{//GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, and GL_ALWAYS
	RENDERER_STENCIL_NEVER = 0,
	RENDERER_STENCIL_LESS = 1,
	RENDERER_STENCIL_EQUAL = 2,
	RENDERER_STENCIL_LEQUAL = 3,
	RENDERER_STENCIL_GREATER = 4,
	RENDERER_STENCIL_NOTEQUAL = 5,
	RENDERER_STENCIL_GEQUAL = 6,
	RENDERER_STENCIL_ALWAYS = 7
};

enum RendererStencilOp
{// GL_KEEP, GL_ZERO, GL_REPLACE, GL_INCR, GL_INCR_WRAP, GL_DECR, GL_DECR_WRAP, and GL_INVERT
	RENDERER_STENCIL_OP_KEEP = 0,
	RENDERER_STENCIL_OP_ZERO, 
	RENDERER_STENCIL_OP_REPLACE, 
	RENDERER_STENCIL_OP_INCR, 
	RENDERER_STENCIL_OP_INCR_WRAP, 
	RENDERER_STENCIL_OP_DECR, 
	RENDERER_STENCIL_OP_DECR_WRAP, 
	RENDERER_STENCIL_OP_INVERT
};

enum RendererBlendMode
{
	RENDERER_BLEND_OFF = 0,
	RENDERER_BLEND_ON,
};


enum RendererBlendFuncMode
{//GL_ZERO, GL_ONE, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 
	//GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR, GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA, and GL_SRC_ALPHA_SATURATE
	RENDERER_BLEND_ZERO = 0,
	RENDERER_BLEND_ONE = 1,
	RENDERER_BLEND_SRC_COLOR = 2,
	RENDERER_BLEND_ONE_MINUS_SRC_COLOR = 3,
	RENDERER_BLEND_DST_COLOR = 4,
	RENDERER_BLEND_ONE_MINUS_DST_COLOR = 5,
	RENDERER_BLEND_SRC_ALPHA = 6,
	RENDERER_BLEND_ONE_MINUS_SRC_ALPHA = 7,
	RENDERER_BLEND_DST_ALPHA = 8,
	RENDERER_BLEND_ONE_MINUS_DST_ALPHA = 9,
	RENDERER_BLEND_CONSTANT_COLOR = 10,
	RENDERER_BLEND_ONE_MINUS_CONSTANT_COLOR = 11,
	RENDERER_BLEND_CONSTANT_ALPHA = 12,
	RENDERER_BLEND_ONE_MINUS_CONSTANT_ALPHA = 13,
	RENDERER_BLEND_GL_SRC_ALPHA_SATURATE = 14
};

enum RendererDepthMaskMode
{
	RENDERER_DEPTH_MASK_OFF = 0,
	RENDERER_DEPTH_MASK_ON,
};

enum RendererDepthTestFunc
{//GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, and GL_ALWAYS
	RENDERER_DEPTH_TEST_NEVER = 0,
	RENDERER_DEPTH_TEST_LESS = 1,
	RENDERER_DEPTH_TEST_EQUAL = 2,
	RENDERER_DEPTH_TEST_LEQUAL = 3,
	RENDERER_DEPTH_TEST_GREATER = 4,
	RENDERER_DEPTH_TEST_NOTEQUAL = 5,
	RENDERER_DEPTH_TEST_GEQUAL = 6,
	RENDERER_DEPTH_TEST_ALWAYS = 7
};

/*enum RendererDepthTestMode
{
	RENDERER_DEPTH_TEST_OFF = 0,
	RENDERER_DEPTH_TEST_ON,
};*/

enum RendererClearMode
{
	RENDERER_CLEAR_NONE = 0,
	RENDERER_CLEAR_COLOR = 1,
	RENDERER_CLEAR_DEPTH = 2,
	RENDERER_CLEAR_STENCIL = 4,
	RENDERER_CLEAR_COLOR_AND_DEPTH = RENDERER_CLEAR_COLOR | RENDERER_CLEAR_DEPTH,
	RENDERER_CLEAR_COLOR_AND_STENCIL = RENDERER_CLEAR_COLOR | RENDERER_CLEAR_STENCIL,
	RENDERER_CLEAR_DEPTH_AND_STENCIL = RENDERER_CLEAR_DEPTH | RENDERER_CLEAR_STENCIL,
	RENDERER_CLEAR_COLOR_AND_DEPTH_AND_STENCIL = RENDERER_CLEAR_COLOR | RENDERER_CLEAR_DEPTH | RENDERER_CLEAR_STENCIL
};

enum RendererScissorTestMode
{
	RENDERER_SCISSOR_TEST_OFF = 0,
	RENDERER_SCISSOR_TEST_ON,
};

enum RendererTextureType
{
	RENDERER_TEXTURE_1D = 1,
	RENDERER_TEXTURE_2D = 2,
	RENDERER_TEXTURE_3D = 4,
	RENDERER_TEXTURE_CUBE_MAP = 8,
};

enum RendererTexParameter1
{//GL_TEXTURE_MIN_FILTER, GL_TEXTURE_MAG_FILTER,  GL_TEXTURE_WRAP_S,GL_TEXTURE_WRAP_T,GL_TEXTURE_WRAP_R
	RENDERER_TEXTURE_MIN_FILTER = 1,
	RENDERER_TEXTURE_MAG_FILTER = 2,
	RENDERER_TEXTURE_WRAP_S = 3,
	RENDERER_TEXTURE_WRAP_T = 4,
	RENDERER_TEXTURE_WRAP_R = 5
};

enum RendererTexParameter2
{//GL_NEAREST, GL_CLAMP_TO_EDGE,GL_LINEAR,GL_REPEAT,GL_LINEAR_MIPMAP_LINEAR
	RENDERER_NEAREST = 1,
	RENDERER_CLAMP_TO_EDGE = 2,
	RENDERER_LINEAR = 3,
	RENDERER_REPEAT = 4,
	RENDERER_LINEAR_MIPMAP_LINEAR = 5
};


enum RendererLightModeliMode
{
	RENDERER_LIGHT_MODEL_LOCAL_VIEWER = 0,
	RENDERER_LIGHT_MODEL_COLOR_CONTROL = 1
};

enum RendererLightModeliParam
{
	RENDERER_LIGHT_MODEL_TRUE = 0,
	RENDERER_LIGHT_MODEL_SEPARATE_SPECULAR_COLOR = 1
};

enum RendererLightModelfvMode
{
	RENDERER_LIGHT_MODEL_AMBIENT = 0
};

enum RendererColorMaterialMode
{
	RENDERER_COLOR_MATERIAL_OFF = 0,
	RENDERER_COLOR_MATERIAL_ON,
};

enum RendererNormalizeMode
{
	RENDERER_NORMALIZE_OFF = 0,
	RENDERER_NORMALIZE_ON,
};

enum RendererPolygonSmoothMode
{
	RENDERER_POLYGON_SMOOTH_OFF = 0,
	RENDERER_POLYGON_SMOOTH_ON,
};

enum RendererFogMode
{
	RENDERER_FOG_OFF = 0,
	RENDERER_FOG_ON,
};

enum RendererLight0Mode
{
	RENDERER_LIGHT0_OFF = 0,
	RENDERER_LIGHT0_ON,
};

enum RendererLineSmoothMode
{
	RENDERER_LINE_SMOOTH_OFF = 0,
	RENDERER_LINE_SMOOTH_ON,
};

enum RendererColorMaterialFace
{
	RENDERER_FRONT_AND_BACK = 0,
	RENDERER_FRONT = 1,
	RENDERER_BACK = 2
};

enum RendererColorMaterialParam
{
	RENDERER_AMBIENT_AND_DIFFUSE = 0,
	RENDERER_EMISSION = 1,
	RENDERER_AMBIENT = 2,
	RENDERER_DIFFUSE = 3,
	RENDERER_SPECULAR = 4
};

enum RendererPolygonMode
{
	RENDERER_POINT=0,
	RENDERER_LINE=1,
	RENDERER_FILL=2
};

enum RendererQueryType
{
	RENDERER_QUERY_SAMPLES_PASSED = 0,
	RENDERER_QUERY_ANY_SAMPLES_PASSED,
	RENDERER_QUERY_TYPE_COUNT
};


struct PointLight
{
	v4f position;
	v4f attenuation;
	v4f specular;
	v4f diffuse;
	v4f ambient;
};
struct DirLight
{
	v4f position;
	v4f diffuse;
	v4f specular;
	v4f ambient;
};
struct SpotLight
{
	v4f position;
	v4f directionAndCutOff;
	v4f attenuationAndSpotExponent;
	v4f diffuse;
	v4f specular;
	v4f ambient;
};

struct LightStruct
{
	std::vector<PointLight> pointlights;
	std::vector<DirLight> dirlights;
	std::vector<SpotLight> spotlights;
};

namespace std
{
	template<>
	struct hash<PointLight>
	{
		size_t operator()(const PointLight& pl)
		{
			size_t hash = 0;
			hash_combine(hash, pl.position, pl.attenuation, pl.specular, pl.diffuse, pl.ambient);
			return hash;
		}
	};

	template<>
	struct hash<DirLight>
	{
		size_t operator()(const DirLight& dl)
		{
			size_t hash = 0;
			hash_combine(hash, dl.position, dl.specular, dl.diffuse, dl.ambient);
			return hash;
		}
	};

	template<>
	struct hash<SpotLight>
	{
		size_t operator()(const SpotLight& sl)
		{
			size_t hash = 0;
			hash_combine(hash, sl.position, sl.directionAndCutOff, sl.attenuationAndSpotExponent, sl.diffuse, sl.specular, sl.ambient);
			return hash;
		}
	};

	template<>
	struct hash<LightStruct>
	{
		size_t operator()(const LightStruct& lightstruct)
		{
			size_t hash = 0;
			for (auto& l : lightstruct.pointlights)
				hash_combine(hash, l);
			for (auto& l : lightstruct.dirlights)
				hash_combine(hash, l);
			for (auto& l : lightstruct.spotlights)
				hash_combine(hash, l);
			return hash;
		}
	};
}


class DirectRenderingMethods;
class RenderingScreen;



// ****************************************
// * ModuleRenderer class
// * --------------------------------------
/**
 * \file	ModuleRenderer.h
 * \class	ModuleRenderer
 * \ingroup Renderer
 * \ingroup Module
 * \brief	Generic rendering module.
 */
 // ****************************************
class ModuleRenderer : public ModuleBase
{
public:

	enum ShaderFlags
	{
		VERTEX_ARRAY_MASK = 1 << 0,
		NORMAL_ARRAY_MASK = 1 << 1,
		COLOR_ARRAY_MASK = 1 << 2,
		TEXCOORD_ARRAY_MASK = 1 << 3,
		TEXCOORD_ARRAY_MASK1 = 1 << 4,
		TEXCOORD_ARRAY_MASK2 = 1 << 5,
		TEXCOORD_ARRAY_MASK3 = 1 << 6,
		TANGENT_ARRAY_MASK = 1 << 7,
		BONE_WEIGHT_ARRAY_MASK = 1 << 8,
		BONE_INDEX_ARRAY_MASK = 1 << 9,
		DEFERRED_SHADER_MASK = 1 << 10,
		NO_LIGHT_MASK = 1 << 11,
		ALPHA_TEST_LOW = 1 << 12,
		ALPHA_TEST_HIGH = 1 << 13,
		DRAW_NORMALS = 1 << 14,
		DRAW_UVS = 1 << 15,
		HOLOGRAPHIC = 1 << 16,
		FOG = 1 << 17,
		INSTANCED = 1 << 18,
		SHADER_FLAGS_USER1 = 1 << 19,
	};

	DECLARE_CLASS_INFO(ModuleRenderer, ModuleBase, Renderer);
	DECLARE_CONSTRUCTOR(ModuleRenderer);

	/**
	 * \brief		initialize module
	 * \fn			void Init(Core* core, const kstl::vector<CoreModifiableAttribute*>* params);
	 * \param		core : link to the core, NOT NULL
	 * \param		params : list of parameters
	 */
	void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;

	/**
	 * \brief		close module
	 * \fn			void Close();
	 */
	void Close() override;

	/**
	 * \brief		update module
	 * \fn			virtual void Update(const Timer& timer, void* addParam);
	 * \param		timer : timer global
	 */
	void Update(const Timer& timer, void* addParam) override;

	ModuleSpecificRenderer*	GetSpecificRenderer()
	{
		return mSpecificRenderer;
	}

	static ModuleSpecificRenderer * mTheGlobalRenderer;

protected:	
	ModuleSpecificRenderer*		mSpecificRenderer;
};

// ****************************************
// * RenderingState class
// * --------------------------------------
/**
 * \file	ModuleRenderer.h
 * \class	RenderingState
 * \ingroup Renderer
 * \brief	Keep current global rendering state
 */
 // ****************************************
class RenderingState
{
public:
	virtual ~RenderingState() = default;

	void InitHardwareState()
	{
		if (!mHardwareWasInit)
		{
			ProtectedInitHardwareState();
			mHardwareWasInit = true;
		}
	}

	void UninitHardwareState()
	{
		mHardwareWasInit = false;
	}

	virtual void FlushState(RenderingState* currentState, bool force = false) = 0;	// copy this to currentState effective change currentState
	virtual void FlushLightModeli(RenderingState* currentState) = 0;
	virtual void FlushLightModelfv(RenderingState* currentState) = 0;
	
	void SetCullMode(RendererCullMode mode)
	{
		mGlobalCullFlag = mode;
	}

	void SetLightMode(RendererLightMode mode)
	{
		mGlobalLightFlag = (mode == 1);
	}

	void SetAlphaTestMode(RendererAlphaTestMode mode)
	{
		mGlobalAlphaTestFlag = (mode == 1);
	}

	void SetAlphaMode(RendererAlphaMode mode, float value)
	{
		mGlobalAlphaFlag = mode;
		mGlobalAlphaValueFlag = value;
	}

	void SetBlendMode(RendererBlendMode mode)
	{
		mGlobalBlendFlag = (mode == 1);
	}

	void SetBlendFuncMode(RendererBlendFuncMode mode1, RendererBlendFuncMode mode2)
	{
		mGlobalBlendValue1Flag = mode1;
		mGlobalBlendValue2Flag = mode2;
	}

	void SetDepthMaskMode(RendererDepthMaskMode mode)
	{
		mGlobalDepthMaskFlag = (mode == 1);
	}

	void SetDepthTestMode(bool mode)
	{
		mGlobalDepthTestFlag = mode;
	}

	void SetDepthTestFunc(RendererDepthTestFunc func)
	{
		mGlobalDepthTestFunc = func;
	}

	void SetDepthValueMode(float mode)
	{
		mGlobalDepthValueFlag = mode;
	}

	void SetClearColorValue(float r, float g, float b, float a)
	{
		mGlobalRedClearValueFlag = r;
		mGlobalGreenClearValueFlag = g;
		mGlobalBlueClearValueFlag = b;
		mGlobalAlphaClearValueFlag = a;
	}

	virtual void ClearView(RendererClearMode clearMode) = 0;

	virtual void Viewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {	}

	void SetScissorTestMode(RendererScissorTestMode mode)
	{
		mGlobalScissorTestFlag = (mode == 1);
	}

	void SetScissorValue(int x, int y, int width, int height)
	{
		mGlobalScissorXFlag = x;
		mGlobalScissorYFlag = y;
		mGlobalScissorWidthFlag = width;
		mGlobalScissorHeightFlag = height;
	}

	void SetLightModeli(RendererLightModeliMode name, RendererLightModeliParam param) {

		mGlobalLightModeliFlag = name;
		mGlobalLightModeliParamFlag = param;
	}

	void SetLightModelfv(RendererLightModelfvMode name, float* param) {

		mGlobalLightModelfvFlag = name;
		mGlobalLightModelfvParamFlag[0] = param[0];
		mGlobalLightModelfvParamFlag[1] = param[1];
		mGlobalLightModelfvParamFlag[2] = param[2];
		mGlobalLightModelfvParamFlag[3] = param[3];
	}

	void SetColorMaterialMode(bool mode) { mGlobalColorMaterialFlag = mode; }

	void SetNormalizeMode(RendererNormalizeMode mode) {

		mGlobalNormalizeFlag = (mode == 1);
	}

	void SetPolygonSmoothMode(RendererPolygonSmoothMode mode) {

		mGlobalPolygonSmoothFlag = (mode == 1);

	}

	void SetFogMode(RendererFogMode mode) {

		mGlobalFogFlag = (mode == 1);
	}

	void SetLight0Mode(RendererLight0Mode mode, unsigned int index) {
		mGlobalLight0Flag = (mode == 1);
		mGlobalLight0IndexFlag = index;
	}

	void SetLineSmoothMode(RendererLineSmoothMode mode) {

		mGlobalLineSmoothFlag = (mode == 1);

	}

	void SetColorMaterial(RendererColorMaterialFace face, RendererColorMaterialParam param) {

		mGlobalColorMaterialFaceFlag = face;
		mGlobalColorMaterialParamFlag = param;

	}

	void SetColorMask(bool r, bool g, bool b, bool a)
	{
		mGlobalColorMask[0] = r;
		mGlobalColorMask[1] = g;
		mGlobalColorMask[2] = b;
		mGlobalColorMask[3] = a;
	}

	void SetStencilMode(RendererCullMode face, RendererStencilMode mode, int reference_value, u32 mask)
	{
		if (face == RendererCullMode::RENDERER_CULL_FRONT || face == RendererCullMode::RENDERER_CULL_FRONT_AND_BACK)
		{
			mGlobalStencilMode[0] = mode;
			mGlobalStencilFuncRef[0] = reference_value;
			mGlobalStencilFuncMask[0] = mask;
		}
		if (face == RendererCullMode::RENDERER_CULL_BACK || face == RendererCullMode::RENDERER_CULL_FRONT_AND_BACK)
		{
			mGlobalStencilMode[1] = mode;
			mGlobalStencilFuncRef[1] = reference_value;
			mGlobalStencilFuncMask[1] = mask;
		}
	}

	void SetStencilMask(RendererCullMode face, u32 mask)
	{
		if (face == RendererCullMode::RENDERER_CULL_FRONT || face == RendererCullMode::RENDERER_CULL_FRONT_AND_BACK)
			mGlobalStencilMask[0] = mask;
		if (face == RendererCullMode::RENDERER_CULL_BACK || face == RendererCullMode::RENDERER_CULL_FRONT_AND_BACK)
			mGlobalStencilMask[1] = mask;
	}

	void SetStencilOp(RendererCullMode face, RendererStencilOp sfail, RendererStencilOp dpfail, RendererStencilOp pass)
	{
		if (face == RendererCullMode::RENDERER_CULL_FRONT || face == RendererCullMode::RENDERER_CULL_FRONT_AND_BACK)
		{
			mGlobalStencilOpSFail[0] = sfail;
			mGlobalStencilOpDPFail[0] = dpfail;
			mGlobalStencilOpPass[0] = pass;
		}
		if (face == RendererCullMode::RENDERER_CULL_BACK || face == RendererCullMode::RENDERER_CULL_FRONT_AND_BACK)
		{
			mGlobalStencilOpSFail[1] = sfail;
			mGlobalStencilOpDPFail[1] = dpfail;
			mGlobalStencilOpPass[1] = pass;
		}
	}

	void SetStencilTest(bool enable)
	{
		mGlobalStencilEnabled = enable;
	}

	void SetPolygonMode(RendererPolygonMode mode)
	{
		mPolygonMode = mode;
	}

protected:
	virtual void ProtectedInitHardwareState() = 0;

	//! global cull flag 
	int		mGlobalCullFlag = RENDERER_CULL_NONE;
	int		mGlobalAlphaFlag = RENDERER_ALPHA_ALWAYS;
	float	mGlobalAlphaValueFlag = 0.0f;

	int		mGlobalBlendValue1Flag = RENDERER_BLEND_ONE;
	int		mGlobalBlendValue2Flag = RENDERER_BLEND_ZERO;

	float	mGlobalDepthValueFlag = 1.0f;
	union {
		struct {
			float	mGlobalRedClearValueFlag;
			float	mGlobalGreenClearValueFlag;
			float	mGlobalBlueClearValueFlag;
			float	mGlobalAlphaClearValueFlag;
		};
		float mGlobalClearValueFlag[4] = {0,0,0,0};
	};
	int		mGlobalScissorXFlag = 0;
	int		mGlobalScissorYFlag = 0;
	int		mGlobalScissorWidthFlag = 0;
	int		mGlobalScissorHeightFlag = 0;
	int		mGlobalLight0IndexFlag = 0;
	int		mGlobalColorMaterialFaceFlag = 0;
	int		mGlobalColorMaterialParamFlag = 0;

	int		mGlobalLightModeliFlag = 0;
	int		mGlobalLightModeliParamFlag = 0;
	int		mGlobalLightModelfvFlag = 0;
	float	mGlobalLightModelfvParamFlag[4] = {};

	bool	mGlobalLightModelFlag = true;
	bool	mGlobalColorMaterialFlag = false;
	bool	mGlobalNormalizeFlag = false;
	bool	mGlobalPolygonSmoothFlag = false;
	bool	mGlobalFogFlag = false;
	bool	mGlobalLight0Flag = false;
	bool	mGlobalScissorTestFlag = RENDERER_SCISSOR_TEST_OFF;
	bool	mGlobalDepthMaskFlag = RENDERER_DEPTH_MASK_ON;
	bool	mGlobalDepthTestFlag = false;
	RendererDepthTestFunc mGlobalDepthTestFunc = RENDERER_DEPTH_TEST_LESS;
	bool	mGlobalBlendFlag = RENDERER_BLEND_OFF;
	bool	mGlobalLightFlag = RENDERER_LIGHT_OFF;
	bool	mGlobalAlphaTestFlag = RENDERER_ALPHA_TEST_OFF;
	bool	mGlobalLineSmoothFlag = RENDERER_LINE_SMOOTH_OFF;

	int		mPolygonMode = RENDERER_FILL;

	bool    mGlobalColorMask[4] = { true, true, true, true };
	bool	mGlobalStencilEnabled = false;
	u32		mGlobalStencilMask[2] = { 0xFFFFFFFF,0xFFFFFFFF };
	RendererStencilMode		mGlobalStencilMode[2] = { RENDERER_STENCIL_ALWAYS,RENDERER_STENCIL_ALWAYS };
	int		mGlobalStencilFuncRef[2] = {};
	u32		mGlobalStencilFuncMask[2] = { 0xFFFFFFFF,0xFFFFFFFF };
	RendererStencilOp		mGlobalStencilOpSFail[2] = { RENDERER_STENCIL_OP_KEEP,RENDERER_STENCIL_OP_KEEP };
	RendererStencilOp		mGlobalStencilOpDPFail[2] = { RENDERER_STENCIL_OP_KEEP,RENDERER_STENCIL_OP_KEEP };
	RendererStencilOp		mGlobalStencilOpPass[2] = { RENDERER_STENCIL_OP_KEEP,RENDERER_STENCIL_OP_KEEP };

	bool	mHardwareWasInit = false;
};

static const int MATRIX_MODE_MODEL = 0;
static const int MATRIX_MODE_PROJECTION = 1;
static const int MATRIX_MODE_VIEW = 2;

// ****************************************
// * VertexBufferManagerBase class
// * --------------------------------------
/**
 * \file	ModuleRenderer.h
 * \class	VertexBufferManagerBase
 * \ingroup Renderer
 * \brief	Virtual base class for Vertex buffer manager.
 */
 // ****************************************
class VertexBufferManagerBase
{
public:
	VertexBufferManagerBase() {}
	virtual ~VertexBufferManagerBase() {}

	virtual void GenBuffer(int count, unsigned int * id) = 0;
	virtual void DelBuffer(int count, unsigned int * id)=0;
	virtual void DelBufferLater(int count, unsigned int * id)=0;

	virtual void BufferData(unsigned int bufferName, unsigned int bufftype, int size, void* data, unsigned int usage)=0;

	virtual void UnbindBuffer(unsigned int bufferName, int target = 0) = 0;
	virtual void FlushBindBuffer(int target = 0, bool force = false) = 0;
	virtual void Clear(bool push = false) = 0;

	virtual void SetArrayBuffer(unsigned int bufferName, int slot = 0) = 0;
	virtual void SetElementBuffer(unsigned int bufferName)=0;
	virtual void SetVertexAttrib(unsigned int bufferName, unsigned int attribID, int size, unsigned int type, bool normalized, unsigned int stride, void * offset, const Locations* locs = nullptr)=0;
	virtual void SetVertexAttribDivisor(unsigned int bufferName, int attribute_location, int divisor) {};

	virtual void DoDelayedAction()=0;

	// called at the end of drawElements or DrawArray to mark already used vertex array
	virtual void MarkVertexAttrib() { ; }
	// called in drawElements or DrawArray to disable unused attributes
	virtual void FlushUnusedVertexAttrib() { ; }



	virtual size_t GetAllocatedBufferCount() { return 0; }

};

#define PREALLOCATED_VBO_COUNT					64

// ****************************************
// * ModuleSpecificRenderer class
// * --------------------------------------
/**
 * \file	ModuleRenderer.h
 * \class	ModuleSpecificRenderer
 * \ingroup Renderer
 * \ingroup Module
 * \brief	Abstract base class for specific renderers, so that they must overload some generic functionnality
 */
 // ****************************************
class ModuleSpecificRenderer : public ModuleBase
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(ModuleSpecificRenderer, ModuleBase, Renderer)
	DECLARE_CONSTRUCTOR(ModuleSpecificRenderer);

	/**
	 * \brief		initialize module
	 * \fn			void Init(Core* core, const kstl::vector<CoreModifiableAttribute*>* params);
	 * \param		core : link to the core, NOT NULL
	 * \param		params : list of parameters
	 */
	void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;
	virtual void PlatformInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) {}

	/**
	 * \brief		close module
	 * \fn			void Close();
	 */
	void Close() override;
	virtual void PlatformClose() {}

	/**
	 * \brief		update module
	 * \fn			virtual void Update(const Timer& timer, void* addParam);
	 * \param		timer : timer global
	 */
	void Update(const Timer& timer, void* addParam) override;
	virtual void PlatformUpdate(const Timer& timer, void* addParam) {}

	void	pushShader(ShaderBase*, TravState* state);
	void	popShader(ShaderBase*, TravState* state);
	bool	HasShader();
	virtual ShaderBase* GetActiveShader();
	virtual void	setCurrentShaderProgram(ShaderInfo* p)
	{
		mCurrentShaderProgram = p;
	}

	void SetDirtyMatrix()
	{
		mDirtyMatrix = 0xffffffff;
		mDirtyShaderMatrix = 0xffffffff;
	}

	CMSP&	getDefaultUiShader()
	{
		return mDefaultUIShader;
	}

	virtual void CreateTexture(int count, unsigned int * id) {}
	virtual void DeleteTexture(int count, unsigned int * id) {}

	virtual void ActiveTextureChannel(unsigned int channel)
	{
		mCurrentTextureUnit = channel;
	}
	virtual void EnableTexture() {}
	virtual void DisableTexture() {}
	virtual void BindTexture(RendererTextureType type, unsigned int ID) {}
	virtual void UnbindTexture(RendererTextureType type, unsigned int ID) {}

	void		SetCullMode(RendererCullMode mode)
	{
		mStateStackTop->SetCullMode(mode);
	}

	void		SetLightMode(RendererLightMode mode)
	{
		mStateStackTop->SetLightMode(mode);
	}

	void		SetAlphaTestMode(RendererAlphaTestMode mode)
	{
		mStateStackTop->SetAlphaTestMode(mode);
	}

	void		SetAlphaMode(RendererAlphaMode mode, float value)
	{
		mStateStackTop->SetAlphaMode(mode, value);
	}

	void		SetBlendMode(RendererBlendMode mode)
	{
		mStateStackTop->SetBlendMode(mode);
	}

	void		SetBlendFuncMode(RendererBlendFuncMode mode1, RendererBlendFuncMode mode2)
	{
		mStateStackTop->SetBlendFuncMode(mode1, mode2);
	}

	void		SetDepthMaskMode(RendererDepthMaskMode mode)
	{
		mStateStackTop->SetDepthMaskMode(mode);
	}

	void		SetDepthTestMode(bool mode)
	{
		mStateStackTop->SetDepthTestMode(mode);
	}

	void SetDepthTestFunc(RendererDepthTestFunc func)
	{
		mStateStackTop->SetDepthTestFunc(func);
	}

	void		SetDepthValueMode(float mode)
	{
		mStateStackTop->SetDepthValueMode(mode);
	}

	void		SetClearColorValue(float r, float g, float b, float a)
	{
		mStateStackTop->SetClearColorValue(r, g, b, a);
	}

	void		SetScissorTestMode(RendererScissorTestMode mode)
	{
		mStateStackTop->SetScissorTestMode(mode);
	}

	void		SetScissorValue(int x, int y, int width, int height)
	{
		mStateStackTop->SetScissorValue(x, y, width, height);
	}

	void		SetColor(float r, float g, float b, float a)
	{
		//myStateStackTop->SetColor(r, g, b, a);
	}

	void		SetColor(float r, float g, float b)
	{
		//myStateStackTop->SetColor(r, g, b);
	}

	void		SetLightModeli(RendererLightModeliMode name, RendererLightModeliParam param) {

		mStateStackTop->SetLightModeli(name, param);
	}

	void		SetLightModelfv(RendererLightModelfvMode name, float* param) {

		mStateStackTop->SetLightModelfv(name, param);
	}

	void		SetColorMaterialMode(bool mode) {

		mStateStackTop->SetColorMaterialMode(mode);

	}

	void		SetNormalizeMode(RendererNormalizeMode mode) {

		mStateStackTop->SetNormalizeMode(mode);

	}

	void		SetPolygonSmoothMode(RendererPolygonSmoothMode mode) {

		mStateStackTop->SetPolygonSmoothMode(mode);

	}

	void		SetFogMode(RendererFogMode mode) {

		mStateStackTop->SetFogMode(mode);

	}

	void		SetLight0Mode(RendererLight0Mode mode, unsigned int index) {

		mStateStackTop->SetLight0Mode(mode, index);
	}

	void		SetLineSmoothMode(RendererLineSmoothMode mode) {

		mStateStackTop->SetLineSmoothMode(mode);

	}

	void		SetColorMaterial(RendererColorMaterialFace face, RendererColorMaterialParam param) {

		mStateStackTop->SetColorMaterial(face, param);

	}

	void SetColorMask(bool r, bool g, bool b, bool a)
	{
		mStateStackTop->SetColorMask(r,g,b,a);
	}

	void SetStencilMode(RendererCullMode face, RendererStencilMode mode, int reference_value, u32 mask)
	{
		mStateStackTop->SetStencilMode(face, mode, reference_value, mask);
	}

	void SetStencilMask(RendererCullMode face, u32 mask)
	{
		mStateStackTop->SetStencilMask(face, mask);
	}

	void SetStencilTest(bool enable)
	{
		mStateStackTop->SetStencilTest(enable);
	}

	void SetStencilOp(RendererCullMode face, RendererStencilOp sfail, RendererStencilOp dpfail, RendererStencilOp pass)
	{
		mStateStackTop->SetStencilOp(face, sfail, dpfail, pass);
	}

	void SetPolygonMode(RendererPolygonMode mode)
	{
		mStateStackTop->SetPolygonMode(mode);
	}

	void SetArrayBuffer(unsigned int bufferName, int slot = 0)
	{
		mVertexBufferManager->SetArrayBuffer(bufferName, slot);
	}

	void SetElementBuffer(unsigned int bufferName)
	{
		mVertexBufferManager->SetElementBuffer(bufferName);
	}

	void UnsetElementBuffer(unsigned int bufferName)
	{
		mVertexBufferManager->UnbindBuffer(bufferName, KIGS_BUFFER_TARGET_ELEMENT);
	}

	void UnsetArrayBuffer(unsigned int bufferName)
	{
		mVertexBufferManager->UnbindBuffer(bufferName, KIGS_BUFFER_TARGET_ARRAY);
	}

	void BufferData(unsigned int bufferName, unsigned int bufftype, int size, void* data, unsigned int usage)
	{
		mVertexBufferManager->BufferData(bufferName, bufftype, size, data, usage);
	}

	void SetVertexAttrib(unsigned int bufferName, unsigned int attribID, int size, unsigned int type, bool normalized, unsigned int stride, void * offset, const Locations *locs=nullptr)
	{
		mVertexBufferManager->SetVertexAttrib(bufferName, attribID, size, type, normalized, stride, offset, locs);
	}

	virtual void SetVertexAttribDivisor(TravState* state, unsigned int bufferName, int attribute_location, int divisor) = 0;


	virtual void	TextureParameteri(RendererTextureType type, RendererTexParameter1 name, RendererTexParameter2 param) {};

	// rendering state management
	virtual void	FlushState(bool force = false);	// make wanted state current (to be called just before real draw to avoid useless state changes)
	void	PushState();					// backup wanted state on stack to be able to come back to previous state with PopState
	void	PopState();
	void	ClearView(RendererClearMode clearMode);
	void	Viewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
	void	FlushLightModelfv();
	void	FlushLightModeli();

	void			Frustum(int mode, kfloat left, kfloat right, kfloat bottom, kfloat top, kfloat zNear, kfloat zFar);
	void			Ortho(int mode, kfloat left, kfloat right, kfloat bottom, kfloat top, kfloat zNear, kfloat zFar);
	void			Perspective(int mode, kfloat fovy, kfloat aspect, kfloat nearZ, kfloat farZ);
	void			LookAt(int mode, kfloat eyex, kfloat eyey, kfloat eyez, kfloat centerx, kfloat centery, kfloat centerz, kfloat upx, kfloat upy, kfloat upz);


	// matrix

	inline void			PushMatrix(int mode)
	{
		mMatrixStack[mode].push_back(mMatrixStack[mode].back());
		mDirtyMatrix |= (1 << mode);
	}
	inline void			PushAndLoadMatrix(int mode,const Matrix4x4& m)
	{
		mMatrixStack[mode].push_back(m);
		mDirtyMatrix |= (1 << mode);
	}

	inline void			PopMatrix(int mode)
	{
		mDirtyMatrix |= (1 << mode);
		mMatrixStack[mode].pop_back();
	}

	void			LoadIdentity(int mode);
	void			LoadMatrix(int mode, const kfloat *newMat);
	void			Translate(int mode, kfloat x, kfloat y, kfloat z);
	void			Rotate(int mode, kfloat angle, kfloat x, kfloat y, kfloat z);
	void			Scale(int mode, kfloat x, kfloat y, kfloat z);
	void			Multiply(int mode, const kfloat *m)
	{
		mDirtyMatrix |= (1 << mode);
		Matrix4x4& m1 = mMatrixStack[mode].back();
		Matrix4x4& m2 = *((Matrix4x4*)m);
		//myMatrixStack[mode].back().Mult(m1, m2);
		m1 *= m2;
	}

	inline void			PushAndMultMatrix(int mode, const kfloat *m)
	{
		mDirtyMatrix |= (1 << mode);
		Matrix4x4& m1 = mMatrixStack[mode].back();
		Matrix4x4& m2 = *((Matrix4x4*)m);
		mMatrixStack[mode].push_back();
		mMatrixStack[mode].back().Mult(m2, m1);
	}


	virtual void	FlushMatrix(TravState* state)
	{
		ProtectedFlushMatrix(state);
		mDirtyMatrix = 0;
	}

	RenderingScreen*	getFirstRenderingScreen()
	{
		return mFirstRenderingScreen;
	}

	// only register first one
	void	RegisterRenderingScreen(RenderingScreen* toregister)
	{
		if (mFirstRenderingScreen == 0)
		{
			mFirstRenderingScreen = toregister;
		}
	}

	// Direct rendering method for UI
	virtual void DrawUIQuad(TravState * state, const UIVerticesInfo * qi) = 0;
	virtual void DrawUITriangles(TravState * state, const UIVerticesInfo * qi) = 0;

	void CreateBuffer(int count, unsigned int * id);
	void DeleteBuffer(int count, unsigned int * id);

	virtual void InitHardwareState()
	{
		mCurrentState->InitHardwareState();
		initVBOs();
	}

	virtual void UninitHardwareState()
	{
		mCurrentVBO = -1;
		mUICurrentVBO = -1;
		mCurrentState->UninitHardwareState();
	}

	struct LightCount
	{
		int mSpot;
		int mPoint;
		int mDir;
	};
	virtual LightCount SetLightsInfo(kstl::set<CoreModifiable*>* lights) { return {}; }
	virtual void SendLightsInfo(TravState* travstate) {}
	virtual void ClearLightsInfo(TravState* travstate) {}

	virtual void	startFrame(TravState* state)
	{
		mActivatedScreenList.clear();
	}

	void	addActivatedRenderingScreen(CoreModifiable* renderingscreen)
	{
		mActivatedScreenList.insert(renderingscreen);
	}

	virtual void	endFrame(TravState* state);

	virtual void DrawPendingInstances(TravState* state) = 0;

	VertexBufferManagerBase*	GetVertexBufferManager()
	{
		return mVertexBufferManager.get();
	}

	void	initVBOs();
	unsigned int getVBO()
	{
		unsigned int index = mCurrentVBO;
		// 4 vbo are used for ui quads
		mCurrentVBO = (mCurrentVBO + 1) % (PREALLOCATED_VBO_COUNT - 4);
		return mVBO[index];
	}
	unsigned int getUIVBO()
	{
		unsigned int index = (PREALLOCATED_VBO_COUNT - 4) + mUICurrentVBO;
		// 4 vbo are used for ui quads
		mUICurrentVBO = (mUICurrentVBO + 1) % 4;
		return mVBO[index];
	}


	virtual void	DrawArrays(TravState* state, unsigned int mode, int first, int count)=0;
	virtual void	DrawElements(TravState* state, unsigned int mode, int count, unsigned int type, void* indices = 0)=0;
	virtual void	DrawElementsInstanced(TravState* state, unsigned int mode, int count, unsigned int type, void* indices, int primcount) = 0;


	virtual bool    BeginOcclusionQuery(TravState* state, u64& query_id, RendererQueryType type, int frames_to_keep = 1) { KIGS_ASSERT(!"Occlusion queries not supported"); return false; }
	virtual void    EndOcclusionQuery(TravState* state, u64 query_id) { KIGS_ASSERT(!"Occlusion queries not supported"); }
	virtual bool    GetOcclusionQueryResult(TravState* state, u64 query_id, u64& result, int frames_to_extend_if_not_ready = 0) { KIGS_ASSERT(!"Occlusion queries not supported"); return false; }

protected:

	virtual void	ProtectedFlushMatrix(TravState* state) = 0;	// rendering specific


	RenderingState*						mCurrentState;
	RenderingState*						mStateStackTop;
	kstl::vector<RenderingState*>		mWantedState;

	virtual RenderingState*	createNewState(RenderingState* toCopy = 0) = 0;

	// matrix management

	FixedSizeStack<Matrix4x4, 32>						mMatrixStack[3];
	unsigned int										mDirtyMatrix;			// flag to check if matrix stack was modified

	int													mCurrentTextureUnit=0;
	RenderingScreen*									mFirstRenderingScreen;

	kstl::set<CoreModifiable*>	mActivatedScreenList;

	ShaderBase*						mCurrentShader;
	ShaderInfo*						mCurrentShaderProgram;
	kstl::vector<ShaderBase*>		mShaderStack;
	unsigned int					mDirtyShaderMatrix;	// set when shader has changed, so we have to push again matrix

	CMSP	mDefaultUIShader;


	std::unique_ptr<VertexBufferManagerBase> mVertexBufferManager;

	unsigned int		mVBO[PREALLOCATED_VBO_COUNT];
	unsigned int		mCurrentVBO;
	unsigned int		mUICurrentVBO;

};

#endif //_MODULERENDERER_H_
