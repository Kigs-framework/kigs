#include "PrecompiledHeaders.h"


#include "GLSLDeferred.h"
#include "GLSLDeferredFilter.h"
#include "RendererOpenGL.h"
#include "RendererDefines.h"

#include "TextureFileManager.h"
#include "Texture.h"
#include "GLSLLight.h"
#include "RenderingScreen.h"
#include "Scene3D.h"
#include "TravState.h"
#include "OpenGLCamera.h"

// include after all other kigs include (for iOS)
#include "Platform/Renderer/OpenGLInclude.h"

IMPLEMENT_CLASS_INFO(API3DDeferred)

//# CONSTRUCTOR
API3DDeferred::API3DDeferred(const kstl::string& name, CLASS_NAME_TREE_ARG) : Node3D(name, PASS_CLASS_NAME_TREE_ARG)
{
	myFilters.clear();

	myFboRenderer = myFboTexture[0] = myFboTexture[1] = myFboTexture[2] = myFboTexture[3] = myFboTexture[4] = myDepthTexture = 0xFFFFFFFF;
	mySize[0] = 2; mySize[1] = 2;
	myDepthTexture = 0xFFFFFFFF;

	myFboLightTexture = 0xFFFFFFFF;
	myFboLight = 0xFFFFFFFF;

	myCoords[0] = -1; myCoords[1] = 1;
	myCoords[2] = -1; myCoords[3] = -1;
	myCoords[4] = 1; myCoords[5] = 1;
	myCoords[6] = 1; myCoords[7] = -1;
}

bool API3DDeferred::addItem(CoreModifiable *item, ItemPosition pos DECLARE_DEFAULT_LINK_NAME)
{
	if (item->isSubType(API3DDeferredFilter::myClassID))
	{
		myFilters.insert(static_cast<API3DDeferredFilter*>(item));
	}
	return Node3D::addItem(item, pos PASS_LINK_NAME(linkName));
}

bool API3DDeferred::removeItem(CoreModifiable* item DECLARE_DEFAULT_LINK_NAME)
{
	if (item->isSubType(API3DDeferredFilter::myClassID))
	{
		myFilters.erase(static_cast<API3DDeferredFilter*>(item));
	}
	return Node3D::removeItem(item PASS_LINK_NAME(linkName));
}

//# DESTRUCTOR
API3DDeferred::~API3DDeferred()
{
	myRenderShader->Destroy();
}

void	API3DDeferred::Dealloc()
{
	if (myDepthTexture != 0xFFFFFFFF)
	{
		glDeleteRenderbuffers(1, &myDepthTexture);
		myDepthTexture = 0xFFFFFFFF;
	}

	if (myFboTexture[0] != 0xFFFFFFFF)
	{
		ModuleRenderer::theGlobalRenderer->DeleteTexture(5, myFboTexture);
		myFboTexture[0] = myFboTexture[1] = myFboTexture[2] = myFboTexture[3] = myFboTexture[4] = 0xFFFFFFFF;
	}

	if (myFboRenderer != 0xFFFFFFFF)
	{
		glDeleteFramebuffers(1, &myFboRenderer);
		myFboRenderer = 0xFFFFFFFF;
	}
}

