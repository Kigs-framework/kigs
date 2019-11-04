#ifndef _MODULERENDERER_H_
#define _MODULERENDERER_H_

#include "ModuleBase.h"

#include "FixedSizeStack.h"
#include "UIVerticesInfo.h"
#include "Shader.h"

#include "CoreSTL.h"
#include "TecLibs/TecHash.h"



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
	v4f ambiant;
};
struct DirLight
{
	v4f position;
	v4f diffuse;
	v4f specular;
	v4f ambiant;
};
struct SpotLight
{
	v4f position;
	v4f directionAndCutOff;
	v4f attenuationAndSpotExponent;
	v4f diffuse;
	v4f specular;
	v4f ambiant;
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
			hash_combine(hash, pl.position, pl.attenuation, pl.specular, pl.diffuse, pl.ambiant);
			return hash;
		}
	};

	template<>
	struct hash<DirLight>
	{
		size_t operator()(const DirLight& dl)
		{
			size_t hash = 0;
			hash_combine(hash, dl.position, dl.specular, dl.diffuse, dl.ambiant);
			return hash;
		}
	};

	template<>
	struct hash<SpotLight>
	{
		size_t operator()(const SpotLight& sl)
		{
			size_t hash = 0;
			hash_combine(hash, sl.position, sl.directionAndCutOff, sl.attenuationAndSpotExponent, sl.diffuse, sl.specular, sl.ambiant);
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

/*! \defgroup Renderer Renderer module
 *  manage rendering
 */
 /*! \defgroup RendererDrawable Drawable submodule
  *  \ingroup Renderer
  */

  // ****************************************
  // * ModuleRenderer class
  // * --------------------------------------
  /**
   * \file	ModuleRenderer.h
   * \class	ModuleRenderer
   * \ingroup Renderer
   * \ingroup Module
   * \brief	Renderer module
   * \author	ukn
   * \version ukn
   * \date	ukn
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
		return mySpecificRenderer;
	}

	static ModuleSpecificRenderer * theGlobalRenderer;

protected:	
	ModuleSpecificRenderer*		mySpecificRenderer;
};

// class keeping current rendering state
class RenderingState
{
public:
	virtual ~RenderingState() = default;

	void InitHardwareState()
	{
		if (!myHardwareWasInit)
		{
			ProtectedInitHardwareState();
			myHardwareWasInit = true;
		}
	}

	void UninitHardwareState()
	{
		myHardwareWasInit = false;
	}

	virtual void FlushState(RenderingState* currentState, bool force = false) = 0;	// copy this to currentState effective change currentState
	virtual void FlushLightModeli(RenderingState* currentState) = 0;
	virtual void FlushLightModelfv(RenderingState* currentState) = 0;
	
	void SetCullMode(RendererCullMode mode)
	{
		myGlobalCullFlag = mode;
	}

	void SetLightMode(RendererLightMode mode)
	{
		myGlobalLightFlag = (mode == 1);
	}

	void SetAlphaTestMode(RendererAlphaTestMode mode)
	{
		myGlobalAlphaTestFlag = (mode == 1);
	}

	void SetAlphaMode(RendererAlphaMode mode, float value)
	{
		myGlobalAlphaFlag = mode;
		myGlobalAlphaValueFlag = value;
	}

	void SetBlendMode(RendererBlendMode mode)
	{
		myGlobalBlendFlag = (mode == 1);
	}

	void SetBlendFuncMode(RendererBlendFuncMode mode1, RendererBlendFuncMode mode2)
	{
		myGlobalBlendValue1Flag = mode1;
		myGlobalBlendValue2Flag = mode2;
	}

	void SetDepthMaskMode(RendererDepthMaskMode mode)
	{
		myGlobalDepthMaskFlag = (mode == 1);
	}

	void SetDepthTestMode(bool mode)
	{
		myGlobalDepthTestFlag = mode;
	}

	void SetDepthValueMode(float mode)
	{
		myGlobalDepthValueFlag = mode;
	}

	void SetClearColorValue(float r, float g, float b, float a)
	{
		myGlobalRedClearValueFlag = r;
		myGlobalGreenClearValueFlag = g;
		myGlobalBlueClearValueFlag = b;
		myGlobalAlphaClearValueFlag = a;
	}

	virtual void ClearView(RendererClearMode clearMode) = 0;

	virtual void Viewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {	}

	void SetScissorTestMode(RendererScissorTestMode mode)
	{
		myGlobalScissorTestFlag = (mode == 1);
	}

	void SetScissorValue(int x, int y, int width, int height)
	{
		myGlobalScissorXFlag = x;
		myGlobalScissorYFlag = y;
		myGlobalScissorWidthFlag = width;
		myGlobalScissorHeightFlag = height;
	}

	void SetLightModeli(RendererLightModeliMode name, RendererLightModeliParam param) {

		myGlobalLightModeliFlag = name;
		myGlobalLightModeliParamFlag = param;
	}

	void SetLightModelfv(RendererLightModelfvMode name, float* param) {

		myGlobalLightModelfvFlag = name;
		myGlobalLightModelfvParamFlag[0] = param[0];
		myGlobalLightModelfvParamFlag[1] = param[1];
		myGlobalLightModelfvParamFlag[2] = param[2];
		myGlobalLightModelfvParamFlag[3] = param[3];
	}

	void SetColorMaterialMode(bool mode) { myGlobalColorMaterialFlag = mode; }

	void SetNormalizeMode(RendererNormalizeMode mode) {

		myGlobalNormalizeFlag = (mode == 1);
	}

	void SetPolygonSmoothMode(RendererPolygonSmoothMode mode) {

		myGlobalPolygonSmoothFlag = (mode == 1);

	}

	void SetFogMode(RendererFogMode mode) {

		myGlobalFogFlag = (mode == 1);
	}

	void SetLight0Mode(RendererLight0Mode mode, unsigned int index) {
		myGlobalLight0Flag = (mode == 1);
		myGlobalLight0IndexFlag = index;
	}

	void SetLineSmoothMode(RendererLineSmoothMode mode) {

		myGlobalLineSmoothFlag = (mode == 1);

	}

	void SetColorMaterial(RendererColorMaterialFace face, RendererColorMaterialParam param) {

		myGlobalColorMaterialFaceFlag = face;
		myGlobalColorMaterialParamFlag = param;

	}

	void SetColorMask(bool r, bool g, bool b, bool a)
	{
		myGlobalColorMask[0] = r;
		myGlobalColorMask[1] = g;
		myGlobalColorMask[2] = b;
		myGlobalColorMask[3] = a;
	}

	void SetStencilMode(RendererCullMode face, RendererStencilMode mode, int reference_value, u32 mask)
	{
		if (face == RendererCullMode::RENDERER_CULL_FRONT || face == RendererCullMode::RENDERER_CULL_FRONT_AND_BACK)
		{
			myGlobalStencilMode[0] = mode;
			myGlobalStencilFuncRef[0] = reference_value;
			myGlobalStencilFuncMask[0] = mask;
		}
		if (face == RendererCullMode::RENDERER_CULL_BACK || face == RendererCullMode::RENDERER_CULL_FRONT_AND_BACK)
		{
			myGlobalStencilMode[1] = mode;
			myGlobalStencilFuncRef[1] = reference_value;
			myGlobalStencilFuncMask[1] = mask;
		}
	}

	void SetStencilMask(RendererCullMode face, u32 mask)
	{
		if (face == RendererCullMode::RENDERER_CULL_FRONT || face == RendererCullMode::RENDERER_CULL_FRONT_AND_BACK)
			myGlobalStencilMask[0] = mask;
		if (face == RendererCullMode::RENDERER_CULL_BACK || face == RendererCullMode::RENDERER_CULL_FRONT_AND_BACK)
			myGlobalStencilMask[1] = mask;
	}

	void SetStencilOp(RendererCullMode face, RendererStencilOp sfail, RendererStencilOp dpfail, RendererStencilOp pass)
	{
		if (face == RendererCullMode::RENDERER_CULL_FRONT || face == RendererCullMode::RENDERER_CULL_FRONT_AND_BACK)
		{
			myGlobalStencilOpSFail[0] = sfail;
			myGlobalStencilOpDPFail[0] = dpfail;
			myGlobalStencilOpPass[0] = pass;
		}
		if (face == RendererCullMode::RENDERER_CULL_BACK || face == RendererCullMode::RENDERER_CULL_FRONT_AND_BACK)
		{
			myGlobalStencilOpSFail[1] = sfail;
			myGlobalStencilOpDPFail[1] = dpfail;
			myGlobalStencilOpPass[1] = pass;
		}
	}

	void SetStencilTest(bool enable)
	{
		myGlobalStencilEnabled = enable;
	}

	void SetPolygonMode(RendererPolygonMode mode)
	{
		myPolygonMode = mode;
	}

protected:
	virtual void ProtectedInitHardwareState() = 0;

	//! global cull flag 
	int		myGlobalCullFlag = RENDERER_CULL_NONE;
	int		myGlobalAlphaFlag = RENDERER_ALPHA_ALWAYS;
	float	myGlobalAlphaValueFlag = 0.0f;

	int		myGlobalBlendValue1Flag = RENDERER_BLEND_ONE;
	int		myGlobalBlendValue2Flag = RENDERER_BLEND_ZERO;

	float	myGlobalDepthValueFlag = 1.0f;
	union {
		struct {
			float	myGlobalRedClearValueFlag;
			float	myGlobalGreenClearValueFlag;
			float	myGlobalBlueClearValueFlag;
			float	myGlobalAlphaClearValueFlag;
		};
		float myGlobalClearValueFlag[4] = {0,0,0,0};
	};
	int		myGlobalScissorXFlag = 0;
	int		myGlobalScissorYFlag = 0;
	int		myGlobalScissorWidthFlag = 0;
	int		myGlobalScissorHeightFlag = 0;
	int		myGlobalLight0IndexFlag = 0;
	int		myGlobalColorMaterialFaceFlag = 0;
	int		myGlobalColorMaterialParamFlag = 0;

	int		myGlobalLightModeliFlag = 0;
	int		myGlobalLightModeliParamFlag = 0;
	int		myGlobalLightModelfvFlag = 0;
	float	myGlobalLightModelfvParamFlag[4] = {};

	bool	myGlobalLightModelFlag = true;
	bool	myGlobalColorMaterialFlag = false;
	bool	myGlobalNormalizeFlag = false;
	bool	myGlobalPolygonSmoothFlag = false;
	bool	myGlobalFogFlag = false;
	bool	myGlobalLight0Flag = false;
	bool	myGlobalScissorTestFlag = RENDERER_SCISSOR_TEST_OFF;
	bool	myGlobalDepthMaskFlag = RENDERER_DEPTH_MASK_ON;
	bool	myGlobalDepthTestFlag = false;
	bool	myGlobalBlendFlag = RENDERER_BLEND_OFF;
	bool	myGlobalLightFlag = RENDERER_LIGHT_OFF;
	bool	myGlobalAlphaTestFlag = RENDERER_ALPHA_TEST_OFF;
	bool	myGlobalLineSmoothFlag = RENDERER_LINE_SMOOTH_OFF;

	int		myPolygonMode = RENDERER_FILL;

	bool    myGlobalColorMask[4] = { true, true, true, true };
	bool	myGlobalStencilEnabled = false;
	u32		myGlobalStencilMask[2] = { 0xFFFFFFFF,0xFFFFFFFF };
	RendererStencilMode		myGlobalStencilMode[2] = { RENDERER_STENCIL_ALWAYS,RENDERER_STENCIL_ALWAYS };
	int		myGlobalStencilFuncRef[2] = {};
	u32		myGlobalStencilFuncMask[2] = { 0xFFFFFFFF,0xFFFFFFFF };
	RendererStencilOp		myGlobalStencilOpSFail[2] = { RENDERER_STENCIL_OP_KEEP,RENDERER_STENCIL_OP_KEEP };
	RendererStencilOp		myGlobalStencilOpDPFail[2] = { RENDERER_STENCIL_OP_KEEP,RENDERER_STENCIL_OP_KEEP };
	RendererStencilOp		myGlobalStencilOpPass[2] = { RENDERER_STENCIL_OP_KEEP,RENDERER_STENCIL_OP_KEEP };

	bool	myHardwareWasInit = false;
};

static const int MATRIX_MODE_MODEL = 0;
static const int MATRIX_MODE_PROJECTION = 1;
static const int MATRIX_MODE_VIEW = 2;


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

	virtual size_t GetAllocatedBufferCount() { return 0; }

};

#define PREALLOCATED_VBO_COUNT					64

// base class for specific renderers, so that they must overload some generic functionnality
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
		myCurrentShaderProgram = p;
	}



	int getSpotLightCount() { return mySpotLightCount; }
	int getPointLightCount() { return myPointLightCount; }
	int getDirLightCount() { return myDirLightCount; }


	CoreModifiable*	getDefaultUiShader()
	{
		return myDefaultUIShader;
	}

	virtual void CreateTexture(int count, unsigned int * id) {}
	virtual void DeleteTexture(int count, unsigned int * id) {}

	virtual void ActiveTextureChannel(unsigned int channel)
	{
		myCurrentTextureUnit = channel;
	}
	virtual void EnableTexture() {}
	virtual void DisableTexture() {}
	virtual void BindTexture(RendererTextureType type, unsigned int ID) {}
	virtual void UnbindTexture(RendererTextureType type, unsigned int ID) {}

	void		SetCullMode(RendererCullMode mode)
	{
		myStateStackTop->SetCullMode(mode);
	}

	void		SetLightMode(RendererLightMode mode)
	{
		myStateStackTop->SetLightMode(mode);
	}

	void		SetAlphaTestMode(RendererAlphaTestMode mode)
	{
		myStateStackTop->SetAlphaTestMode(mode);
	}

	void		SetAlphaMode(RendererAlphaMode mode, float value)
	{
		myStateStackTop->SetAlphaMode(mode, value);
	}

	void		SetBlendMode(RendererBlendMode mode)
	{
		myStateStackTop->SetBlendMode(mode);
	}

	void		SetBlendFuncMode(RendererBlendFuncMode mode1, RendererBlendFuncMode mode2)
	{
		myStateStackTop->SetBlendFuncMode(mode1, mode2);
	}

	void		SetDepthMaskMode(RendererDepthMaskMode mode)
	{
		myStateStackTop->SetDepthMaskMode(mode);
	}

	void		SetDepthTestMode(bool mode)
	{
		myStateStackTop->SetDepthTestMode(mode);
	}

	void		SetDepthValueMode(float mode)
	{
		myStateStackTop->SetDepthValueMode(mode);
	}

	void		SetClearColorValue(float r, float g, float b, float a)
	{
		myStateStackTop->SetClearColorValue(r, g, b, a);
	}

	void		SetScissorTestMode(RendererScissorTestMode mode)
	{
		myStateStackTop->SetScissorTestMode(mode);
	}

	void		SetScissorValue(int x, int y, int width, int height)
	{
		myStateStackTop->SetScissorValue(x, y, width, height);
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

		myStateStackTop->SetLightModeli(name, param);
	}

	void		SetLightModelfv(RendererLightModelfvMode name, float* param) {

		myStateStackTop->SetLightModelfv(name, param);
	}

	void		SetColorMaterialMode(bool mode) {

		myStateStackTop->SetColorMaterialMode(mode);

	}

	void		SetNormalizeMode(RendererNormalizeMode mode) {

		myStateStackTop->SetNormalizeMode(mode);

	}

	void		SetPolygonSmoothMode(RendererPolygonSmoothMode mode) {

		myStateStackTop->SetPolygonSmoothMode(mode);

	}

	void		SetFogMode(RendererFogMode mode) {

		myStateStackTop->SetFogMode(mode);

	}

	void		SetLight0Mode(RendererLight0Mode mode, unsigned int index) {

		myStateStackTop->SetLight0Mode(mode, index);
	}

	void		SetLineSmoothMode(RendererLineSmoothMode mode) {

		myStateStackTop->SetLineSmoothMode(mode);

	}

	void		SetColorMaterial(RendererColorMaterialFace face, RendererColorMaterialParam param) {

		myStateStackTop->SetColorMaterial(face, param);

	}

	void SetColorMask(bool r, bool g, bool b, bool a)
	{
		myStateStackTop->SetColorMask(r,g,b,a);
	}

	void SetStencilMode(RendererCullMode face, RendererStencilMode mode, int reference_value, u32 mask)
	{
		myStateStackTop->SetStencilMode(face, mode, reference_value, mask);
	}

	void SetStencilMask(RendererCullMode face, u32 mask)
	{
		myStateStackTop->SetStencilMask(face, mask);
	}

	void SetStencilTest(bool enable)
	{
		myStateStackTop->SetStencilTest(enable);
	}

	void SetStencilOp(RendererCullMode face, RendererStencilOp sfail, RendererStencilOp dpfail, RendererStencilOp pass)
	{
		myStateStackTop->SetStencilOp(face, sfail, dpfail, pass);
	}

	void SetPolygonMode(RendererPolygonMode mode)
	{
		myStateStackTop->SetPolygonMode(mode);
	}

	void SetArrayBuffer(unsigned int bufferName, int slot = 0)
	{
		myVertexBufferManager->SetArrayBuffer(bufferName, slot);
	}

	void SetElementBuffer(unsigned int bufferName)
	{
		myVertexBufferManager->SetElementBuffer(bufferName);
	}

	void UnsetElementBuffer(unsigned int bufferName)
	{
		myVertexBufferManager->UnbindBuffer(bufferName, KIGS_BUFFER_TARGET_ELEMENT);
	}

	void UnsetArrayBuffer(unsigned int bufferName)
	{
		myVertexBufferManager->UnbindBuffer(bufferName, KIGS_BUFFER_TARGET_ARRAY);
	}

	void BufferData(unsigned int bufferName, unsigned int bufftype, int size, void* data, unsigned int usage)
	{
		myVertexBufferManager->BufferData(bufferName, bufftype, size, data, usage);
	}

	void SetVertexAttrib(unsigned int bufferName, unsigned int attribID, int size, unsigned int type, bool normalized, unsigned int stride, void * offset, const Locations *locs=nullptr)
	{
		myVertexBufferManager->SetVertexAttrib(bufferName, attribID, size, type, normalized, stride, offset, locs);
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
		myMatrixStack[mode].push_back(myMatrixStack[mode].back());
		myDirtyMatrix |= (1 << mode);
	}
	inline void			PushAndLoadMatrix(int mode,const Matrix4x4& m)
	{
		myMatrixStack[mode].push_back(m);
		myDirtyMatrix |= (1 << mode);
	}

	inline void			PopMatrix(int mode)
	{
		myDirtyMatrix |= (1 << mode);
		myMatrixStack[mode].pop_back();
	}

	void			LoadIdentity(int mode);
	void			LoadMatrix(int mode, const kfloat *newMat);
	void			Translate(int mode, kfloat x, kfloat y, kfloat z);
	void			Rotate(int mode, kfloat angle, kfloat x, kfloat y, kfloat z);
	void			Scale(int mode, kfloat x, kfloat y, kfloat z);
	void			Multiply(int mode, const kfloat *m)
	{
		myDirtyMatrix |= (1 << mode);
		Matrix4x4& m1 = myMatrixStack[mode].back();
		Matrix4x4& m2 = *((Matrix4x4*)m);
		//myMatrixStack[mode].back().Mult(m1, m2);
		m1 *= m2;
	}

	inline void			PushAndMultMatrix(int mode, const kfloat *m)
	{
		myDirtyMatrix |= (1 << mode);
		Matrix4x4& m1 = myMatrixStack[mode].back();
		Matrix4x4& m2 = *((Matrix4x4*)m);
		myMatrixStack[mode].push_back();
		myMatrixStack[mode].back().Mult(m2, m1);
	}


	virtual void	FlushMatrix(TravState* state)
	{
		ProtectedFlushMatrix(state);
		myDirtyMatrix = 0;
	}

	RenderingScreen*	getFirstRenderingScreen()
	{
		return myFirstRenderingScreen;
	}

	// only register first one
	void	RegisterRenderingScreen(RenderingScreen* toregister)
	{
		if (myFirstRenderingScreen == 0)
		{
			myFirstRenderingScreen = toregister;
		}
	}

	// Direct rendering method for UI
	virtual void DrawUIQuad(TravState * state, const UIVerticesInfo * qi) = 0;
	virtual void DrawUITriangles(TravState * state, const UIVerticesInfo * qi) = 0;

	void CreateBuffer(int count, unsigned int * id);
	void DeleteBuffer(int count, unsigned int * id);

	virtual void InitHardwareState()
	{
		myCurrentState->InitHardwareState();
		initVBOs();
	}

	virtual void UninitHardwareState()
	{
		myCurrentVBO = -1;
		myUICurrentVBO = -1;
		myCurrentState->UninitHardwareState();
	}

	virtual void SetLightsInfo(kstl::set<CoreModifiable*>*lights) {}
	virtual void SendLightsInfo(TravState* travstate) {}
	virtual void ClearLightsInfo(TravState* travstate) {}

	virtual void	startFrame(TravState* state)
	{
		myActivatedScreenList.clear();
	}

	void	addActivatedRenderingScreen(CoreModifiable* renderingscreen)
	{
		myActivatedScreenList.insert(renderingscreen);
	}

	void	endFrame(TravState* state);

	virtual void DrawPendingInstances(TravState* state) = 0;

	VertexBufferManagerBase*	GetVertexBufferManager()
	{
		return myVertexBufferManager.get();
	}

	void	initVBOs();
	unsigned int getVBO()
	{
		unsigned int index = myCurrentVBO;
		// 4 vbo are used for ui quads
		myCurrentVBO = (myCurrentVBO + 1) % (PREALLOCATED_VBO_COUNT - 4);
		return myVBO[index];
	}
	unsigned int getUIVBO()
	{
		unsigned int index = (PREALLOCATED_VBO_COUNT - 4) + myUICurrentVBO;
		// 4 vbo are used for ui quads
		myUICurrentVBO = (myUICurrentVBO + 1) % 4;
		return myVBO[index];
	}


	virtual void	DrawArrays(TravState* state, unsigned int mode, int first, int count)=0;
	virtual void	DrawElements(TravState* state, unsigned int mode, int count, unsigned int type, void* indices = 0, bool unused = false)=0;
	virtual void	DrawElementsInstanced(TravState* state, unsigned int mode, int count, unsigned int type, void* indices, int primcount, bool clear_manager = true) = 0;


	virtual bool    BeginOcclusionQuery(TravState* state, u64& query_id, RendererQueryType type, int frames_to_keep = 1) { KIGS_ASSERT(!"Occlusion queries not supported"); return false; }
	virtual void    EndOcclusionQuery(TravState* state, u64 query_id) { KIGS_ASSERT(!"Occlusion queries not supported"); }
	virtual bool    GetOcclusionQueryResult(TravState* state, u64 query_id, u64& result, int frames_to_extend_if_not_ready = 0) { KIGS_ASSERT(!"Occlusion queries not supported"); return false; }

protected:

	virtual void	ProtectedFlushMatrix(TravState* state) = 0;	// rendering specific


	RenderingState*						myCurrentState;
	RenderingState*						myStateStackTop;
	kstl::vector<RenderingState*>		myWantedState;

	virtual RenderingState*	createNewState(RenderingState* toCopy = 0) = 0;

	// matrix management

	FixedSizeStack<Matrix4x4, 32>						myMatrixStack[3];
	unsigned int										myDirtyMatrix;			// flag to check if matrix stack was modified

	int													myCurrentTextureUnit=0;
	RenderingScreen*									myFirstRenderingScreen;

	kstl::set<CoreModifiable*>	myActivatedScreenList;

	ShaderBase*						myCurrentShader;
	ShaderInfo*						myCurrentShaderProgram;
	kstl::vector<ShaderBase*>		myShaderStack;
	unsigned int					myDirtyShaderMatrix;	// set when shader has changed, so we have to push again matrix

		//! One UIShader to rule them all
	CoreModifiable*		myUIShader;
	int mySpotLightCount = 0;
	int myPointLightCount = 0;
	int myDirLightCount = 0;
	CoreModifiable*	myDefaultUIShader;

	std::unique_ptr<VertexBufferManagerBase> myVertexBufferManager;

	unsigned int		myVBO[PREALLOCATED_VBO_COUNT];
	unsigned int		myCurrentVBO;
	unsigned int		myUICurrentVBO;

};

#endif //_MODULERENDERER_H_
