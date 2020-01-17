

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
#ifndef JAVASCRIPT
#include "FreeType_TextDrawer.h"
#endif

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



// ## Static object initialization
ModuleSpecificRenderer *	RendererOpenGL::theGlobalRenderer = NULL;
FreeType_TextDrawer*		RendererOpenGL::myDrawer = NULL;

/*API3DShader*					RendererOpenGL::myCurrentShader = NULL;
unsigned int				RendererOpenGL::myCurrentShaderProgram=0;
kstl::vector<API3DShader*>	RendererOpenGL::myShaderStack;
unsigned int				RendererOpenGL::myDirtyShaderMatrix = 0;*/


#define mCurrentArrayBound mCurrentBoundBuffer[0]
#define mCurrentElemBound mCurrentBoundBuffer[1]
#define mAskedArrayBound mAskedBoundBuffer[0]
#define mAskedElemBound mAskedBoundBuffer[1]


#ifdef WUP
extern bool gIsHolographic;
#endif

IMPLEMENT_CLASS_INFO(RendererOpenGL)

RendererOpenGL::RendererOpenGL(const kstl::string& name, CLASS_NAME_TREE_ARG) : ModuleSpecificRenderer(name, PASS_CLASS_NAME_TREE_ARG)

, myCurrentOGLMatrixMode(-1)

{
	myShaderStack.clear();
	myShaderStack.reserve(16);
	myCurrentShader = NULL;
	myCurrentShaderProgram = 0;

	myVBO[0] = -1; // for init check
}

void	RendererOpenGL::ProtectedFlushMatrix(TravState* state)
{
	if (HasShader()) // load uniform
	{
		auto locations = GetActiveShader()->GetLocation();
		if ((myDirtyMatrix & 1) || myDirtyShaderMatrix)
		{
			
			if (locations->modelMatrix != -1)
			{
				glUniformMatrix4fv(locations->modelMatrix, 1, false, &(myMatrixStack[0].back().e[0][0])); CHECK_GLERROR;
			}
		}

		auto cam = state->GetCurrentCamera();
		if (myDirtyShaderMatrix && locations->fogScale != -1 && cam)
		{
			//auto D = myMatrixStack[1].back()[14];
			//auto C = myMatrixStack[1].back()[10];
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

				auto viewproj = myMatrixStack[MATRIX_MODE_PROJECTION].back()*myMatrixStack[MATRIX_MODE_VIEW].back();
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
			if ((myDirtyMatrix & 2) || myDirtyShaderMatrix)
			{
				if (locations->projMatrix != -1)
				{
					glUniformMatrix4fv(locations->projMatrix, 1, false, &(myMatrixStack[1].back().e[0][0])); CHECK_GLERROR;
				}
			}

			if ((myDirtyMatrix & 4) || myDirtyShaderMatrix)
			{
				if (locations->viewMatrix != -1)
				{
					glUniformMatrix4fv(locations->viewMatrix, 1, false, &(myMatrixStack[2].back().e[0][0])); CHECK_GLERROR;
				}
			}
		}

		myDirtyShaderMatrix = 0;
	}
}

