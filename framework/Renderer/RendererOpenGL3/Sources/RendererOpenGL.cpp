

#include "Core.h"
#include "CoreBaseApplication.h"

#include "ModuleRenderer.h"
#include "RendererOpenGL.h"
#include "RendererDefines.h"

#include "OpenGLCamera.h"
#include "OpenGLCameraOrtho.h"
#include "OpenGLRenderingScreen.h"
#include "OpenGLTexture.h"
#include "GLSLLight.h"
#include "OpenGLRenderingMatrix.h"
#include "OpenGLHolo3DPanel.h"
#include "OpenGLMaterial.h"


#include "GLSLDeferred.h"
#include "GLSLGenericMeshShader.h"
#include "GLSLSkinShader.h"
#include "GLSLShader.h"
#include "GLSLUniform.h"
#include "UIVerticesInfo.h"
#include "GLSLUIShader.h"
#include "GLSLCutShader.h"
#include "TextureFileManager.h"
#include "GLSLDebugDraw.h"
// include after all other kigs include (for iOS)
#include "Platform/Renderer/OpenGLInclude.h"
#include "Platform/Renderer/RendererOpenGL3.inl.h"
#include "ModuleSceneGraph.h"

#include "ModernMesh.h"

#include "algorithm"


using namespace Kigs::Draw;
using namespace Kigs::Draw2D;

#define mCurrentArrayBound mCurrentBoundBuffer[0]
#define mCurrentElemBound mCurrentBoundBuffer[1]
#define mAskedArrayBound mAskedBoundBuffer[0]
#define mAskedElemBound mAskedBoundBuffer[1]


#ifdef WUP
extern bool gIsHolographic;
#endif

IMPLEMENT_CLASS_INFO(RendererOpenGL)

RendererOpenGL::RendererOpenGL(const std::string& name, CLASS_NAME_TREE_ARG) : ModuleSpecificRenderer(name, PASS_CLASS_NAME_TREE_ARG)

, mCurrentOGLMatrixMode(-1)

{
	mShaderStack.clear();
	mShaderStack.reserve(16);
	mCurrentShader = NULL;
	mCurrentShaderProgram = 0;

	mVBO[0] = -1; // for init check
}

void	RendererOpenGL::ProtectedFlushMatrix(TravState* state)
{
	if (HasShader()) // load uniform
	{
		auto locations = GetActiveShader()->GetLocation();
		if ((mDirtyMatrix & 1) || mDirtyShaderMatrix)
		{
			
			if (locations->modelMatrix != -1)
			{
				glUniformMatrix4fv(locations->modelMatrix, 1, false, &(mMatrixStack[0].back().e[0][0])); CHECK_GLERROR;
			}
		}

		auto cam = state->GetCurrentCamera();
		if (mDirtyShaderMatrix && locations->fogScale != -1 && cam)
		{
			//auto D = mMatrixStack[1].back()[14];
			//auto C = mMatrixStack[1].back()[10];
			auto nearPlane = 0.1f; // D / (C - 1.0f);
			auto farPlane = 100.0f; // D / (C + 1.0f);
			cam->getValue("NearPlane", nearPlane);
			cam->getValue("FarPlane", farPlane);
			glUniform1f(locations->farPlane, farPlane); CHECK_GLERROR;
			v4f fog_color(0, 0, 0, 1);
			float fog_scale = (farPlane - nearPlane) / 10;
			cam->getValue("FogColor", fog_color);
			cam->getValue("FogScale", fog_scale);
			glUniform1f(locations->fogScale, fog_scale); CHECK_GLERROR;
			glUniform4fv(locations->fogColor, 1, &fog_color.x); CHECK_GLERROR;
		}

		// in hologtraphic mode, projection and view are provided by device

#ifdef WUP
		if (state->GetHolographicMode())
		{
			if (state->HolographicUseStackMatrix)
			{
				GLint uniLoc0 = locations->holoViewMatrixLocation[0];
				GLint uniLoc1 = locations->holoViewMatrixLocation[1];
				if (uniLoc0 == -1 || uniLoc1 == -1)
					return;

				auto viewproj = mMatrixStack[MATRIX_MODE_PROJECTION].back()*mMatrixStack[MATRIX_MODE_VIEW].back();
				glUniformMatrix4fv(uniLoc0, 1, false, &(viewproj.e[0][0])); CHECK_GLERROR;
				glUniformMatrix4fv(uniLoc1, 1, false, &(viewproj.e[0][0])); CHECK_GLERROR;
			}
			else
			{
				GLint uniLoc = locations->holoViewMatrixLocation[2];
				if (uniLoc == -1)
					return;

				glUniformMatrix4fv(uniLoc, 2, false, mHoloMatrix->mHoloMatrix); CHECK_GLERROR;
			}
		}
		else
#endif
		{
			if ((mDirtyMatrix & 2) || mDirtyShaderMatrix)
			{
				if (locations->projMatrix != -1)
				{
					glUniformMatrix4fv(locations->projMatrix, 1, false, &(mMatrixStack[1].back().e[0][0])); CHECK_GLERROR;
				}
			}

			if ((mDirtyMatrix & 4) || mDirtyShaderMatrix)
			{
				if (locations->viewMatrix != -1)
				{
					glUniformMatrix4fv(locations->viewMatrix, 1, false, &(mMatrixStack[2].back().e[0][0])); CHECK_GLERROR;
				}
			}
		}
		if ((mDirtyMatrix & 8) || mDirtyShaderMatrix)
		{
			if (locations->uvMatrix != -1)
			{
				// get only needed elements from 4x4 matrix

				mat3	uvm;
				mat4& uvm4x4 = mMatrixStack[3].back();
				uvm.e[0][0]= uvm4x4.e[0][0];
				uvm.e[0][1] = uvm4x4.e[1][0];
				uvm.e[1][0] = uvm4x4.e[0][1];
				uvm.e[1][1] = uvm4x4.e[1][1];
				uvm.e[2][0] = uvm4x4.e[0][2];
				uvm.e[2][1] = uvm4x4.e[1][2];
				uvm.e[0][2] = uvm.e[1][2] = uvm.e[2][2]=0.0f;

				glUniformMatrix3fv(locations->uvMatrix, 1, false, &(uvm.e[0][0])); CHECK_GLERROR;
			}
		}
		mDirtyShaderMatrix = 0;
	}
}

void OpenGLRenderingState::ProtectedInitHardwareState()
{
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_SCISSOR_TEST);
	glBlendColor(0, 0, 0, 1.0f);

	//glClearColor(mGlobalRedClearValueFlag, mGlobalGreenClearValueFlag, mGlobalBlueClearValueFlag, mGlobalAlphaClearValueFlag);
}


static int ConvertStencilOp(RendererStencilOp op)
{
	//GL_KEEP, GL_ZERO, GL_REPLACE, GL_INCR, GL_INCR_WRAP, GL_DECR, GL_DECR_WRAP, and GL_INVERT
	if (op == RENDERER_STENCIL_OP_KEEP)
		return GL_KEEP;
	if (op == RENDERER_STENCIL_OP_ZERO)
		return GL_ZERO;
	if (op == RENDERER_STENCIL_OP_REPLACE)
		return GL_REPLACE;
	if (op == RENDERER_STENCIL_OP_INCR)
		return GL_INCR;
	if (op == RENDERER_STENCIL_OP_INCR_WRAP)
		return GL_INCR_WRAP;
	if (op == RENDERER_STENCIL_OP_DECR)
		return GL_DECR;
	if (op == RENDERER_STENCIL_OP_DECR_WRAP)
		return GL_DECR_WRAP;
	if (op == RENDERER_STENCIL_OP_INVERT)
		return GL_INVERT;

	return GL_KEEP;
}

