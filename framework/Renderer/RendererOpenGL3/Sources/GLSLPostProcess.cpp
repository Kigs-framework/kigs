#include "PrecompiledHeaders.h"

#include "GLSLPostProcess.h"
#include "FilePathManager.h"
#include "ModuleFileManager.h"
#include "Core.h"
#include "RendererOpenGL.h"
#include "CoreRawBuffer.h"
//#include "OpenGLLight.h"
#include "Camera.h"
#include "TravState.h"
#include "CoreBaseApplication.h"
#include "Timer.h"
// include after all other kigs include (for iOS)
#include "Platform/Renderer/OpenGLInclude.h"


IMPLEMENT_CLASS_INFO(API3DPostProcess)

API3DPostProcess::API3DPostProcess(const kstl::string& name,CLASS_NAME_TREE_ARG) : Drawable(name,PASS_CLASS_NAME_TREE_ARG)
,myVertexShaderText(*this,false,LABEL_AND_ID(VertexShader),"")
,myFragmentShaderText(*this,false,LABEL_AND_ID(FragmentShader),"")
{
	myVertexShader=myFragmentShader=myShaderProgram.mID=0xFFFFFFFF;
	myFboRenderer = myFboTexture = myDepthTexture =0xFFFFFFFF;
}

void API3DPostProcess::NotifyUpdate(const unsigned int labelid )
{
	if( (labelid==myVertexShaderText.getLabelID()) ||(labelid==myFragmentShaderText.getLabelID()))
	{
		Dealloc();
		// rebuild only if both shaders are set
		if((((kstl::string)myVertexShaderText)!="") && (((kstl::string)myFragmentShaderText)!=""))
		{
			Rebuild();
		}
	}

	Drawable::NotifyUpdate(labelid);
}

API3DPostProcess::~API3DPostProcess()
{
	Dealloc();
}

void	API3DPostProcess::Dealloc()
{
	if(myVertexShader != 0xFFFFFFFF)
	{
		glDeleteShader(myVertexShader);
		myVertexShader=0xFFFFFFFF;
	}
	if(myFragmentShader != 0xFFFFFFFF)
	{
		glDeleteShader(myFragmentShader);
		myFragmentShader=0xFFFFFFFF;
	}
	if(myShaderProgram.mID != 0xFFFFFFFF)
	{
		glDeleteProgram(myShaderProgram.mID);
		myShaderProgram.mID =0xFFFFFFFF;
	}
	if(myDepthTexture != 0xFFFFFFFF)
	{
		glDeleteRenderbuffers(1, &myDepthTexture);
		myDepthTexture=0xFFFFFFFF;
	}
	if(myFboTexture != 0xFFFFFFFF)
	{
		ModuleRenderer::theGlobalRenderer->DeleteTexture(1, &myFboTexture);
		myFboTexture=0xFFFFFFFF;
	}
	if(myFboRenderer != 0xFFFFFFFF)
	{
		glDeleteFramebuffers(1, &myFboRenderer);
		myFboRenderer=0xFFFFFFFF;
	}

}