void OpenGLRenderingState::ProtectedInitHardwareState()
{
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_SCISSOR_TEST);

	//glClearColor(myGlobalRedClearValueFlag, myGlobalGreenClearValueFlag, myGlobalBlueClearValueFlag, myGlobalAlphaClearValueFlag);
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
	if (force || myGlobalCullFlag != otherOne->myGlobalCullFlag)
	{
		otherOne->myGlobalCullFlag = myGlobalCullFlag;
		int cullmode = myGlobalCullFlag;

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

	if (force || myGlobalBlendFlag != otherOne->myGlobalBlendFlag) {
		otherOne->myGlobalBlendFlag = myGlobalBlendFlag;
		bool blendmode = (bool)myGlobalBlendFlag;

		if (!blendmode) {
			glDisable(GL_BLEND); CHECK_GLERROR;
		}
		else {
			glEnable(GL_BLEND); CHECK_GLERROR;
		}
	}

	if (force || myGlobalBlendValue1Flag != otherOne->myGlobalBlendValue1Flag || myGlobalBlendValue2Flag != otherOne->myGlobalBlendValue2Flag) {
		otherOne->myGlobalBlendValue1Flag = myGlobalBlendValue1Flag;
		otherOne->myGlobalBlendValue2Flag = myGlobalBlendValue2Flag;
		int blendvalue1 = (int)myGlobalBlendValue1Flag;
		int blendvalue2 = (int)myGlobalBlendValue2Flag;
		GLenum sfactor;
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
			glBlendFunc(sfactor, GL_ZERO); CHECK_GLERROR;
		}
		else if (blendvalue2 == 1) {
			glBlendFunc(sfactor, GL_ONE); CHECK_GLERROR;
		}
		else if (blendvalue2 == 2) {
			glBlendFunc(sfactor, GL_SRC_COLOR); CHECK_GLERROR;
		}
		else if (blendvalue2 == 3) {
			glBlendFunc(sfactor, GL_ONE_MINUS_SRC_COLOR); CHECK_GLERROR;
		}
		else if (blendvalue2 == 4) {
			glBlendFunc(sfactor, GL_DST_COLOR); CHECK_GLERROR;
		}
		else if (blendvalue2 == 5) {
			glBlendFunc(sfactor, GL_ONE_MINUS_DST_COLOR); CHECK_GLERROR;
		}
		else if (blendvalue2 == 6) {
			glBlendFunc(sfactor, GL_SRC_ALPHA); CHECK_GLERROR;
		}
		else if (blendvalue2 == 7) {
			glBlendFunc(sfactor, GL_ONE_MINUS_SRC_ALPHA); CHECK_GLERROR;
		}
		else if (blendvalue2 == 8) {
			glBlendFunc(sfactor, GL_DST_ALPHA); CHECK_GLERROR;
		}
		else if (blendvalue2 == 9) {
			glBlendFunc(sfactor, GL_ONE_MINUS_DST_ALPHA); CHECK_GLERROR;
		}
#ifdef WIN32
		else if (blendvalue2 == 10) {
			glBlendFunc(sfactor, GL_CONSTANT_COLOR); CHECK_GLERROR;
		}
		else if (blendvalue2 == 11) {
			glBlendFunc(sfactor, GL_ONE_MINUS_CONSTANT_COLOR); CHECK_GLERROR;
		}
		else if (blendvalue2 == 12) {
			glBlendFunc(sfactor, GL_CONSTANT_ALPHA); CHECK_GLERROR;
		}
		else {
			glBlendFunc(sfactor, GL_ONE_MINUS_CONSTANT_ALPHA); CHECK_GLERROR;
		}
#endif
	}

	if (force || myGlobalDepthMaskFlag != otherOne->myGlobalDepthMaskFlag) {
		otherOne->myGlobalDepthMaskFlag = myGlobalDepthMaskFlag;
		bool depthmaskmode = (bool)myGlobalDepthMaskFlag;

		if (!depthmaskmode) {
			glDepthMask(GL_FALSE); CHECK_GLERROR;
		}
		else {
			glDepthMask(GL_TRUE); CHECK_GLERROR;
		}
	}

	if (force || myGlobalDepthTestFlag != otherOne->myGlobalDepthTestFlag) {
		otherOne->myGlobalDepthTestFlag = myGlobalDepthTestFlag;
		bool depthTestmode = (bool)myGlobalDepthTestFlag;

		if (!depthTestmode) {
			glDisable(GL_DEPTH_TEST); CHECK_GLERROR;
		}
		else {
			glEnable(GL_DEPTH_TEST); CHECK_GLERROR;
		}
	}

	if (force || myGlobalDepthValueFlag != otherOne->myGlobalDepthValueFlag) {
		otherOne->myGlobalDepthValueFlag = myGlobalDepthValueFlag;
#if defined (WIN32) && !defined(WUP)
		float depthvalue = (float)myGlobalDepthValueFlag;
		glClearDepth(depthvalue); CHECK_GLERROR;
#endif
	}

	if (force || myGlobalScissorTestFlag != otherOne->myGlobalScissorTestFlag) {
		otherOne->myGlobalScissorTestFlag = myGlobalScissorTestFlag;
		bool scissorTestmode = (bool)myGlobalScissorTestFlag;

		if (!scissorTestmode) {
			glDisable(GL_SCISSOR_TEST); CHECK_GLERROR;
		}
		else {
			glEnable(GL_SCISSOR_TEST); CHECK_GLERROR;
		}
	}

	if (force || myGlobalScissorXFlag != otherOne->myGlobalScissorXFlag
		|| myGlobalScissorYFlag != otherOne->myGlobalScissorYFlag
		|| myGlobalScissorWidthFlag != otherOne->myGlobalScissorWidthFlag
		|| myGlobalScissorHeightFlag != otherOne->myGlobalScissorHeightFlag)
	{
		otherOne->myGlobalScissorXFlag = myGlobalScissorXFlag;
		otherOne->myGlobalScissorYFlag = myGlobalScissorYFlag;
		otherOne->myGlobalScissorWidthFlag = myGlobalScissorWidthFlag;
		otherOne->myGlobalScissorHeightFlag = myGlobalScissorHeightFlag;

		int x = myGlobalScissorXFlag;
		int y = myGlobalScissorYFlag;
		int width = myGlobalScissorWidthFlag;
		int height = myGlobalScissorHeightFlag;

		glScissor(x, y, width, height); CHECK_GLERROR;
	}

	if (force || myGlobalColorMaterialFaceFlag != otherOne->myGlobalColorMaterialFaceFlag || myGlobalColorMaterialParamFlag != otherOne->myGlobalColorMaterialParamFlag) {

		int face = myGlobalColorMaterialFaceFlag;
		int param = myGlobalColorMaterialParamFlag;
		otherOne->myGlobalColorMaterialFaceFlag = face;
		otherOne->myGlobalColorMaterialParamFlag = param;
	}

	

	for (int i = 0; i < 2; ++i)
	{
		auto face = i == 0 ? GL_FRONT : GL_BACK;
		if (force || myGlobalStencilMode[i] != otherOne->myGlobalStencilMode[i] || myGlobalStencilFuncMask[i] != otherOne->myGlobalStencilFuncMask[i] || myGlobalStencilFuncRef[i] != otherOne->myGlobalStencilFuncRef[i])
		{
			otherOne->myGlobalStencilMode[i] = myGlobalStencilMode[i];
			otherOne->myGlobalStencilFuncMask[i] = myGlobalStencilFuncMask[i];
			otherOne->myGlobalStencilFuncRef[i] = myGlobalStencilFuncRef[i];

			int mode = GL_ALWAYS;

			if (myGlobalStencilMode[i] == RENDERER_STENCIL_NEVER)
				mode = GL_NEVER;
			else if (myGlobalStencilMode[i] == RENDERER_STENCIL_LESS)
				mode = GL_LESS;
			else if (myGlobalStencilMode[i] == RENDERER_STENCIL_EQUAL)
				mode = GL_EQUAL;
			else if (myGlobalStencilMode[i] == RENDERER_STENCIL_LEQUAL)
				mode = GL_LEQUAL;
			else if (myGlobalStencilMode[i] == RENDERER_STENCIL_GREATER)
				mode = GL_GREATER;
			else if (myGlobalStencilMode[i] == RENDERER_STENCIL_NOTEQUAL)
				mode = GL_NOTEQUAL;
			else if (myGlobalStencilMode[i] == RENDERER_STENCIL_GEQUAL)
				mode = GL_GEQUAL;
			else if (myGlobalStencilMode[i] == RENDERER_STENCIL_ALWAYS)
				mode = GL_ALWAYS;

			glStencilFuncSeparate(face, mode, myGlobalStencilFuncRef[i], myGlobalStencilFuncMask[i]);
		}

		if (force || myGlobalStencilMask[i] != otherOne->myGlobalStencilMask[i])
		{
			otherOne->myGlobalStencilMask[i] = myGlobalStencilMask[i];
			glStencilMaskSeparate(face, myGlobalStencilMask[i]);
		}
		
		if (force || myGlobalStencilOpDPFail[i] != otherOne->myGlobalStencilOpDPFail[i]
			|| myGlobalStencilOpSFail[i] != otherOne->myGlobalStencilOpSFail[i]
			|| myGlobalStencilOpPass[i] != otherOne->myGlobalStencilOpPass[i])
		{
			otherOne->myGlobalStencilOpDPFail[i] = myGlobalStencilOpDPFail[i];
			otherOne->myGlobalStencilOpSFail[i] = myGlobalStencilOpSFail[i];
			otherOne->myGlobalStencilOpPass[i] = myGlobalStencilOpPass[i];

			glStencilOpSeparate(face, ConvertStencilOp(myGlobalStencilOpSFail[i]), ConvertStencilOp(myGlobalStencilOpDPFail[i]), ConvertStencilOp(myGlobalStencilOpPass[i]));
		}
	}
	



	if (force || myGlobalColorMask[0] != otherOne->myGlobalColorMask[0]
		|| myGlobalColorMask[1] != otherOne->myGlobalColorMask[1]
		|| myGlobalColorMask[2] != otherOne->myGlobalColorMask[2]
		|| myGlobalColorMask[3] != otherOne->myGlobalColorMask[3])
	{
		otherOne->myGlobalColorMask[0] = myGlobalColorMask[0];
		otherOne->myGlobalColorMask[1] = myGlobalColorMask[1];
		otherOne->myGlobalColorMask[2] = myGlobalColorMask[2];
		otherOne->myGlobalColorMask[3] = myGlobalColorMask[3];

		glColorMask(myGlobalColorMask[0], myGlobalColorMask[1], myGlobalColorMask[2], myGlobalColorMask[3]);
	}

	if (force || myGlobalStencilEnabled != otherOne->myGlobalStencilEnabled)
	{
		otherOne->myGlobalStencilEnabled = myGlobalStencilEnabled;
		if (myGlobalStencilEnabled)
			glEnable(GL_STENCIL_TEST);
		else
			glDisable(GL_STENCIL_TEST);
	}