void OpenGLRenderingState::FlushState(RenderingState* currentState, bool force)
{
	OpenGLRenderingState* otherOne = static_cast<OpenGLRenderingState*>(currentState);
	if (force || mGlobalCullFlag != otherOne->mGlobalCullFlag)
	{
		otherOne->mGlobalCullFlag = mGlobalCullFlag;
		int cullmode = mGlobalCullFlag;

		if (cullmode == 0)
		{
			glDisable(GL_CULL_FACE); CHECK_GLERROR;
		}
		else
		{
			glEnable(GL_CULL_FACE); CHECK_GLERROR;
			if (cullmode == 1)
			{
				glCullFace(GL_BACK); CHECK_GLERROR;
			}
			else if (cullmode == 2)
			{
				glCullFace(GL_FRONT); CHECK_GLERROR;
			}
			else
			{
				glCullFace(GL_FRONT_AND_BACK); CHECK_GLERROR;
			}
		}
	}

	if (force || mGlobalBlendFlag != otherOne->mGlobalBlendFlag) {
		otherOne->mGlobalBlendFlag = mGlobalBlendFlag;
		bool blendmode = (bool)mGlobalBlendFlag;

		if (!blendmode) {
			glDisable(GL_BLEND); CHECK_GLERROR;
		}
		else {
			glEnable(GL_BLEND); CHECK_GLERROR;
		}
	}

	if (force || mGlobalBlendValue1Flag != otherOne->mGlobalBlendValue1Flag || mGlobalBlendValue2Flag != otherOne->mGlobalBlendValue2Flag) {
		otherOne->mGlobalBlendValue1Flag = mGlobalBlendValue1Flag;
		otherOne->mGlobalBlendValue2Flag = mGlobalBlendValue2Flag;
		int blendvalue1 = (int)mGlobalBlendValue1Flag;
		int blendvalue2 = (int)mGlobalBlendValue2Flag;
		GLenum sfactor,dfactor;
		//GL_ZERO, GL_ONE, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 
		//GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR, GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA, and GL_SRC_ALPHA_SATURATE
		if (blendvalue1 == 0) {
			sfactor = GL_ZERO;
		}
		else if (blendvalue1 == 1) {
			sfactor = GL_ONE;
		}
		else if (blendvalue1 == 2) {
			sfactor = GL_SRC_COLOR;
		}
		else if (blendvalue1 == 3) {
			sfactor = GL_ONE_MINUS_SRC_COLOR;
		}
		else if (blendvalue1 == 4) {
			sfactor = GL_DST_COLOR;
		}
		else if (blendvalue1 == 5) {
			sfactor = GL_ONE_MINUS_DST_COLOR;
		}
		else if (blendvalue1 == 6) {
			sfactor = GL_SRC_ALPHA;
		}
		else if (blendvalue1 == 7) {
			sfactor = GL_ONE_MINUS_SRC_ALPHA;
		}
		else if (blendvalue1 == 8) {
			sfactor = GL_DST_ALPHA;
		}
		else if (blendvalue1 == 9) {
			sfactor = GL_ONE_MINUS_DST_ALPHA;
		}
#ifdef WIN32
		else if (blendvalue1 == 10) {
			sfactor = GL_CONSTANT_COLOR;
		}
		else if (blendvalue1 == 11) {
			sfactor = GL_ONE_MINUS_CONSTANT_COLOR;
		}
		else if (blendvalue1 == 12) {
			sfactor = GL_CONSTANT_ALPHA;
		}
		else if (blendvalue1 == 13) {
			sfactor = GL_ONE_MINUS_CONSTANT_ALPHA;
		}
#endif
		else {
			sfactor = GL_SRC_ALPHA_SATURATE;
		}

		if (blendvalue2 == 0) {
			dfactor = GL_ZERO;
		}
		else if (blendvalue2 == 1) {
			dfactor = GL_ONE;
		}
		else if (blendvalue2 == 2) {
			dfactor = GL_SRC_COLOR;
		}
		else if (blendvalue2 == 3) {
			dfactor = GL_ONE_MINUS_SRC_COLOR;
		}
		else if (blendvalue2 == 4) {
			dfactor = GL_DST_COLOR;
		}
		else if (blendvalue2 == 5) {
			dfactor = GL_ONE_MINUS_DST_COLOR;
		}
		else if (blendvalue2 == 6) {
			dfactor = GL_SRC_ALPHA;
		}
		else if (blendvalue2 == 7) {
			dfactor = GL_ONE_MINUS_SRC_ALPHA;
		}
		else if (blendvalue2 == 8) {
			dfactor = GL_DST_ALPHA;
		}
		else if (blendvalue2 == 9) {
			dfactor = GL_ONE_MINUS_DST_ALPHA;
		}
#ifdef WIN32
		else if (blendvalue2 == 10) {
			dfactor = GL_CONSTANT_COLOR;
		}
		else if (blendvalue2 == 11) {
			dfactor = GL_ONE_MINUS_CONSTANT_COLOR;
		}
		else if (blendvalue2 == 12) {
			dfactor = GL_CONSTANT_ALPHA;
		}
		else {
			dfactor = GL_ONE_MINUS_CONSTANT_ALPHA;
		}
#endif
		glBlendFuncSeparate(sfactor, dfactor, GL_ONE, GL_ONE); CHECK_GLERROR;
	}

	if (force || mGlobalDepthMaskFlag != otherOne->mGlobalDepthMaskFlag) {
		otherOne->mGlobalDepthMaskFlag = mGlobalDepthMaskFlag;
		bool depthmaskmode = (bool)mGlobalDepthMaskFlag;

		if (!depthmaskmode) {
			glDepthMask(GL_FALSE); CHECK_GLERROR;
		}
		else {
			glDepthMask(GL_TRUE); CHECK_GLERROR;
		}
	}

	if (force || mGlobalDepthTestFlag != otherOne->mGlobalDepthTestFlag) {
		otherOne->mGlobalDepthTestFlag = mGlobalDepthTestFlag;
		bool depthTestmode = (bool)mGlobalDepthTestFlag;

		if (!depthTestmode) {
			glDisable(GL_DEPTH_TEST); CHECK_GLERROR;
		}
		else {
			glEnable(GL_DEPTH_TEST); CHECK_GLERROR;
		}
	}

	if (force || mGlobalDepthValueFlag != otherOne->mGlobalDepthValueFlag) {
		otherOne->mGlobalDepthValueFlag = mGlobalDepthValueFlag;
#if defined (WIN32) && !defined(WUP)
		float depthvalue = (float)mGlobalDepthValueFlag;
		glClearDepth(depthvalue); CHECK_GLERROR;
#endif
	}

	if (force || mGlobalScissorTestFlag != otherOne->mGlobalScissorTestFlag) {
		otherOne->mGlobalScissorTestFlag = mGlobalScissorTestFlag;
		bool scissorTestmode = (bool)mGlobalScissorTestFlag;

		if (!scissorTestmode) {
			glDisable(GL_SCISSOR_TEST); CHECK_GLERROR;
		}
		else {
			glEnable(GL_SCISSOR_TEST); CHECK_GLERROR;
		}
	}

	if (force || mGlobalScissorXFlag != otherOne->mGlobalScissorXFlag
		|| mGlobalScissorYFlag != otherOne->mGlobalScissorYFlag
		|| mGlobalScissorWidthFlag != otherOne->mGlobalScissorWidthFlag
		|| mGlobalScissorHeightFlag != otherOne->mGlobalScissorHeightFlag)
	{
		otherOne->mGlobalScissorXFlag = mGlobalScissorXFlag;
		otherOne->mGlobalScissorYFlag = mGlobalScissorYFlag;
		otherOne->mGlobalScissorWidthFlag = mGlobalScissorWidthFlag;
		otherOne->mGlobalScissorHeightFlag = mGlobalScissorHeightFlag;

		int x = mGlobalScissorXFlag;
		int y = mGlobalScissorYFlag;
		int width = mGlobalScissorWidthFlag;
		int height = mGlobalScissorHeightFlag;

		glScissor(x, y, width, height); CHECK_GLERROR;
	}

	if (force || mGlobalColorMaterialFaceFlag != otherOne->mGlobalColorMaterialFaceFlag || mGlobalColorMaterialParamFlag != otherOne->mGlobalColorMaterialParamFlag) {

		int face = mGlobalColorMaterialFaceFlag;
		int param = mGlobalColorMaterialParamFlag;
		otherOne->mGlobalColorMaterialFaceFlag = face;
		otherOne->mGlobalColorMaterialParamFlag = param;
	}

	

	for (int i = 0; i < 2; ++i)
	{
		auto face = i == 0 ? GL_FRONT : GL_BACK;
		if (force || mGlobalStencilMode[i] != otherOne->mGlobalStencilMode[i] || mGlobalStencilFuncMask[i] != otherOne->mGlobalStencilFuncMask[i] || mGlobalStencilFuncRef[i] != otherOne->mGlobalStencilFuncRef[i])
		{
			otherOne->mGlobalStencilMode[i] = mGlobalStencilMode[i];
			otherOne->mGlobalStencilFuncMask[i] = mGlobalStencilFuncMask[i];
			otherOne->mGlobalStencilFuncRef[i] = mGlobalStencilFuncRef[i];

			int mode = GL_ALWAYS;

			if (mGlobalStencilMode[i] == RENDERER_STENCIL_NEVER)
				mode = GL_NEVER;
			else if (mGlobalStencilMode[i] == RENDERER_STENCIL_LESS)
				mode = GL_LESS;
			else if (mGlobalStencilMode[i] == RENDERER_STENCIL_EQUAL)
				mode = GL_EQUAL;
			else if (mGlobalStencilMode[i] == RENDERER_STENCIL_LEQUAL)
				mode = GL_LEQUAL;
			else if (mGlobalStencilMode[i] == RENDERER_STENCIL_GREATER)
				mode = GL_GREATER;
			else if (mGlobalStencilMode[i] == RENDERER_STENCIL_NOTEQUAL)
				mode = GL_NOTEQUAL;
			else if (mGlobalStencilMode[i] == RENDERER_STENCIL_GEQUAL)
				mode = GL_GEQUAL;
			else if (mGlobalStencilMode[i] == RENDERER_STENCIL_ALWAYS)
				mode = GL_ALWAYS;

			glStencilFuncSeparate(face, mode, mGlobalStencilFuncRef[i], mGlobalStencilFuncMask[i]);
		}

		if (force || mGlobalStencilMask[i] != otherOne->mGlobalStencilMask[i])
		{
			otherOne->mGlobalStencilMask[i] = mGlobalStencilMask[i];
			glStencilMaskSeparate(face, mGlobalStencilMask[i]);
		}
		
		if (force || mGlobalStencilOpDPFail[i] != otherOne->mGlobalStencilOpDPFail[i]
			|| mGlobalStencilOpSFail[i] != otherOne->mGlobalStencilOpSFail[i]
			|| mGlobalStencilOpPass[i] != otherOne->mGlobalStencilOpPass[i])
		{
			otherOne->mGlobalStencilOpDPFail[i] = mGlobalStencilOpDPFail[i];
			otherOne->mGlobalStencilOpSFail[i] = mGlobalStencilOpSFail[i];
			otherOne->mGlobalStencilOpPass[i] = mGlobalStencilOpPass[i];

			glStencilOpSeparate(face, ConvertStencilOp(mGlobalStencilOpSFail[i]), ConvertStencilOp(mGlobalStencilOpDPFail[i]), ConvertStencilOp(mGlobalStencilOpPass[i]));
		}
	}
	



	if (force || mGlobalColorMask[0] != otherOne->mGlobalColorMask[0]
		|| mGlobalColorMask[1] != otherOne->mGlobalColorMask[1]
		|| mGlobalColorMask[2] != otherOne->mGlobalColorMask[2]
		|| mGlobalColorMask[3] != otherOne->mGlobalColorMask[3])
	{
		otherOne->mGlobalColorMask[0] = mGlobalColorMask[0];
		otherOne->mGlobalColorMask[1] = mGlobalColorMask[1];
		otherOne->mGlobalColorMask[2] = mGlobalColorMask[2];
		otherOne->mGlobalColorMask[3] = mGlobalColorMask[3];

		glColorMask(mGlobalColorMask[0], mGlobalColorMask[1], mGlobalColorMask[2], mGlobalColorMask[3]);
	}

	if (force || mGlobalStencilEnabled != otherOne->mGlobalStencilEnabled)
	{
		otherOne->mGlobalStencilEnabled = mGlobalStencilEnabled;
		if (mGlobalStencilEnabled)
			glEnable(GL_STENCIL_TEST);
		else
			glDisable(GL_STENCIL_TEST);
	}
#if !defined(WUP) && !defined(_M_ARM) && !defined(GL_ES2)
	if (force || mPolygonMode != otherOne->mPolygonMode)
	{
		otherOne->mPolygonMode = mPolygonMode;
		switch (mPolygonMode)
		{
		case RENDERER_POINT:
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			break;
		case RENDERER_LINE:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
		case RENDERER_FILL:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		}
	}
#endif
}