void	API3DPostProcess::Rebuild()
{
	myVertexShader=glCreateShader(GL_VERTEX_SHADER);
	myFragmentShader=glCreateShader(GL_FRAGMENT_SHADER);

	const char* source=0;
	CoreRawBuffer* rawbuffer=0;
	FilePathManager*	pathManager=(FilePathManager*)KigsCore::GetSingleton(_S_2_ID("FilePathManager"));

	if((myVertexShaderText.const_ref())[0]=='!') // load from file
	{
		const char* filename=(myVertexShaderText.c_str()+1);

		kstl::string fullfilename=filename;
		SmartPointer<FileHandle> fullfilenamehandle;

		if (pathManager)
		{
			fullfilenamehandle = pathManager->FindFullName(filename);
			if (fullfilenamehandle)
			{
				fullfilename = fullfilenamehandle->myFullFileName;
			}
		}
		u64 length;
		rawbuffer=ModuleFileManager::LoadFileAsCharString(fullfilename.c_str(),length);
		if(rawbuffer)
		{
			source=(const char*)rawbuffer->buffer();
		}
		else
		{
			Dealloc();
			return;
		}
	}
	else
	{
		source=myVertexShaderText.c_str();
	}
	// compile vertex shader
	glShaderSource(myVertexShader,1,&source,0);

	glCompileShader(myVertexShader);
	rawbuffer->Destroy();
	GLint isCompiled = 0;
	glGetShaderiv(myVertexShader, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE)
	{
#ifdef _DEBUG
		GLint maxLength = 0;
		glGetShaderiv(myVertexShader, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		std::vector<char> errorLog(maxLength);
		glGetShaderInfoLog(myVertexShader, maxLength, &maxLength, &errorLog[0]);
#endif
		//Provide the infolog in whatever manor you deem best.
		//Exit with failure.
		Dealloc();
		return;
	}


	if((myFragmentShaderText.const_ref())[0]=='!') // load from file
	{
		const char* filename=(myFragmentShaderText.c_str()+1);

		kstl::string fullfilename=filename;
		SmartPointer<FileHandle> fullfilenamehandle;

		if (pathManager)
		{
			fullfilenamehandle = pathManager->FindFullName(filename);
			if (fullfilenamehandle)
			{
				fullfilename = fullfilenamehandle->myFullFileName;
			}
		}

		u64 length;
		rawbuffer=ModuleFileManager::LoadFileAsCharString(fullfilename.c_str(),length);
		if(rawbuffer)
		{
			source=(const char*)rawbuffer->buffer();
		}
		else
		{
			Dealloc();
			return;
		}
	}
	else
	{
		source=myFragmentShaderText.c_str();
	}

	// compile fragment shader
	glShaderSource(myFragmentShader,1,&source,0);
	glCompileShader(myFragmentShader);
	rawbuffer->Destroy();
	isCompiled = 0;
	glGetShaderiv(myFragmentShader, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE)
	{
#ifdef _DEBUG
		GLint maxLength = 0;
		glGetShaderiv(myFragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		std::vector<char> errorLog(maxLength);
		glGetShaderInfoLog(myFragmentShader, maxLength, &maxLength, &errorLog[0]);
#endif
		//Provide the infolog in whatever manor you deem best.
		//Exit with failure.
		Dealloc();
		return;
	}

	myShaderProgram.mID = glCreateProgram();

	//Attach our shaders to our program
	glAttachShader(myShaderProgram.mID, myVertexShader);
	glAttachShader(myShaderProgram.mID, myFragmentShader);

	//Link our program
	glLinkProgram(myShaderProgram.mID);

	//Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(myShaderProgram.mID, GL_LINK_STATUS, (int *)&isLinked);
	if(isLinked == GL_FALSE)
	{
#ifdef _DEBUG
		GLint maxLength = 0;
		glGetProgramiv(myShaderProgram.mID, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(myShaderProgram.mID, maxLength, &maxLength, &infoLog[0]);
#endif
		//We don't need the program anymore.
		Dealloc();
		return;
	}

	//Always detach shaders after a successful link.
	glDetachShader(myShaderProgram.mID, myVertexShader);
	glDetachShader(myShaderProgram.mID, myFragmentShader);

}

void	API3DPostProcess::InitModifiable()
{
	RendererOpenGL* renderer = static_cast<RendererOpenGL*>(ModuleRenderer::theGlobalRenderer); // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	if(!IsInit())
	{
		if((((kstl::string)myVertexShaderText)!="") && (((kstl::string)myFragmentShaderText)!=""))
		{
			Rebuild();
			myVertexShaderText.changeNotificationLevel(Owner);
			myFragmentShaderText.changeNotificationLevel(Owner);

			// create offscreen buffer
			renderer->FlushState();
			renderer->CreateTexture(1, &myFboTexture);
			//glBindTexture(GL_TEXTURE_2D, myFboTexture);
			renderer->FlushState();
			renderer->ActiveTextureChannel(0);
			renderer->BindTexture(RENDERER_TEXTURE_2D, myFboTexture);
			/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);*/
			renderer->TextureParameteri(RENDERER_TEXTURE_2D,RENDERER_TEXTURE_MAG_FILTER,RENDERER_NEAREST);
			renderer->TextureParameteri(RENDERER_TEXTURE_2D,RENDERER_TEXTURE_MIN_FILTER,RENDERER_NEAREST);
			renderer->TextureParameteri(RENDERER_TEXTURE_2D,RENDERER_TEXTURE_WRAP_S,RENDERER_CLAMP_TO_EDGE);
			renderer->TextureParameteri(RENDERER_TEXTURE_2D,RENDERER_TEXTURE_WRAP_T,RENDERER_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 768, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			//glBindTexture(GL_TEXTURE_2D, 0);
			renderer->FlushState();
			renderer->BindTexture(RENDERER_TEXTURE_2D, 0);
			/* Depth buffer */
			glGenRenderbuffers(1, &myDepthTexture);
			glBindRenderbuffer(GL_RENDERBUFFER, myDepthTexture);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 1024, 768);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);

			/* Framebuffer to link everything together */
			glGenFramebuffers(1, &myFboRenderer);
			glBindFramebuffer(GL_FRAMEBUFFER, myFboRenderer);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, myFboTexture, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, myDepthTexture);
			GLenum status;
			if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
			{
				KIGS_ERROR("glCheckFramebufferStatus: error",1);
				return;
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			Drawable::InitModifiable();
		}
	}
}

// setup offscreen rendering
bool	API3DPostProcess::PreDraw(TravState* travstate)
{
	if(Drawable::PreDraw(travstate))
	{
		glBindFramebuffer(GL_FRAMEBUFFER, myFboRenderer);

		// retreive current camera

		Camera*	cam=travstate->GetCurrentCamera();
		if(cam)
		{
			glClearColor(0.0, 0.0, 0.0,0.0);	// clear color with full transparency
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		}
		return true;
	}
	return false;
}

bool	API3DPostProcess::PostDraw(TravState* travstate)
{
	RendererOpenGL* renderer=(RendererOpenGL*)travstate->GetRenderer();
	if(Drawable::PostDraw(travstate))
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		Active(travstate);
		//glBindTexture(GL_TEXTURE_2D, myFboTexture);
		renderer->FlushState();
		renderer->ActiveTextureChannel(0);
		renderer->BindTexture(RENDERER_TEXTURE_2D, myFboTexture);

		CoreBaseApplication* app=KigsCore::Instance()-> GetCoreApplication();

		float currentTime=(float)app->GetApplicationTimer()->GetTime();

		unsigned int L_Location = glGetUniformLocation(myShaderProgram.mID,"time");
		glUniform1f(L_Location,currentTime);


		//glDisable(GL_DEPTH_TEST);
		renderer->SetDepthTestMode(false);
		//glEnable(GL_ALPHA_TEST);
		renderer->SetAlphaTestMode(RENDERER_ALPHA_TEST_ON);
		//glAlphaFunc( GL_ALWAYS,0.0f );
		renderer->SetAlphaMode(RENDERER_ALPHA_ALWAYS,0.0f);
		//glEnable(GL_BLEND);
		renderer->SetBlendMode(RENDERER_BLEND_ON);
		//glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		renderer->SetBlendFuncMode(RENDERER_BLEND_SRC_ALPHA,RENDERER_BLEND_ONE_MINUS_SRC_ALPHA);
		//glDisable(GL_LIGHTING);
		renderer->SetLightMode(RENDERER_LIGHT_OFF);
		renderer->FlushState();

#ifndef WUP
		// draw quad
		glBegin(GL_QUADS);

		// draw the quad
		glVertex3i(-1   , -1    ,0);
		glVertex3i( 1   , -1    ,0);
		glVertex3i( 1   ,  1    ,0);
		glVertex3i(-1   ,  1    ,0);

		glEnd();
#endif

		Deactive(travstate);
		//glEnable(GL_DEPTH_TEST);
		renderer->SetDepthTestMode(true);
		renderer->FlushState();
		return true;
	}
	return false;
}

void	API3DPostProcess::Active(TravState* t)
{
	if(myShaderProgram.mID != 0xFFFFFFFF)
	{
		t->GetRenderer()->setCurrentShaderProgram(&myShaderProgram);
	}
}

void	API3DPostProcess::Deactive(TravState* t)
{
	t->GetRenderer()->setCurrentShaderProgram(0);
}