#if !defined(WUP) && !defined(_M_ARM) && !defined(GL_ES2)
	if (force || myPolygonMode != otherOne->myPolygonMode)
	{
		otherOne->myPolygonMode = myPolygonMode;
		switch (myPolygonMode)
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
		glClearColor(myGlobalRedClearValueFlag, myGlobalGreenClearValueFlag, myGlobalBlueClearValueFlag, myGlobalAlphaClearValueFlag); CHECK_GLERROR;
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

void RendererOpenGL::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	BaseInit(core, "RendererOpenGL", params);
	DECLARE_FULL_CLASS_INFO(core, OpenGLRenderingMatrix, RendererMatrix, Renderer)
	DECLARE_FULL_CLASS_INFO(core, OpenGLRenderingScreen, RenderingScreen, Renderer)

	DECLARE_FULL_CLASS_INFO(core, DebugDraw, DebugDraw, Renderer);
	//DECLARE_FULL_CLASS_INFO(core, OpenGLTexture, Texture, Renderer)
	DECLARE_CLASS_INFO_WITHOUT_FACTORY(OpenGLTexture, "Texture");
	RegisterClassToInstanceFactory( core, "Renderer", "Texture",
		[](const kstl::string& instancename, kstl::vector<CoreModifiableAttribute*>* args) -> CoreModifiable*
	{
		if (args && args->size())
		{
			return OpenGLTexture::CreateInstance(instancename, args);
		}
		auto& texfileManager = KigsCore::Singleton<TextureFileManager>();
		SP<Texture> texture = texfileManager->GetTexture(instancename, false);
		// texture will be delete when lambda exit ( as only the pointer is returned )
		if (texture)
		{
			texture->GetRef(); // so get a ref before exiting
		}
		return texture.get(); // and return the pointer
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
	


#ifndef JAVASCRIPT
	// create the freetype drawer
	if (!myDrawer)
	{
		myDrawer = new FreeType_TextDrawer();
		myDrawer->startBuildFonts();
	}
#endif

	myVertexBufferManager = std::make_unique<VertexBufferManager>();

	if (!theGlobalRenderer)
		theGlobalRenderer = this;


	MAX_TEXTURE_UNIT = -1;

	ModuleSpecificRenderer::Init(core, params);

	myDefaultUIShader = KigsCore::GetInstanceOf("UIShader", "API3DUIShader");

	PlatformInit(core, params);
}

void RendererOpenGL::Close()
{
#ifndef JAVASCRIPT
	if (myDrawer)
	{
		delete myDrawer;
		myDrawer = NULL;
	}
#endif

	myDefaultUIShader = nullptr;

#ifdef WIN32
#ifndef WUP
	wglMakeCurrent(NULL, NULL);
#endif
#endif

	if(myVBO[0]!=-1)
		glDeleteBuffers(PREALLOCATED_VBO_COUNT, myVBO); CHECK_GLERROR;

	PlatformClose();
	ModuleSpecificRenderer::Close();
	BaseClose();
	theGlobalRenderer = nullptr;
}

void RendererOpenGL::Update(const Timer& timer, void* addParam)
{
	PlatformUpdate(timer, addParam);
	ModuleSpecificRenderer::Update(timer, addParam);
}

ModuleBase*	PlatformRendererModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	KigsCore::ModuleStaticInit(core);

	DECLARE_CLASS_INFO_WITHOUT_FACTORY(RendererOpenGL, "RendererOpenGL");
	ModuleBase*    gInstanceRendererOpenGL = new RendererOpenGL("RendererOpenGL");
	gInstanceRendererOpenGL->Init(core, params);

	return gInstanceRendererOpenGL;
}
/*
bool	RendererOpenGL::HasShader()
{
	return myCurrentShader != nullptr;
}

API3DShader*		RendererOpenGL::GetActiveShader()
{
	return myCurrentShader;
}

void	RendererOpenGL::pushShader(API3DShader* shad, TravState* state)
{
	bool needActive = false;
	if (myShaderStack.size())
	{
		if( (myCurrentShader != shad) || (myCurrentShaderProgram != shad->Get_ShaderProgram())) // need activation only if different shader
		{
			myCurrentShader->Deactive(state);
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
		if (state&&state->myCurrentMaterial)
		{
			Material*	toPostDraw = state->myCurrentMaterial;
			state->myCurrentMaterial = 0;
			toPostDraw->CheckPostDraw(state);

		}
	}

	myCurrentShader = shad;
	myShaderStack.push_back(shad); // must be pushed before activation
	//printf("PUSH Shader %p %s(%d)\n", shad, shad->getName().c_str(), shad->Get_ShaderProgram());

	if (needActive)
	{
		myDirtyShaderMatrix = 1;
		shad->Active(state, true);
	}

	if (StateMaterial)
		StateMaterial->CheckPreDraw(state);
}

void	RendererOpenGL::popShader(API3DShader* shad, TravState* state)
{
	// just make sure shad is the shader on top of the stack
	bool isShaderOK = false;
	if (myShaderStack.size())
	{
		if (myCurrentShader == shad)
		{
			isShaderOK = true;
		}

		myShaderStack.pop_back();

		API3DShader* shaderBack = 0;
		if (myShaderStack.size())
		{
			shaderBack = myShaderStack.back();
		}
		//printf("POP  Shader %p %s(%d)\n", shad, shad->getName().c_str(), shad->Get_ShaderProgram());


		Material*	StateMaterial = 0;
		if ((shaderBack != shad) || (myCurrentShaderProgram != shaderBack->Get_ShaderProgram()))
		{
			if (state&&state->myCurrentMaterial)
			{
				Material*	toPostDraw = state->myCurrentMaterial;
				state->myCurrentMaterial = 0;
				toPostDraw->CheckPostDraw(state);

			}

			shad->Deactive(state);

			myCurrentShader = shaderBack;

			if (myCurrentShader)
			{
				myDirtyShaderMatrix = 1;
				myCurrentShader->Active(state, true);
				
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
	glUseProgram(myCurrentShaderProgram->mID); CHECK_GLERROR;
}

void RendererOpenGL::FlushState(bool force)
{
	if (force)
	{
		myCurrentTextureUnit = 0;

		TextureUnitInfo tui;
		tui.BindedTextureID = -1;
		tui.BindedTextureType = -1;

		for (int i = 0; i < MAX_TEXTURE_UNIT; i++)
		{
			memcpy(&myTextureUnit[i], &tui, sizeof(TextureUnitInfo));
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
	/*TextureUnitInfo * tui = myTextureUnit;
	for (int i = 0; i < count; i++)
	{
		for (int j = 0; j < MAX_TEXTURE_UNIT; j++)
		{
			if (tui[j].BindedTextureID == id[i])
				tui[j].BindedTextureID = -1;
		}
	}*/

	for (int i = 0; i < count; i++)
	{
		for (auto &tui : myTextureUnit)
		{
			if (tui.BindedTextureID == id[i])
				tui.BindedTextureID = -1;
		}
	}


	glDeleteTextures(count, id); CHECK_GLERROR;
}