void OpenGLRenderingState::ClearView(RendererClearMode clearMode)
{
	if (clearMode == RENDERER_CLEAR_NONE)
		return;

	int glMode = 0;
	if (clearMode & RENDERER_CLEAR_COLOR)
	{
		glMode |= GL_COLOR_BUFFER_BIT;
		glClearColor(mGlobalRedClearValueFlag, mGlobalGreenClearValueFlag, mGlobalBlueClearValueFlag, mGlobalAlphaClearValueFlag); CHECK_GLERROR;
	}
		
	if (clearMode & RENDERER_CLEAR_DEPTH)
		glMode |= GL_DEPTH_BUFFER_BIT;
	if (clearMode & RENDERER_CLEAR_STENCIL)
		glMode |= GL_STENCIL_BUFFER_BIT;
	
	glClear(glMode); CHECK_GLERROR;
}

void OpenGLRenderingState::Viewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	glViewport(x, y, width, height);
}

#ifdef WIN32
extern "C"
{
	static void APIENTRY doNothing(GLenum texture)
	{

	}
}
#endif

using namespace Kigs::Utils;

void RendererOpenGL::Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params)
{
	BaseInit(core, "RendererOpenGL", params);
	DECLARE_FULL_CLASS_INFO(core, OpenGLRenderingMatrix, RendererMatrix, Renderer)
	DECLARE_FULL_CLASS_INFO(core, OpenGLRenderingScreen, RenderingScreen, Renderer)

	DECLARE_FULL_CLASS_INFO(core, DebugDraw, DebugDraw, Renderer);
	//DECLARE_FULL_CLASS_INFO(core, OpenGLTexture, Texture, Renderer)
	DECLARE_CLASS_INFO_WITHOUT_FACTORY(OpenGLTexture, "Texture");
	RegisterClassToInstanceFactory( core, "Renderer", "Texture",
		[](const std::string& instancename, std::vector<CoreModifiableAttribute*>* args) -> CMSP
	{
		if (args && args->size())
		{
			return OpenGLTexture::CreateInstance(instancename, args);
		}
		auto texfileManager = KigsCore::Singleton<TextureFileManager>();
		SP<Texture> texture = texfileManager->GetTexture(instancename, false);
		return texture;
	} );

	
	DECLARE_FULL_CLASS_INFO(core, OpenGLCamera, Camera, Renderer)
	DECLARE_FULL_CLASS_INFO(core, OpenGLCameraOrtho, CameraOrtho, Renderer)
	DECLARE_FULL_CLASS_INFO(core, OpenGLMaterial, Material, Renderer)
	DECLARE_FULL_CLASS_INFO(core, OpenGLHolo3DPanel, Holo3DPanel, Renderer)
	DECLARE_FULL_CLASS_INFO(core, API3DLight, Light, Renderer)

	DECLARE_FULL_CLASS_INFO(core, API3DShader, API3DShader, Renderer)
	DECLARE_FULL_CLASS_INFO(core, API3DGenericMeshShader, API3DGenericMeshShader, Renderer)

	DECLARE_FULL_CLASS_INFO(core, API3DUniformInt, API3DUniformInt, Renderer)
	DECLARE_FULL_CLASS_INFO(core, API3DUniformFloat, API3DUniformFloat, Renderer)
	DECLARE_FULL_CLASS_INFO(core, API3DUniformFloat2, API3DUniformFloat2, Renderer)
	DECLARE_FULL_CLASS_INFO(core, API3DUniformFloat3, API3DUniformFloat3, Renderer)
	DECLARE_FULL_CLASS_INFO(core, API3DUniformFloat4, API3DUniformFloat4, Renderer)
	DECLARE_FULL_CLASS_INFO(core, API3DUniformTexture, API3DUniformTexture, Renderer)
	DECLARE_FULL_CLASS_INFO(core, API3DUniformDataTexture, API3DUniformDataTexture, Renderer)
	DECLARE_FULL_CLASS_INFO(core, API3DUniformGeneratedTexture, API3DUniformGeneratedTexture, Renderer)
	DECLARE_FULL_CLASS_INFO(core, API3DUniformMatrixArray, API3DUniformMatrixArray, Renderer)
	DECLARE_FULL_CLASS_INFO(core, API3DSkinShader, API3DSkinShader, Renderer)
	DECLARE_FULL_CLASS_INFO(core, API3DUIShader, API3DUIShader, Renderer)
	DECLARE_FULL_CLASS_INFO(core, API3DCutShader, API3DCutShader, Renderer)
	/*
#ifdef WIN32
	DECLARE_FULL_CLASS_INFO(core, API3DDeferred, API3DDeferred, Renderer)
#endif*/
	

	mVertexBufferManager = std::make_unique<VertexBufferManager>();

	if (!ModuleRenderer::mTheGlobalRenderer)
		ModuleRenderer::mTheGlobalRenderer = this;


	MAX_TEXTURE_UNIT = -1;

	ModuleSpecificRenderer::Init(core, params);

	mDefaultUIShader = KigsCore::GetInstanceOf("UIShader", "API3DUIShader");

	// ask default ui shader init
	mDefaultUIShader->Init();

	PlatformInit(core, params);
}