void API3DDeferred::CreateFramebuffer(RendererOpenGL* renderer)
{
	{
		/* Framebuffer to link everything together */
		glGenFramebuffers(1, &myFboRenderer);
		glBindFramebuffer(GL_FRAMEBUFFER, myFboRenderer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, myFboTexture[0], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, myFboTexture[1], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, myFboTexture[2], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, myFboTexture[3], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, myFboTexture[4], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, myDepthTexture, 0);

		unsigned int DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 , GL_COLOR_ATTACHMENT4 };
		glDrawBuffers(5, DrawBuffers);
		GLenum status;
		if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
		{
			KIGS_ERROR("glCheckFramebufferStatus: error", 1);
			return;
		}
	}

	{
		/* Framebuffer to link everything together */
		glGenFramebuffers(1, &myFboLight);
		glBindFramebuffer(GL_FRAMEBUFFER, myFboLight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, myFboLightTexture, 0);

		unsigned int DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, DrawBuffers);
		GLenum status;
		if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
		{
			KIGS_ERROR("glCheckFramebufferStatus: error", 1);
			return;
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void API3DDeferred::CreateBuffers(RendererOpenGL* renderer, int sizeX, int sizeY)
{
	// create light final texture
	renderer->ActiveTextureChannel(0);
	renderer->BindTexture(RENDERER_TEXTURE_2D, myFboLightTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sizeX, sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// create offscreen color buffer
	renderer->BindTexture(RENDERER_TEXTURE_2D, myFboTexture[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mySize[0], mySize[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// normal buffer
	renderer->BindTexture(RENDERER_TEXTURE_2D, myFboTexture[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mySize[0], mySize[1], 0, GL_RGBA, GL_HALF_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// position
	renderer->BindTexture(RENDERER_TEXTURE_2D, myFboTexture[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mySize[0], mySize[1], 0, GL_RGBA, GL_HALF_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// specular
	renderer->BindTexture(RENDERER_TEXTURE_2D, myFboTexture[3]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mySize[0], mySize[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// ambiant
	renderer->BindTexture(RENDERER_TEXTURE_2D, myFboTexture[4]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mySize[0], mySize[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Depth buffer //
	renderer->BindTexture(RENDERER_TEXTURE_2D, myDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mySize[0], mySize[1], 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void	API3DDeferred::InitModifiable()
{
	Node3D::InitModifiable();
	if (IsInit())
	{
		// create final shader
		kstl::string ShaderFrag = "// generated deffered RenderShader\n"
			"#version 150\n"
			"in vec2 TexCoords;\n"
			"uniform sampler2D s1_ColorMap;\n"
			"out vec4 data;\n"

			"void main()\n"
			"{\n"

			"data =  texture(s1_ColorMap, TexCoords.st); \n"

			"}\n";

		kstl::string ShaderVert = "// generated deffered RenderShader\n"
			"#version 150\n"
			"in vec3 position;\n"
			"in vec2 texCoords;\n"

			"out vec2 TexCoords;\n"

			"void main()\n"
			"{\n"
			"gl_Position = vec4(position, 1.0f);\n"
			"TexCoords = texCoords;\n"
			"}\n";

		myRenderShader = static_cast<API3DShader*>(KigsCore::GetInstanceOf("GeneratedGeometryShader", _S_2_ID("API3DShader")));
		myRenderShader->setValue(LABEL_TO_ID(FragmentShader), ShaderFrag);
		myRenderShader->setValue(LABEL_TO_ID(VertexShader), ShaderVert);
		myRenderShader->Init();

		RendererOpenGL* renderer = static_cast<RendererOpenGL*>(ModuleRenderer::theGlobalRenderer); //(RendererOpenGL*)static_cast<ModuleRenderer*>(Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
		renderer->CreateTexture(5, myFboTexture);
		renderer->CreateTexture(1, &myFboLightTexture);
		renderer->CreateTexture(1, &myDepthTexture);

		CreateBuffers(renderer, 2, 2);
		CreateFramebuffer(renderer);
	}
}

// setup offscreen rendering
void API3DDeferred::TravDraw(TravState* travstate)
{
	// retreive current camera
	OpenGLCamera*	cam = (OpenGLCamera*)travstate->GetCurrentCamera();
	if (cam == nullptr)
		return;

	// merge filter with camera filter
	auto list = cam->GetFilters();
	for (auto f : list)
	{
		myFilters.insert(f);
	}

	myFinalRenderTexture = myFboLightTexture;

	// compute geometry in first pass
	glBindFramebuffer(GL_FRAMEBUFFER, myFboRenderer);

	RendererOpenGL* renderer = (RendererOpenGL*)travstate->GetRenderer();
	renderer->pushShader(myRenderShader, 0);
	renderer->PushState();

	PreDrawTreatment(travstate);
	PreGeometryTreatment(travstate);


	CheckUniqueObject rs;
	cam->getValue(LABEL_TO_ID(RenderingScreen), rs);

	float x, y;
	static_cast<RenderingScreen*>((RefCountedClass*)rs)->GetSize(x, y);
	//float scrX = x;
	//float scrY = y;


	kfloat vpminx, vpminy, vpsx, vpsy;
	cam->GetViewport(vpminx, vpminy, vpsx, vpsy);

	// camera quality factor (1 = normal, 0.5 = big pixels)
	float quality = 1.0f;
	cam->getValue(LABEL_TO_ID(Quality), quality);

	// clamp quality
	if (quality > 2.0f)
		quality = 2.0f;
	if (quality < 0.2f)
		quality = 0.2f;

	x *= vpsx*quality;
	y *= vpsy*quality;

	if (x != mySize[0] || y != mySize[1])
	{
		mySize[0] = (int)x;
		mySize[1] = (int)y;

		CreateBuffers(renderer, mySize[0], mySize[1]);
	}

	renderer->Viewport(0, 0, mySize[0], mySize[1]);
	renderer->SetScissorValue(0, 0, mySize[0], mySize[1]);


	renderer->SetClearColorValue(0.0f, 0.0f, 0.0f, 0.0f);
	renderer->FlushState();
	renderer->ClearView(RENDERER_CLEAR_COLOR_AND_DEPTH);

	// calculate cam position
	Point3D lCamPos;
	const Matrix3x4& lMatCam = cam->GetLocalToGlobal();
	Point3D outCam(0.0f, 0.0f, 0.0f);
	lMatCam.TransformPoint(&outCam, &lCamPos);

	renderer->SetBlendMode(RENDERER_BLEND_OFF);
	Node3D::TravDraw(travstate);
	renderer->SetBlendMode(RENDERER_BLEND_ON);

	PostGeometryTreatment(travstate);

	// render lights in second pass
	renderer->PopState();
	glBindFramebuffer(GL_FRAMEBUFFER, myFboLight);

	static_cast<RenderingScreen*>((RefCountedClass*)rs)->GetSize(x, y);
	renderer->Viewport((u32)(vpminx*x), (u32)(vpminy*y), (u32)(vpsx*x), (u32)(vpsy*y));
	renderer->SetScissorValue((s32)(vpminx*x), (s32)(vpminy*y), (s32)(vpsx*x), (s32)(vpsy*y));

	renderer->SetClearColorValue(0.0f, 0.0f, 0.0f, 0.0f);
	renderer->FlushState();
	renderer->ClearView(RENDERER_CLEAR_COLOR_AND_DEPTH);

	PreLightTreatment(travstate);

	// sort light by priority
	const kstl::set<CoreModifiable*>& lights = travstate->GetScene()->getLights();
	kstl::set<API3DLight*, LightPriorityCompare> sortedLights;

	{
		kstl::set<CoreModifiable*>::const_iterator itr = lights.begin();
		kstl::set<CoreModifiable*>::const_iterator end = lights.end();
		for (; itr != end; ++itr)
		{
			if ((*itr)->IsInit())
				sortedLights.insert(static_cast<API3DLight*>(*itr));
		}
	}


	bool firstLight = false;
	renderer->ActiveTextureChannel(2);
	renderer->BindTexture(RENDERER_TEXTURE_2D, myFboTexture[0]);
	renderer->ActiveTextureChannel(3);
	renderer->BindTexture(RENDERER_TEXTURE_2D, myFboTexture[1]);
	renderer->ActiveTextureChannel(4);
	renderer->BindTexture(RENDERER_TEXTURE_2D, myFboTexture[2]);
	renderer->ActiveTextureChannel(5);
	renderer->BindTexture(RENDERER_TEXTURE_2D, myFboTexture[3]);
	renderer->ActiveTextureChannel(6);
	renderer->BindTexture(RENDERER_TEXTURE_2D, myFboTexture[4]);

	API3DLight * light;
	kstl::set<API3DLight*, LightPriorityCompare>::iterator itr = sortedLights.begin();
	kstl::set<API3DLight*, LightPriorityCompare>::iterator end = sortedLights.end();
	for (; itr != end; ++itr)
	{
		light = *itr;
		if (!light->PreRendering(renderer, cam, lCamPos))
			continue;

		renderer->pushShader(light, 0);

		if (firstLight) // not on first light
		{
			renderer->SetDepthTestMode(false);
			renderer->SetBlendFuncMode(RENDERER_BLEND_ONE, RENDERER_BLEND_ONE);	// render glsllight
			renderer->SetBlendMode(RENDERER_BLEND_ON);
		}

		firstLight = true;

		//Bind texture
		glUniform1i(glGetUniformLocation(light->GetCurrentShaderProgram()->mID, "s1_ColorMap"), 2);
		glUniform1i(glGetUniformLocation(light->GetCurrentShaderProgram()->mID, "s2_NormalMap"), 3);
		glUniform1i(glGetUniformLocation(light->GetCurrentShaderProgram()->mID, "s3_PositionMap"), 4);
		glUniform1i(glGetUniformLocation(light->GetCurrentShaderProgram()->mID, "s4_SpecularMap"), 5);
		glUniform1i(glGetUniformLocation(light->GetCurrentShaderProgram()->mID, "s5_AmbiantMap"), 6);


		light->DrawLight(travstate);

		renderer->FlushState();

		RenderQuad();

		light->PostDrawLight(travstate);

		renderer->popShader(light, 0);
	}

	renderer->ActiveTextureChannel(2);
	renderer->BindTexture(RENDERER_TEXTURE_2D, NULL);
	renderer->ActiveTextureChannel(3);
	renderer->BindTexture(RENDERER_TEXTURE_2D, NULL);
	renderer->ActiveTextureChannel(4);
	renderer->BindTexture(RENDERER_TEXTURE_2D, NULL);
	renderer->ActiveTextureChannel(5);
	renderer->BindTexture(RENDERER_TEXTURE_2D, NULL);
	renderer->ActiveTextureChannel(6);
	renderer->BindTexture(RENDERER_TEXTURE_2D, NULL);

	PostLightTreatment(travstate);

	PostDrawTreatment(travstate);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	renderer->ActiveTextureChannel(0);
	renderer->BindTexture(RENDERER_TEXTURE_2D, myFinalRenderTexture);

	renderer->SetBlendMode(RENDERER_BLEND_OFF);

	renderer->FlushState();
	RenderQuad();
	renderer->popShader(myRenderShader, 0);

	// remove merged filter 
	for (auto f : list)
	{
		myFilters.erase(f);
	}
}



unsigned int quadVAO = 0;
unsigned int quadVBO;
void API3DDeferred::RenderQuad()
{
	if (quadVAO == 0)
	{
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

/// Filters
void API3DDeferred::PreDrawTreatment(TravState* state)
{
	for (API3DDeferredFilter* f : myFilters)
	{
		if (f->NeedPass(API3DDeferredFilter::PreDrawPass))
			f->PreDrawTreatment(state, this);
	}
}

void API3DDeferred::PreGeometryTreatment(TravState* state)
{
	for (API3DDeferredFilter* f : myFilters)
	{
		if (f->NeedPass(API3DDeferredFilter::PreGeoPass))
			f->PreGeometryTreatment(state, this);
	}
}

void API3DDeferred::PostGeometryTreatment(TravState* state)
{
	for (API3DDeferredFilter* f : myFilters)
	{
		if (f->NeedPass(API3DDeferredFilter::PostGeoPass))
			f->PostGeometryTreatment(state, this);
	}
}

void API3DDeferred::PreLightTreatment(TravState* state)
{
	for (API3DDeferredFilter* f : myFilters)
	{
		if (f->NeedPass(API3DDeferredFilter::PreLightPass))
			f->PreLightTreatment(state, this);
	}
}

void API3DDeferred::PostLightTreatment(TravState* state)
{
	for (API3DDeferredFilter* f : myFilters)
	{
		if (f->NeedPass(API3DDeferredFilter::PostLightPass))
			f->PostLightTreatment(state, this);
	}
}

void API3DDeferred::PostDrawTreatment(TravState* state)
{
	for (API3DDeferredFilter* f : myFilters)
	{
		if (f->NeedPass(API3DDeferredFilter::PostDrawPass))
		{
			unsigned int out = f->PostDrawTreatment(state, this);
			if (out != -1)
				myFinalRenderTexture = out;
		}
	}
}