void RendererOpenGL::SetLightsInfo(kstl::set<CoreModifiable*>*lights)
{
	int newNumberOfDirectLights = 0;
	int newNumberOfPointLights = 0;
	int newNumberOfSpotLights = 0;

	char buf[128];

	auto itr = lights->begin();
	auto end = lights->end();
	for (; itr != end; ++itr)
	{
		API3DLight* myLight = static_cast<API3DLight*>(*itr);
		if (myLight->getIsDeffered())
			continue;
		if (!myLight->getIsOn())
			continue;


		switch (myLight->GetTypeOfLight())
		{
		case DIRECTIONAL_LIGHT:
			/*sprintf(printbuffer, "%d", newNumberOfDirectLights);
			number = printbuffer;
			myLight->SetUniformLocation(SPECULAR_COLOR, ("dirLights[" + number + "].specular").c_str());
			myLight->SetUniformLocation(DIFFUSE_COLOR, ("dirLights[" + number + "].diffuse").c_str());
			myLight->SetUniformLocation(AMBIANT_COLOR, ("dirLights[" + number + "].ambiant").c_str());
			myLight->SetUniformLocation(POSITION_LIGHT, ("dirLights[" + number + "].position").c_str());*/
			sprintf(buf, "dirLights[%d].specular", newNumberOfDirectLights);
			myLight->SetUniformLocation(SPECULAR_COLOR, buf);
			sprintf(buf, "dirLights[%d].diffuse", newNumberOfDirectLights);
			myLight->SetUniformLocation(DIFFUSE_COLOR, buf);
			sprintf(buf, "dirLights[%d].ambiant", newNumberOfDirectLights);
			myLight->SetUniformLocation(AMBIANT_COLOR, buf);
			sprintf(buf, "dirLights[%d].position", newNumberOfDirectLights);
			myLight->SetUniformLocation(POSITION_LIGHT, buf);
			newNumberOfDirectLights++;


			break;
		case POINT_LIGHT:
			/*sprintf(printbuffer, "%d", newNumberOfPointLights);
			number = printbuffer;
			myLight->SetUniformLocation(POSITION_LIGHT, ("pointLights[" + number + "].position").c_str());
			myLight->SetUniformLocation(SPECULAR_COLOR, ("pointLights[" + number + "].specular").c_str());
			myLight->SetUniformLocation(DIFFUSE_COLOR, ("pointLights[" + number + "].diffuse").c_str());
			myLight->SetUniformLocation(AMBIANT_COLOR, ("pointLights[" + number + "].ambiant").c_str());
			myLight->SetUniformLocation(ATTENUATION, ("pointLights[" + number + "].attenuation").c_str());*/
			sprintf(buf, "pointLights[%d].position", newNumberOfPointLights);
			myLight->SetUniformLocation(POSITION_LIGHT, buf);
			sprintf(buf, "pointLights[%d].specular", newNumberOfPointLights);
			myLight->SetUniformLocation(SPECULAR_COLOR, buf);
			sprintf(buf, "pointLights[%d].diffuse", newNumberOfPointLights);
			myLight->SetUniformLocation(DIFFUSE_COLOR, buf);
			sprintf(buf, "pointLights[%d].ambiant", newNumberOfPointLights);
			myLight->SetUniformLocation(AMBIANT_COLOR, buf);
			sprintf(buf, "pointLights[%d].attenuation", newNumberOfPointLights);
			myLight->SetUniformLocation(ATTENUATION, buf);
			newNumberOfPointLights++;
			break;


		case SPOT_LIGHT:
			/*sprintf(printbuffer, "%d", newNumberOfSpotLights);
			number = printbuffer;
			myLight->SetUniformLocation(POSITION_LIGHT, ("spotLights[" + number + "].position").c_str());
			myLight->SetUniformLocation(SPECULAR_COLOR, ("spotLights[" + number + "].specular").c_str());
			myLight->SetUniformLocation(DIFFUSE_COLOR, ("spotLights[" + number + "].diffuse").c_str());
			myLight->SetUniformLocation(AMBIANT_COLOR, ("spotLights[" + number + "].ambiant").c_str());
			myLight->SetUniformLocation(ATTENUATION, ("spotLights[" + number + "].attenuation").c_str());
			myLight->SetUniformLocation(SPOT_CUT_OFF, ("spotLights[" + number + "].cutOff").c_str());
			myLight->SetUniformLocation(SPOT_EXPONENT, ("spotLights[" + number + "].spotExponent").c_str());
			myLight->SetUniformLocation(SPOT_DIRECTION, ("spotLights[" + number + "].direction").c_str());*/
			sprintf(buf, "spotLights[%d].position", newNumberOfSpotLights);
			myLight->SetUniformLocation(POSITION_LIGHT, buf);
			sprintf(buf, "spotLights[%d].specular", newNumberOfSpotLights);
			myLight->SetUniformLocation(SPECULAR_COLOR, buf);
			sprintf(buf, "spotLights[%d].diffuse", newNumberOfSpotLights);
			myLight->SetUniformLocation(DIFFUSE_COLOR, buf);
			sprintf(buf, "spotLights[%d].ambiant", newNumberOfSpotLights);
			myLight->SetUniformLocation(AMBIANT_COLOR, buf);
			sprintf(buf, "spotLights[%d].attenuation", newNumberOfSpotLights);
			myLight->SetUniformLocation(ATTENUATION, buf);
			sprintf(buf, "spotLights[%d].cutOff", newNumberOfSpotLights);
			myLight->SetUniformLocation(SPOT_CUT_OFF, buf);
			sprintf(buf, "spotLights[%d].spotExponent", newNumberOfSpotLights);
			myLight->SetUniformLocation(SPOT_EXPONENT, buf);
			sprintf(buf, "spotLights[%d].direction", newNumberOfSpotLights);
			myLight->SetUniformLocation(SPOT_DIRECTION, buf);
			newNumberOfSpotLights++;
			break;
		}
	}
	if (newNumberOfDirectLights != myDirLightCount || newNumberOfPointLights != myPointLightCount || newNumberOfSpotLights != mySpotLightCount)
	{
		myDirLightCount = newNumberOfDirectLights;
		myPointLightCount = newNumberOfPointLights;
		mySpotLightCount = newNumberOfSpotLights;
	}
}