void RendererOpenGL::Close()
{
	

	mDefaultUIShader = nullptr;

#ifdef WIN32
#ifndef WUP
	wglMakeCurrent(NULL, NULL);
#endif
#endif

	if (mVBO[0] != -1)
	{
		glDeleteBuffers(PREALLOCATED_VBO_COUNT, mVBO); CHECK_GLERROR;
	}
	PlatformClose();
	ModuleSpecificRenderer::Close();
	BaseClose();
	ModuleRenderer::mTheGlobalRenderer = nullptr;
}

void RendererOpenGL::Update(const Timer& timer, void* addParam)
{
	PlatformUpdate(timer, addParam);
	ModuleSpecificRenderer::Update(timer, addParam);
}

SP<ModuleBase> Kigs::Draw::PlatformRendererModuleInit(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params)
{
	KigsCore::ModuleStaticInit(core);
	DECLARE_CLASS_INFO_WITHOUT_FACTORY(RendererOpenGL, "RendererOpenGL");
	auto ptr = MakeRefCounted<RendererOpenGL>("RendererOpenGL");
	ModuleBase* gInstanceRendererOpenGL = ptr.get();
	gInstanceRendererOpenGL->Init(core, params);
	return ptr;
}
/*
bool	RendererOpenGL::HasShader()
{
	return mCurrentShader != nullptr;
}

API3DShader*		RendererOpenGL::GetActiveShader()
{
	return mCurrentShader;
}

void	RendererOpenGL::pushShader(API3DShader* shad, TravState* state)
{
	bool needActive = false;
	if (mShaderStack.size())
	{
		if( (mCurrentShader != shad) || (mCurrentShaderProgram != shad->Get_ShaderProgram())) // need activation only if different shader
		{
			mCurrentShader->Deactive(state);
			needActive = true;
		}
	}
	else
	{
		needActive = true;
	}

	Material*	StateMaterial = 0;
	if (needActive)
	{
		if (state&&state->mCurrentMaterial)
		{
			Material*	toPostDraw = state->mCurrentMaterial;
			state->mCurrentMaterial = 0;
			toPostDraw->CheckPostDraw(state);

		}
	}

	mCurrentShader = shad;
	mShaderStack.push_back(shad); // must be pushed before activation
	//printf("PUSH Shader %p %s(%d)\n", shad, shad->getName().c_str(), shad->Get_ShaderProgram());

	if (needActive)
	{
		mDirtyShaderMatrix = 1;
		shad->Active(state, true);
	}

	if (StateMaterial)
		StateMaterial->CheckPreDraw(state);
}

void	RendererOpenGL::popShader(API3DShader* shad, TravState* state)
{
	// just make sure shad is the shader on top of the stack
	bool isShaderOK = false;
	if (mShaderStack.size())
	{
		if (mCurrentShader == shad)
		{
			isShaderOK = true;
		}

		mShaderStack.pop_back();

		API3DShader* shaderBack = 0;
		if (mShaderStack.size())
		{
			shaderBack = mShaderStack.back();
		}
		//printf("POP  Shader %p %s(%d)\n", shad, shad->getName().c_str(), shad->Get_ShaderProgram());


		Material*	StateMaterial = 0;
		if ((shaderBack != shad) || (mCurrentShaderProgram != shaderBack->Get_ShaderProgram()))
		{
			if (state&&state->mCurrentMaterial)
			{
				Material*	toPostDraw = state->mCurrentMaterial;
				state->mCurrentMaterial = 0;
				toPostDraw->CheckPostDraw(state);

			}

			shad->Deactive(state);

			mCurrentShader = shaderBack;

			if (mCurrentShader)
			{
				mDirtyShaderMatrix = 1;
				mCurrentShader->Active(state, true);
				
				if (StateMaterial)
					StateMaterial->CheckPreDraw(state);
			}
			else
			{
				//RendererOpenGL::setCurrentShaderProgram(0);
			}
		}

		if (isShaderOK)
		{
			return;
		}
	}
	KIGS_ERROR("bad shader pop", 1);
}
*/