void RendererOpenGL::SendLightsInfo(TravState* travstate)
{
	if (travstate->myLights == nullptr)
		return;

	Camera*	cam = travstate->GetCurrentCamera();
	Point3D lCamPos;

	if (cam)
	{
		const Matrix3x4& lMatCam = cam->GetLocalToGlobal();
		Point3D outCam(0.0f, 0.0f, 0.0f);
		lMatCam.TransformPoint(&outCam, &lCamPos);
	}

	auto itr = travstate->myLights->begin();
	auto end = travstate->myLights->end();
	for (; itr != end; ++itr)
	{
		API3DLight* myLight = static_cast<API3DLight*>(*itr);
		myLight->PreRendering(this, cam, lCamPos);
		myLight->DrawLight(travstate);
	}
}

void RendererOpenGL::ClearLightsInfo(TravState* travstate)
{
	if (travstate->myLights == nullptr)
		return;

	auto itr = travstate->myLights->begin();
	auto end = travstate->myLights->end();
	for (; itr != end; ++itr)
	{
		API3DLight* myLight = static_cast<API3DLight*>(*itr);
		myLight->PostDrawLight(travstate);
	}
}

void RendererOpenGL::InitTextureInfo()
{
	if (MAX_TEXTURE_UNIT == -1)
	{
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &MAX_TEXTURE_UNIT); CHECK_GLERROR;
		if (MAX_TEXTURE_UNIT == -1) MAX_TEXTURE_UNIT = 8;
		myTextureUnit.resize(MAX_TEXTURE_UNIT);

		/*TextureUnitInfo tui;
		tui.BindedTextureID = -1;
		tui.BindedTextureType = -1;
		memset(myTextureUnit, 0x00, sizeof(TextureUnitInfo)*MAX_TEXTURE_UNIT);*/

		/*for (int i = 0; i < MAX_TEXTURE_UNIT; i++)
		{
			memcpy(&myTextureUnit[i], &tui, sizeof(TextureUnitInfo));
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
	if (myCurrentTextureUnit != channel)
	{
		glActiveTexture(GL_TEXTURE0 + channel); CHECK_GLERROR;
		myCurrentTextureUnit = channel;

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
	KIGS_ASSERT(((int)myCurrentTextureUnit) < MAX_TEXTURE_UNIT);

	// retreive the texture unit info
	TextureUnitInfo * tu = &myTextureUnit[myCurrentTextureUnit];

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

	if (tu->BindedTextureID == ID && tu->BindedTextureType == glType)
		return;

	if (tu->BindedTextureID != ID)
	{
		glBindTexture(glType, ID); CHECK_GLERROR;


		tu->BindedTextureID = ID;
		tu->BindedTextureType = glType;
	}
}

void RendererOpenGL::UnbindTexture(RendererTextureType type, unsigned int ID)
{
	TextureUnitInfo * tu = &myTextureUnit[myCurrentTextureUnit];

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

	glActiveTexture(GL_TEXTURE0 + myCurrentTextureUnit); CHECK_GLERROR;

	glBindTexture(glType, 0);
	tu->BindedTextureID = 0;
	tu->BindedTextureType = glType;
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
	myVertexBufferManager->SetArrayBuffer(bufferName);
	myVertexBufferManager->FlushBindBuffer(KIGS_BUFFER_TARGET_ARRAY);
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
		if (!state->myPath)
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
		if (!state->myPath && o != -1 && o != k - 1) continue;
		if (state->myPath && t != -1 && t != k - 1) continue;
#endif
		auto mesh = instance.first;
		state->mInstanceCount = instance.second.transforms.size();
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
		BufferData(state->mInstanceBufferIndex, KIGS_BUFFER_TARGET_ARRAY, sizeof(float) * 12 * instance.second.transforms.size(), instance.second.transforms.data(), KIGS_BUFFER_USAGE_STREAM);

		auto shader = RendererOpenGL::GetActiveShader();
		const Locations * locs = shader->GetLocation();

		size_t vec4size = sizeof(v4f);
		
		SetArrayBuffer(state->mInstanceBufferIndex);
		myVertexBufferManager->FlushBindBuffer();

		glEnableVertexAttribArray(locs->attribInstanceMatrix[0]);
		glVertexAttribPointer(locs->attribInstanceMatrix[0], 4, GL_FLOAT, false, 3 * vec4size, (void*)0);
		glEnableVertexAttribArray(locs->attribInstanceMatrix[1]);
		glVertexAttribPointer(locs->attribInstanceMatrix[1], 4, GL_FLOAT, false, 3 * vec4size, (void*)(vec4size));
		glEnableVertexAttribArray(locs->attribInstanceMatrix[2]);
		glVertexAttribPointer(locs->attribInstanceMatrix[2], 4, GL_FLOAT, false, 3 * vec4size, (void*)(2 * vec4size));

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

void VertexBufferManager::DelBuffer(int count, unsigned int * id)
{
	// TODO NONO should invalidate vertex attrib using these buffers
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
		DelBuffer(mToDeleteBuffer.size(), mToDeleteBuffer.data());
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

void VertexBufferManager::UnbindBuffer(unsigned int bufferName, int target)
{
	if (target == KIGS_BUFFER_TARGET_ARRAY || target == 0)
		if (mCurrentArrayBound == bufferName)
			mCurrentArrayBound = -1;

	if (target == KIGS_BUFFER_TARGET_ELEMENT || target == 0)
		if (mCurrentElemBound == bufferName)
			mCurrentElemBound = -1;
}


void VertexBufferManager::Clear(bool push)
{
	if (push)
	{
		kstl::vector<unsigned int>::iterator itr = mEnableVertexAttrib.begin();
		kstl::vector<unsigned int>::iterator end = mEnableVertexAttrib.end();
		for (; itr != end; ++itr)
			glDisableVertexAttribArray(*itr); CHECK_GLERROR;
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


	mEnableVertexAttrib.push_back(locs->attribs[attribID]);
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

	SetVertexAttrib(bufferName, KIGS_VERTEX_ATTRIB_VERTEX_ID, qi->vertexComp, KIGS_FLOAT, false, qi->Offset, (void*)qi->vertexStride, locs);
	SetVertexAttrib(bufferName, KIGS_VERTEX_ATTRIB_COLOR_ID, qi->colorComp, KIGS_UNSIGNED_BYTE, false, qi->Offset, (void*)qi->colorStride, locs);
	SetVertexAttrib(bufferName, KIGS_VERTEX_ATTRIB_TEXCOORD_ID, qi->texComp, KIGS_FLOAT, false, qi->Offset, (void*)qi->texStride, locs);

	DrawArrays(state, GL_TRIANGLE_STRIP, 0, qi->vertexCount);

	SetArrayBuffer(0);
}

void RendererOpenGL::DrawUITriangles(TravState * state, const UIVerticesInfo * qi)
{
	const Locations * locs = RendererOpenGL::GetActiveShader()->GetLocation();
	
	unsigned int bufferName = getUIVBO();
	BufferData(bufferName, KIGS_BUFFER_TARGET_ARRAY, qi->Offset * qi->vertexCount, qi->Buffer(), KIGS_BUFFER_USAGE_STATIC);
	SetVertexAttrib(bufferName, KIGS_VERTEX_ATTRIB_VERTEX_ID, qi->vertexComp, KIGS_FLOAT, false, qi->Offset, (void*)qi->vertexStride, locs);
	SetVertexAttrib(bufferName, KIGS_VERTEX_ATTRIB_COLOR_ID, qi->colorComp, KIGS_UNSIGNED_BYTE, false, qi->Offset, (void*)qi->colorStride, locs);
	SetVertexAttrib(bufferName, KIGS_VERTEX_ATTRIB_TEXCOORD_ID, qi->texComp, KIGS_FLOAT, false, qi->Offset, (void*)qi->texStride, locs);

	DrawArrays(state,GL_TRIANGLES, 0, qi->vertexCount);

	SetArrayBuffer(0);
}