void	 RendererOpenGL::setCurrentShaderProgram(ShaderInfo* p)
{
#ifdef KIGS_TOOLS
	gRendererStats.ShaderSwitch++;
#endif
	ModuleSpecificRenderer::setCurrentShaderProgram(p);
	glUseProgram(mCurrentShaderProgram->mID); CHECK_GLERROR;
}

void RendererOpenGL::FlushState(bool force)
{
	if (force)
	{
		mCurrentTextureUnit = 0;

		TextureUnitInfo tui;
		tui.mBindedTextureID = -1;
		tui.mBindedTextureType = -1;

		for (int i = 0; i < MAX_TEXTURE_UNIT; i++)
		{
			memcpy(&mTextureUnit[i], &tui, sizeof(TextureUnitInfo));
		}
	}
	ModuleSpecificRenderer::FlushState(force);
}

// # Texture Section
void RendererOpenGL::CreateTexture(int count, unsigned int * id)
{
	glGenTextures(count, id); CHECK_GLERROR;
}

void RendererOpenGL::DeleteTexture(int count, unsigned int * id)
{
	// unbind texture before delete it.
	/*TextureUnitInfo * tui = mTextureUnit;
	for (int i = 0; i < count; i++)
	{
		for (int j = 0; j < MAX_TEXTURE_UNIT; j++)
		{
			if (tui[j].mBindedTextureID == id[i])
				tui[j].mBindedTextureID = -1;
		}
	}*/

	for (int i = 0; i < count; i++)
	{
		for (auto &tui : mTextureUnit)
		{
			if (tui.mBindedTextureID == id[i])
				tui.mBindedTextureID = -1;
		}
	}


	glDeleteTextures(count, id); CHECK_GLERROR;
}

ModuleSpecificRenderer::LightCount RendererOpenGL::SetLightsInfo(std::set<CoreModifiable*>*lights)
{
	int newNumberOfDirectLights = 0;
	int newNumberOfPointLights = 0;
	int newNumberOfSpotLights = 0;

	char buf[128];

	auto itr = lights->begin();
	auto end = lights->end();
	for (; itr != end; ++itr)
	{
		API3DLight* currentLight = (API3DLight*)*itr;

		if (!currentLight->getIsOn())
			continue;


		switch (currentLight->GetTypeOfLight())
		{
		case DIRECTIONAL_LIGHT:
			/*sprintf(printbuffer, "%d", newNumberOfDirectLights);
			number = printbuffer;
			currentLight->SetUniformLocation(SPECULAR_COLOR, ("dirLights[" + number + "].specular").c_str());
			currentLight->SetUniformLocation(DIFFUSE_COLOR, ("dirLights[" + number + "].diffuse").c_str());
			currentLight->SetUniformLocation(AMBIANT_COLOR, ("dirLights[" + number + "].ambiant").c_str());
			currentLight->SetUniformLocation(POSITION_LIGHT, ("dirLights[" + number + "].position").c_str());*/
			sprintf(buf, "dirLights[%d].specular", newNumberOfDirectLights);
			currentLight->SetUniformLocation(SPECULAR_COLOR, buf);
			sprintf(buf, "dirLights[%d].diffuse", newNumberOfDirectLights);
			currentLight->SetUniformLocation(DIFFUSE_COLOR, buf);
			sprintf(buf, "dirLights[%d].ambiant", newNumberOfDirectLights);
			currentLight->SetUniformLocation(AMBIANT_COLOR, buf);
			sprintf(buf, "dirLights[%d].position", newNumberOfDirectLights);
			currentLight->SetUniformLocation(POSITION_LIGHT, buf);
			newNumberOfDirectLights++;


			break;
		case POINT_LIGHT:
			/*sprintf(printbuffer, "%d", newNumberOfPointLights);
			number = printbuffer;
			currentLight->SetUniformLocation(POSITION_LIGHT, ("pointLights[" + number + "].position").c_str());
			currentLight->SetUniformLocation(SPECULAR_COLOR, ("pointLights[" + number + "].specular").c_str());
			currentLight->SetUniformLocation(DIFFUSE_COLOR, ("pointLights[" + number + "].diffuse").c_str());
			currentLight->SetUniformLocation(AMBIANT_COLOR, ("pointLights[" + number + "].ambiant").c_str());
			currentLight->SetUniformLocation(ATTENUATION, ("pointLights[" + number + "].attenuation").c_str());*/
			sprintf(buf, "pointLights[%d].position", newNumberOfPointLights);
			currentLight->SetUniformLocation(POSITION_LIGHT, buf);
			sprintf(buf, "pointLights[%d].specular", newNumberOfPointLights);
			currentLight->SetUniformLocation(SPECULAR_COLOR, buf);
			sprintf(buf, "pointLights[%d].diffuse", newNumberOfPointLights);
			currentLight->SetUniformLocation(DIFFUSE_COLOR, buf);
			sprintf(buf, "pointLights[%d].ambiant", newNumberOfPointLights);
			currentLight->SetUniformLocation(AMBIANT_COLOR, buf);
			sprintf(buf, "pointLights[%d].attenuation", newNumberOfPointLights);
			currentLight->SetUniformLocation(ATTENUATION, buf);
			newNumberOfPointLights++;
			break;


		case SPOT_LIGHT:
			/*sprintf(printbuffer, "%d", newNumberOfSpotLights);
			number = printbuffer;
			currentLight->SetUniformLocation(POSITION_LIGHT, ("spotLights[" + number + "].position").c_str());
			currentLight->SetUniformLocation(SPECULAR_COLOR, ("spotLights[" + number + "].specular").c_str());
			currentLight->SetUniformLocation(DIFFUSE_COLOR, ("spotLights[" + number + "].diffuse").c_str());
			currentLight->SetUniformLocation(AMBIANT_COLOR, ("spotLights[" + number + "].ambiant").c_str());
			currentLight->SetUniformLocation(ATTENUATION, ("spotLights[" + number + "].attenuation").c_str());
			currentLight->SetUniformLocation(SPOT_CUT_OFF, ("spotLights[" + number + "].cutOff").c_str());
			currentLight->SetUniformLocation(SPOT_EXPONENT, ("spotLights[" + number + "].spotExponent").c_str());
			currentLight->SetUniformLocation(SPOT_DIRECTION, ("spotLights[" + number + "].direction").c_str());*/
			sprintf(buf, "spotLights[%d].position", newNumberOfSpotLights);
			currentLight->SetUniformLocation(POSITION_LIGHT, buf);
			sprintf(buf, "spotLights[%d].specular", newNumberOfSpotLights);
			currentLight->SetUniformLocation(SPECULAR_COLOR, buf);
			sprintf(buf, "spotLights[%d].diffuse", newNumberOfSpotLights);
			currentLight->SetUniformLocation(DIFFUSE_COLOR, buf);
			sprintf(buf, "spotLights[%d].ambiant", newNumberOfSpotLights);
			currentLight->SetUniformLocation(AMBIANT_COLOR, buf);
			sprintf(buf, "spotLights[%d].attenuation", newNumberOfSpotLights);
			currentLight->SetUniformLocation(ATTENUATION, buf);
			sprintf(buf, "spotLights[%d].cutOff", newNumberOfSpotLights);
			currentLight->SetUniformLocation(SPOT_CUT_OFF, buf);
			sprintf(buf, "spotLights[%d].spotExponent", newNumberOfSpotLights);
			currentLight->SetUniformLocation(SPOT_EXPONENT, buf);
			sprintf(buf, "spotLights[%d].direction", newNumberOfSpotLights);
			currentLight->SetUniformLocation(SPOT_DIRECTION, buf);
			newNumberOfSpotLights++;
			break;
		}
	}


	LightCount count;
	count.mDir = newNumberOfDirectLights;
	count.mSpot = newNumberOfSpotLights;
	count.mPoint = newNumberOfPointLights;
	return count;
}


void RendererOpenGL::SendLightsInfo(TravState* travstate)
{
	if (travstate->mLights == nullptr)
		return;

	Camera*	cam = travstate->GetCurrentCamera();
	v3f lCamPos;

	if (cam)
	{
		const mat4& lMatCam = cam->GetLocalToGlobal();
		v3f outCam(0.0f, 0.0f, 0.0f);
		lMatCam.TransformPoint(&outCam, &lCamPos);
	}

	auto itr = travstate->mLights->begin();
	auto end = travstate->mLights->end();
	for (; itr != end; ++itr)
	{
		API3DLight* currentLight = (API3DLight*)*itr;
		currentLight->PreRendering(this, cam, lCamPos);
		currentLight->DrawLight(travstate);
	}
}

void RendererOpenGL::ClearLightsInfo(TravState* travstate)
{
	if (travstate->mLights == nullptr)
		return;

	auto itr = travstate->mLights->begin();
	auto end = travstate->mLights->end();
	for (; itr != end; ++itr)
	{
		API3DLight* currentLight = (API3DLight*)*itr;
		currentLight->PostDrawLight(travstate);
	}
}

void RendererOpenGL::InitTextureInfo()
{
	if (MAX_TEXTURE_UNIT == -1)
	{
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &MAX_TEXTURE_UNIT); CHECK_GLERROR;
		if (MAX_TEXTURE_UNIT == -1) MAX_TEXTURE_UNIT = 8;
		mTextureUnit.resize(MAX_TEXTURE_UNIT);

		/*TextureUnitInfo tui;
		tui.mBindedTextureID = -1;
		tui.mBindedTextureType = -1;
		memset(mTextureUnit, 0x00, sizeof(TextureUnitInfo)*MAX_TEXTURE_UNIT);*/

		/*for (int i = 0; i < MAX_TEXTURE_UNIT; i++)
		{
			memcpy(&mTextureUnit[i], &tui, sizeof(TextureUnitInfo));
		}*/
	}
}

void RendererOpenGL::EnableTexture()
{
}

void RendererOpenGL::DisableTexture()
{
}

void RendererOpenGL::ActiveTextureChannel(unsigned int channel)
{
	if (mCurrentTextureUnit != channel)
	{
		glActiveTexture(GL_TEXTURE0 + channel); CHECK_GLERROR;
		mCurrentTextureUnit = channel;

		if (GetActiveShader())
		{
			if (GetActiveShader()->isGeneric())
			{
				const Locations* locs = GetActiveShader()->GetLocation();
				glUniform1i(locs->textureLocation[channel], channel); CHECK_GLERROR;
			}
		}
	}
}

void RendererOpenGL::SetUniform1i(unsigned int loc, s32 value)
{
	glUniform1i(loc, value); CHECK_GLERROR;
}


void RendererOpenGL::CheckError(const char* filename, int line)
{
	int err = glGetError();
	if (err != GL_NO_ERROR)
		kigsprintf("glGetError() raised in %s(%d): %d!!\n", filename, line, err);
}

void RendererOpenGL::BindTexture(RendererTextureType type, unsigned int ID)
{
	InitTextureInfo();
	KIGS_ASSERT(((int)mCurrentTextureUnit) < MAX_TEXTURE_UNIT);

	// retreive the texture unit info
	TextureUnitInfo * tu = &mTextureUnit[mCurrentTextureUnit];

	// check the type
	int glType;
	switch (type)
	{
	case RENDERER_TEXTURE_2D:
		glType = GL_TEXTURE_2D;
		break;
#if defined (WIN32) && !defined(WUP)
	case RENDERER_TEXTURE_1D:
		glType = GL_TEXTURE_1D;
		break;
	case RENDERER_TEXTURE_3D:
		glType = GL_TEXTURE_3D;
		break;
	case RENDERER_TEXTURE_CUBE_MAP:
		glType = GL_TEXTURE_CUBE_MAP;
		break;
#endif
	default:
		break;
	}

	if (tu->mBindedTextureID == ID && tu->mBindedTextureType == glType)
		return;

	if (tu->mBindedTextureID != ID)
	{
		glBindTexture(glType, ID); CHECK_GLERROR;


		tu->mBindedTextureID = ID;
		tu->mBindedTextureType = glType;
	}
}

void RendererOpenGL::UnbindTexture(RendererTextureType type, unsigned int ID)
{
	TextureUnitInfo * tu = &mTextureUnit[mCurrentTextureUnit];

	// check the type
	int glType;
	switch (type)
	{
	case RENDERER_TEXTURE_2D:
		glType = GL_TEXTURE_2D;
		break;
#if defined (WIN32) && !defined(WUP)
	case RENDERER_TEXTURE_1D:
		glType = GL_TEXTURE_1D;
		break;
	case RENDERER_TEXTURE_3D:
		glType = GL_TEXTURE_3D;
		break;
	case RENDERER_TEXTURE_CUBE_MAP:
		glType = GL_TEXTURE_CUBE_MAP;
		break;
#endif
	default: // should never be here
		return;
	}

	glActiveTexture(GL_TEXTURE0 + mCurrentTextureUnit); CHECK_GLERROR;

	glBindTexture(glType, 0);
	tu->mBindedTextureID = 0;
	tu->mBindedTextureType = glType;
}

void	RendererOpenGL::TextureParameteri(RendererTextureType type, RendererTexParameter1 name, RendererTexParameter2 param) {

	int texType;
	int param1;
	int param2;
#if defined (WIN32) && !defined(WUP)
	if (type == RENDERER_TEXTURE_1D)texType = GL_TEXTURE_1D;
	else if (type == RENDERER_TEXTURE_2D)texType = GL_TEXTURE_2D;
	else if (type == RENDERER_TEXTURE_CUBE_MAP)texType = GL_TEXTURE_CUBE_MAP;
	else texType = GL_TEXTURE_3D;
#else
	texType = GL_TEXTURE_2D;
#endif

	if (name == RENDERER_TEXTURE_MIN_FILTER)param1 = GL_TEXTURE_MIN_FILTER;
	else if (name == RENDERER_TEXTURE_MAG_FILTER)param1 = GL_TEXTURE_MAG_FILTER;
	else if (name == RENDERER_TEXTURE_WRAP_S)param1 = GL_TEXTURE_WRAP_S;
	else if (name == RENDERER_TEXTURE_WRAP_T)param1 = GL_TEXTURE_WRAP_T;
#ifdef WIN32
	else param1 = GL_TEXTURE_WRAP_R;
#else
	else param1 = GL_TEXTURE_MIN_FILTER;
#endif

	if (param == RENDERER_NEAREST)param2 = GL_NEAREST;
	else if (param == RENDERER_CLAMP_TO_EDGE)param2 = GL_CLAMP_TO_EDGE;
	else if (param == RENDERER_LINEAR)param2 = GL_LINEAR;
	else if (param == RENDERER_REPEAT)param2 = GL_REPEAT;
	else param2 = GL_LINEAR_MIPMAP_LINEAR;

	glTexParameteri(texType, param1, param2);  CHECK_GLERROR;
}


void RendererOpenGL::BindArrayBuffer(unsigned int id)
{
	glBindBuffer(GL_ARRAY_BUFFER, id);
}

void RendererOpenGL::BindElementBuffer(unsigned int id)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void RendererOpenGL::SetVertexAttribDivisor(TravState* state, unsigned int bufferName, int attribute_location, int divisor)
{
	mVertexBufferManager->SetArrayBuffer(bufferName);
	mVertexBufferManager->FlushBindBuffer(KIGS_BUFFER_TARGET_ARRAY);
#if defined(WIN32) || defined(WUP)
#ifdef WUP
	glVertexAttribDivisorANGLE(attribute_location, divisor * (state->GetHolographicMode() ? 2 : 1));
#else
	glVertexAttribDivisor(attribute_location, divisor);
#endif
#endif
}

//#define DEBUG_DRAW_INSTANCES
#ifdef DEBUG_DRAW_INSTANCES
#include "imgui.h"
#include "KigsTools.h"
#endif

void RendererOpenGL::DrawPendingInstances(TravState* state)
{
	state->mDrawingInstances = true;
#ifdef DEBUG_DRAW_INSTANCES
	static int o = -1;
	static int t = -1;
	if (gKigsToolsAvailable)
	{
		if (!state->mPath)
			ImGui::SliderInt("o", &o, -1, state->mInstancing.size() - 1);
		else
			ImGui::SliderInt("t", &t, -1, state->mInstancing.size() - 1);
	}
#endif
	int k = 0;

	ShaderBase* active_shader = GetActiveShader();
	bool first = true;

	

	std::vector<std::pair<ModernMeshItemGroup* const, InstancingData>*> sorted_instances; sorted_instances.reserve(state->mInstancing.size());
	for (auto& instance : state->mInstancing)
	{
		sorted_instances.push_back(&instance);
	}

	std::sort(sorted_instances.begin(), sorted_instances.end(), [](const auto& a, const auto& b)
	{
		return std::make_tuple(INT_MAX - a->second.priority, a->second.shader, a->second.shader_variant) < std::make_tuple(INT_MAX - b->second.priority, b->second.shader, b->second.shader_variant);
	});

	for (auto& instance_ptr : sorted_instances)
	{
		auto& instance = *instance_ptr;
		++k;
#ifdef DEBUG_DRAW_INSTANCES
		if (!state->mPath && o != -1 && o != k - 1) continue;
		if (state->mPath && t != -1 && t != k - 1) continue;
#endif
		auto mesh = instance.first;
		state->mInstanceCount = (u32)instance.second.transforms.size();
		if (active_shader != instance.second.shader)
		{
			if(!first)
				popShader(active_shader, state);

			first = false;
			pushShader(instance.second.shader, state);
			active_shader = instance.second.shader;
		}
		mesh->DoPreDraw(state);

		state->mInstanceBufferIndex = getVBO();
		BufferData(state->mInstanceBufferIndex, KIGS_BUFFER_TARGET_ARRAY,(int)( sizeof(float) * 12 * instance.second.transforms.size()), instance.second.transforms.data(), KIGS_BUFFER_USAGE_STREAM);

		auto shader = RendererOpenGL::GetActiveShader();
		const Locations * locs = shader->GetLocation();

		size_t vec4size = sizeof(v4f);
		
		SetArrayBuffer(state->mInstanceBufferIndex);
		mVertexBufferManager->FlushBindBuffer();

		glEnableVertexAttribArray(locs->attribInstanceMatrix[0]);
		glVertexAttribPointer(locs->attribInstanceMatrix[0], 4, GL_FLOAT, false, (int)(3 * vec4size), (void*)0);
		glEnableVertexAttribArray(locs->attribInstanceMatrix[1]);
		glVertexAttribPointer(locs->attribInstanceMatrix[1], 4, GL_FLOAT, false, (int)(3 * vec4size), (void*)(vec4size));
		glEnableVertexAttribArray(locs->attribInstanceMatrix[2]);
		glVertexAttribPointer(locs->attribInstanceMatrix[2], 4, GL_FLOAT, false, (int)(3 * vec4size), (void*)(2 * vec4size));

		SetVertexAttribDivisor(state, state->mInstanceBufferIndex, locs->attribInstanceMatrix[0], 1);
		SetVertexAttribDivisor(state, state->mInstanceBufferIndex, locs->attribInstanceMatrix[1], 1);
		SetVertexAttribDivisor(state, state->mInstanceBufferIndex, locs->attribInstanceMatrix[2], 1);

		mesh->DoDraw(state);

		SetVertexAttribDivisor(state, state->mInstanceBufferIndex, locs->attribInstanceMatrix[0], 0);
		SetVertexAttribDivisor(state, state->mInstanceBufferIndex, locs->attribInstanceMatrix[1], 0);
		SetVertexAttribDivisor(state, state->mInstanceBufferIndex, locs->attribInstanceMatrix[2], 0);

		glDisableVertexAttribArray(locs->attribInstanceMatrix[0]);
		glDisableVertexAttribArray(locs->attribInstanceMatrix[1]);
		glDisableVertexAttribArray(locs->attribInstanceMatrix[2]);

		mesh->DoPostDraw(state);

		state->mInstanceCount = 0;
	}

	if (!first)
	{
		popShader(active_shader, state);
	}

	state->mDrawingInstances = false;
	state->mInstancing.clear();
}
// ### VertexBufferManager Section
VertexBufferManager::VertexBufferManager()
{
	mCurrentArrayBound = -1;
	mCurrentElemBound = -1;
	mAskedArrayBound = -1;
	mAskedElemBound = -1;

	mEnableVertexAttrib.clear();
	mEnableVertexAttrib.reserve(32);
}

void VertexBufferManager::GenBuffer(int count, unsigned int * id)
{
	glGenBuffers(count, id); CHECK_GLERROR;
}

void VertexBufferManager::DelBuffer(int count, unsigned int* id)
{
	for (int i = 0; i < count; i++)
	{
		for (auto& VA : mEnableVertexAttrib)
		{
			if (VA.second.mBufferName == id[i])
			{
				glDisableVertexAttribArray(VA.first);
				VA.second.mUsed = -1;
				VA.second.mBufferName = -1;
			}
		}
	}
	
	glDeleteBuffers(count, id); CHECK_GLERROR;
}

void VertexBufferManager::DelBufferLater(int count, unsigned int * id)
{
	for (int i = 0; i < count; i++)
		mToDeleteBuffer.push_back(id[i]);
}

void VertexBufferManager::DoDelayedAction()
{
	if (!mToDeleteBuffer.empty())
	{
		DelBuffer((int)mToDeleteBuffer.size(), mToDeleteBuffer.data());
		mToDeleteBuffer.clear();
	}
}

void VertexBufferManager::internalBindBuffer(unsigned int bufferName, unsigned int bufftype)
{
	if (bufftype == KIGS_BUFFER_TARGET_ARRAY)
	{
		SetArrayBuffer(bufferName);
		FlushBindBuffer(KIGS_BUFFER_TARGET_ARRAY);
	}
	else if (bufftype == KIGS_BUFFER_TARGET_ELEMENT)
	{
		SetElementBuffer(bufferName);
		FlushBindBuffer(KIGS_BUFFER_TARGET_ELEMENT);
	}
}

void VertexBufferManager::FlushBindBuffer(int target, bool force)
{
	if (force)
	{
		mCurrentArrayBound = -1;
		mCurrentElemBound = -1;
	}

	if (target == KIGS_BUFFER_TARGET_ARRAY || target == 0)
		if ((mAskedArrayBound != mCurrentArrayBound) && (mAskedArrayBound != -1))
		{
			glBindBuffer(KIGS_BUFFER_TARGET_ARRAY, mAskedArrayBound); CHECK_GLERROR;
			mCurrentArrayBound = mAskedArrayBound;
		}

	if (target == KIGS_BUFFER_TARGET_ELEMENT || target == 0)
		if ((mAskedElemBound != mCurrentElemBound) && (mAskedElemBound != -1))
		{
			glBindBuffer(KIGS_BUFFER_TARGET_ELEMENT, mAskedElemBound); CHECK_GLERROR;
			mCurrentElemBound = mAskedElemBound;
		}

	

	mAskedArrayBound = -1;
	mAskedElemBound = -1;
}

void VertexBufferManager::FlushUnusedVertexAttrib()
{ 
	// disable unused attribs
	for (auto& VA : mEnableVertexAttrib)
	{
		if (VA.second.mUsed == 0)
		{
			glDisableVertexAttribArray(VA.first);
			VA.second.mUsed = -1;
		}
	}
}

void VertexBufferManager::UnbindBuffer(unsigned int bufferName, int target)
{
	if (target == KIGS_BUFFER_TARGET_ARRAY || target == 0)
		if (mCurrentArrayBound == bufferName)
			mCurrentArrayBound = -1;

	if (target == KIGS_BUFFER_TARGET_ELEMENT || target == 0)
		if (mCurrentElemBound == bufferName)
			mCurrentElemBound = -1;
}
// called at the end of drawElements or DrawArray to mark already used vertex array
void VertexBufferManager::MarkVertexAttrib()
{ 
	for (auto& VA : mEnableVertexAttrib)
	{
		if(VA.second.mUsed>0)
			VA.second.mUsed = 0;
	}
	
	// mAskedArrayBound = 0;
	// mAskedElemBound = 0;
}

void VertexBufferManager::Clear(bool push)
{
	if (push)
	{
		for (auto& VA : mEnableVertexAttrib)
		{
			glDisableVertexAttribArray(VA.first); CHECK_GLERROR;
		}
	}

	mAskedArrayBound = 0;
	mAskedElemBound = 0;
	mEnableVertexAttrib.clear();
}

void VertexBufferManager::SetElementBuffer(unsigned int bufferName)
{
	mAskedElemBound = bufferName;
}

void VertexBufferManager::SetArrayBuffer(unsigned int bufferName, int slot)
{
	(void)slot;
	mAskedArrayBound = bufferName;
}

void VertexBufferManager::SetVertexAttrib(unsigned int bufferName, unsigned int attribID, int size, unsigned int type, bool normalized, unsigned int stride, void * offset,const Locations* locs)
{
	if (locs == nullptr) return;
	if (locs->attribs[attribID] == 0xFFFFFFFF) return;

	internalBindBuffer(bufferName, KIGS_BUFFER_TARGET_ARRAY);

	glEnableVertexAttribArray(locs->attribs[attribID]); CHECK_GLERROR;
	glVertexAttribPointer(locs->attribs[attribID], size, type, normalized, stride, offset); CHECK_GLERROR;

	VAStruct& str = mEnableVertexAttrib[locs->attribs[attribID]];
	str.mBufferName= bufferName;
	str.mUsed = 1;
}

void VertexBufferManager::BufferData(unsigned int bufferName, unsigned int bufferType, int size, void* data, unsigned int usage)
{
	internalBindBuffer(bufferName, bufferType);
	glBufferData(bufferType, size, data, usage); CHECK_GLERROR;
}

void RendererOpenGL::DrawUIQuad(TravState * state, const UIVerticesInfo * qi)
{
	const Locations * locs = RendererOpenGL::GetActiveShader()->GetLocation();
	
	unsigned int bufferName = getUIVBO();

	BufferData(bufferName, KIGS_BUFFER_TARGET_ARRAY, qi->Offset * qi->vertexCount, qi->Buffer(), KIGS_BUFFER_USAGE_STATIC);

	SetVertexAttrib(bufferName, KIGS_VERTEX_ATTRIB_VERTEX_ID, qi->vertexComp, KIGS_FLOAT, false, qi->Offset, (void*)(uintptr_t)qi->vertexStride, locs);
	SetVertexAttrib(bufferName, KIGS_VERTEX_ATTRIB_COLOR_ID, qi->colorComp, KIGS_UNSIGNED_BYTE, false, qi->Offset, (void*)(uintptr_t)qi->colorStride, locs);
	SetVertexAttrib(bufferName, KIGS_VERTEX_ATTRIB_TEXCOORD_ID, qi->texComp, KIGS_FLOAT, false, qi->Offset, (void*)(uintptr_t)qi->texStride, locs);

	DrawArrays(state, GL_TRIANGLE_STRIP, 0, qi->vertexCount);

	SetArrayBuffer(0);
}

void RendererOpenGL::DrawUITriangles(TravState * state, const UIVerticesInfo * qi)
{
	const Locations * locs = RendererOpenGL::GetActiveShader()->GetLocation();
	
	unsigned int bufferName = getUIVBO();
	BufferData(bufferName, KIGS_BUFFER_TARGET_ARRAY, qi->Offset * qi->vertexCount, qi->Buffer(), KIGS_BUFFER_USAGE_STATIC);
	SetVertexAttrib(bufferName, KIGS_VERTEX_ATTRIB_VERTEX_ID, qi->vertexComp, KIGS_FLOAT, false, qi->Offset, (void*)(uintptr_t)qi->vertexStride, locs);
	SetVertexAttrib(bufferName, KIGS_VERTEX_ATTRIB_COLOR_ID, qi->colorComp, KIGS_UNSIGNED_BYTE, false, qi->Offset, (void*)(uintptr_t)qi->colorStride, locs);
	SetVertexAttrib(bufferName, KIGS_VERTEX_ATTRIB_TEXCOORD_ID, qi->texComp, KIGS_FLOAT, false, qi->Offset, (void*)(uintptr_t)qi->texStride, locs);

	DrawArrays(state,GL_TRIANGLES, 0, qi->vertexCount);

	SetArrayBuffer(0);
}

