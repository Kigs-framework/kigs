#include "PrecompiledHeaders.h"
#include "GLSLUniform.h"
#include "Core.h"
#include "RendererOpenGL.h"
#include "OpenGLTexture.h"
#include "GLSLShader.h"
#include "TravState.h"
#include "simplexnoise.h"
#include "TextureFileManager.h"
#include "FilePathManager.h"
#include "ModuleSceneGraph.h"

//#define PRINF_GLCALL
#ifdef PRINF_GLCALL
#define GLPRINT(...) kigsprintf(__VA_ARGS__)
#else
#define GLPRINT(...)
#endif

// include after all other kigs include (for iOS)
#include "Platform/Renderer/OpenGLInclude.h"

IMPLEMENT_CLASS_INFO(API3DUniformBase)
IMPLEMENT_CLASS_INFO(API3DUniformInt)
IMPLEMENT_CLASS_INFO(API3DUniformFloat)
IMPLEMENT_CLASS_INFO(API3DUniformFloat2)
IMPLEMENT_CLASS_INFO(API3DUniformFloat3)
IMPLEMENT_CLASS_INFO(API3DUniformFloat4)
IMPLEMENT_CLASS_INFO(API3DUniformTexture)
IMPLEMENT_CLASS_INFO(API3DUniformDataTexture)
IMPLEMENT_CLASS_INFO(API3DUniformGeneratedTexture)
IMPLEMENT_CLASS_INFO(API3DUniformMatrixArray)

API3DUniformBase::API3DUniformBase(const kstl::string& name, CLASS_NAME_TREE_ARG) 
	: Drawable(name, PASS_CLASS_NAME_TREE_ARG)
, myUniName(*this, false, "Name", "")
,myID(-1)
{
	mRenderPassMask = 0xFFFFFFFF;
}

void	API3DUniformBase::InitModifiable()
{
	Drawable::InitModifiable();

	kstl::string strKey;
	myUniName.getValue(strKey);
	myID = CharToID::GetID(strKey);

	myUniName.changeNotificationLevel(Owner);
}

void	API3DUniformBase::NotifyUpdate(const unsigned int  labelid)
{
	if (labelid == myUniName.getLabelID())
	{
		kstl::string strKey;
		myUniName.getValue(strKey);
		myID = CharToID::GetID(strKey);
		//printf("%s >> %u\n", strKey.c_str(), myID);
	}
	else
	{
		Drawable::NotifyUpdate(labelid);
	}
}


bool API3DUniformBase::Push(TravState* t)
{
	if (t->GetRenderer()->HasShader())
	{
		t->GetRenderer()->GetActiveShader()->PushUniform(this);
		return true;
	}

	return false;
}

bool API3DUniformBase::Pop(TravState* t)
{
	if (t->GetRenderer()->HasShader())
	{
		t->GetRenderer()->GetActiveShader()->PopUniform(this);
		return true;
	}

	return false;
}

 bool	API3DUniformBase::PreDraw(TravState* t)
{
	 if(Drawable::PreDraw(t))
		return Push(t);
	 return false;
}

 bool	API3DUniformBase::PostDraw(TravState* t)
 {
	 if (Drawable::PostDraw(t))
		 return Pop(t);
	 return false;
 }


//////////////////////////////////// Int ///////////////////////////////////////////
API3DUniformInt::API3DUniformInt(const kstl::string& name, CLASS_NAME_TREE_ARG) : API3DUniformBase(name, PASS_CLASS_NAME_TREE_ARG)
, myValue(*this, false, "Value", -1)
{
	
}

void	API3DUniformInt::Activate(unsigned int a_Location)
{
	GLPRINT("%s %d>%d\n", getName().c_str(), a_Location, (int)myValue);
	glUniform1i(a_Location, myValue);
}

void	API3DUniformInt::NotifyUpdate(const unsigned int  labelid)
{
	
	
		API3DUniformBase::NotifyUpdate(labelid);
	
}


//////////////////////////////////// Float ///////////////////////////////////////////
API3DUniformFloat::API3DUniformFloat(const kstl::string& name, CLASS_NAME_TREE_ARG) : API3DUniformBase(name, PASS_CLASS_NAME_TREE_ARG)
, myValue(*this, false, "Value", -1)
{
	
}

void	API3DUniformFloat::Activate(unsigned int a_Location)
{
	GLPRINT("%s %d>%0.2f\n", getName().c_str(), a_Location, (float)myValue);
	glUniform1f(a_Location, myValue);
}

void	API3DUniformFloat::NotifyUpdate(const unsigned int  labelid)
{
	
		API3DUniformBase::NotifyUpdate(labelid);
	
}

//////////////////////////////////// Float 2 ///////////////////////////////////////////
API3DUniformFloat2::API3DUniformFloat2(const kstl::string& name, CLASS_NAME_TREE_ARG) : API3DUniformBase(name, PASS_CLASS_NAME_TREE_ARG)
, myValue(*this, false, "Value",0.0f,0.0f)
{
	
}

void	API3DUniformFloat2::Activate(unsigned int a_Location)
{
	GLPRINT("%s %d>(%0.2f:%0.2f)\n", getName().c_str(), a_Location, myValue[0], myValue[1]);
	glUniform2f(a_Location, myValue[0], myValue[1]);
}

void	API3DUniformFloat2::NotifyUpdate(const unsigned int  labelid)
{
	
	API3DUniformBase::NotifyUpdate(labelid);
	
}


//////////////////////////////////// Float 3 ///////////////////////////////////////////
API3DUniformFloat3::API3DUniformFloat3(const kstl::string& name, CLASS_NAME_TREE_ARG) : API3DUniformBase(name, PASS_CLASS_NAME_TREE_ARG)
, myValue(*this, false, "Value",0.0f,0.0f,0.0f)
, myNormalize(*this, false, "Normalize", false)
{

}

void API3DUniformFloat3::InitModifiable()
{
	API3DUniformBase::InitModifiable();

	if (myNormalize)
		Normalize();

	myNormalize.changeNotificationLevel(Owner);
}

void API3DUniformFloat3::NotifyUpdate(const unsigned int labelid)
{
	if (!IsInit())
		return;

	if (labelid == myNormalize.getLabelID())
	{
		if (myNormalize)
			Normalize();
	}
	else if (labelid == myValue.getLabelID())
	{
		if (myNormalize)
			Normalize();
	}
	else
		API3DUniformBase::NotifyUpdate(labelid);
}

void API3DUniformFloat3::Normalize()
{
	Vector3D v(myValue[0], myValue[1], myValue[2]);
	v.Normalize();

	myValue[0] = v.x;
	myValue[1] = v.y;
	myValue[2] = v.z;
}

void	API3DUniformFloat3::Activate(unsigned int a_Location)
{
	GLPRINT("%s %d>(%0.2f:%0.2f:%0.2f)\n", getName().c_str(), a_Location, myValue[0], myValue[1], myValue[2]);
	glUniform3f(a_Location, myValue[0], myValue[1], myValue[2]);
}

//////////////////////////////////// Float 4 ///////////////////////////////////////////
API3DUniformFloat4::API3DUniformFloat4(const kstl::string& name, CLASS_NAME_TREE_ARG) : API3DUniformBase(name, PASS_CLASS_NAME_TREE_ARG)
, myValue(*this, false, "Value",0.0f,0.0f,0.0f,0.0f)
{
	myValue.changeNotificationLevel(Owner);
}

void	API3DUniformFloat4::Activate(unsigned int a_Location)
{
	GLPRINT("%s %d>(%0.2f:%0.2f:%0.2f:%0.2f)\n", getName().c_str(), a_Location, myValue[0], myValue[1], myValue[2], myValue[3]);
	glUniform4f(a_Location, myValue[0], myValue[1], myValue[2], myValue[3]);
}

void	API3DUniformFloat4::NotifyUpdate(const unsigned int  labelid)
{
	
	API3DUniformBase::NotifyUpdate(labelid);
	
}

API3DUniformTexture::API3DUniformTexture(const kstl::string& name, CLASS_NAME_TREE_ARG) : API3DUniformBase(name, PASS_CLASS_NAME_TREE_ARG)
, myTextureChannel(*this, false, "Channel", 0)
, myTextureName(*this, false, "TextureName", "")
, myAttachedTexture(0)
{
	myTextureChannel.changeNotificationLevel(Owner);
}

void	API3DUniformTexture::InitModifiable()
{
	if (!IsInit())
	{
		if ((myTextureName.const_ref()) != "")
		{
			SP<TextureFileManager>	fileManager = KigsCore::GetSingleton("TextureFileManager");
			myAttachedTexture = fileManager->GetTexture(myTextureName.const_ref(), false);
			if (myAttachedTexture)
				if (!myAttachedTexture->IsInit())
				{
					myAttachedTexture->setValue("ForcePow2", true);
					myAttachedTexture->setValue("HasMipmap", true);
					myAttachedTexture->Init();
				}
		}
	}

	API3DUniformBase::InitModifiable();
}

void API3DUniformTexture::Activate(unsigned int a_Location)
{	
	if (myAttachedTexture)
	{
		GLPRINT("%s %d>%d\n", getName().c_str(), a_Location, (int)myTextureChannel);
		glUniform1i(a_Location, myTextureChannel);
		RendererOpenGL* renderer = static_cast<RendererOpenGL*>(ModuleRenderer::theGlobalRenderer); // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
		
		renderer->ActiveTextureChannel(myTextureChannel);
		renderer->BindTexture(RENDERER_TEXTURE_2D, static_cast<OpenGLTexture*>(myAttachedTexture.get())->GetGLID());
	}
}

void	API3DUniformTexture::NotifyUpdate(const unsigned int  labelid)
{
	
	API3DUniformBase::NotifyUpdate(labelid);
	
}


bool	API3DUniformTexture::Deactivate(unsigned int a_Location)
{
	RendererOpenGL* renderer = static_cast<RendererOpenGL*>(ModuleRenderer::theGlobalRenderer);  // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	renderer->ActiveTextureChannel(myTextureChannel);
	renderer->BindTexture(RENDERER_TEXTURE_2D, 0);
	return true;
}

bool API3DUniformTexture::addItem(CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	if (item->isSubType(Texture::myClassID)) // if texture, don't call father addItem
	{
		myAttachedTexture = item;
		return true;
	}

	return API3DUniformBase::addItem(item, pos PASS_LINK_NAME(linkName));

}

bool API3DUniformTexture::removeItem(CMSP& item DECLARE_LINK_NAME)
{
	if (item->isSubType(Texture::myClassID)) // if texture, don't call father removeItem
	{
		if (item == myAttachedTexture)
		{
			myAttachedTexture = 0;
			return true;
		}

		return true;
	}

	return SceneNode::removeItem(item  PASS_LINK_NAME(linkName));
}


API3DUniformTexture::~API3DUniformTexture()
{
	myAttachedTexture = nullptr;
}


API3DUniformDataTexture::API3DUniformDataTexture(const kstl::string& name, CLASS_NAME_TREE_ARG) : API3DUniformBase(name, PASS_CLASS_NAME_TREE_ARG)
, myTextureChannel(*this, false, "Channel", 0)
, myTextureName(*this, false, "TextureName", "")
, myTextureGLIndex(-1)
{
	myTextureChannel.changeNotificationLevel(Owner);
}

void	API3DUniformDataTexture::InitModifiable()
{
	if (!IsInit())
	{
		if ((myTextureName.const_ref()) != "")
		{

			SP<FilePathManager> pathManager = KigsCore::GetSingleton("FilePathManager");

			SmartPointer<FileHandle> fullfilenamehandle = pathManager->FindFullName(myTextureName.const_ref());
			if (fullfilenamehandle)
			{

				TinyImage* image = TinyImage::CreateImage(fullfilenamehandle.get());

				RendererOpenGL* renderer = static_cast<RendererOpenGL*>(ModuleRenderer::theGlobalRenderer);  // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();

				renderer->CreateTexture(1, &myTextureGLIndex);

				//glBindTexture(GL_TEXTURE_3D, myTextureGLIndex);
				renderer->ActiveTextureChannel(myTextureChannel);
				renderer->BindTexture(RENDERER_TEXTURE_2D, myTextureGLIndex);
				renderer->TextureParameteri(RENDERER_TEXTURE_2D, RENDERER_TEXTURE_MIN_FILTER, RENDERER_NEAREST);
				renderer->TextureParameteri(RENDERER_TEXTURE_2D, RENDERER_TEXTURE_MAG_FILTER, RENDERER_NEAREST);
				renderer->TextureParameteri(RENDERER_TEXTURE_2D, RENDERER_TEXTURE_WRAP_S, RENDERER_CLAMP_TO_EDGE);
				renderer->TextureParameteri(RENDERER_TEXTURE_2D, RENDERER_TEXTURE_WRAP_T, RENDERER_CLAMP_TO_EDGE);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->GetWidth(), image->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image->GetPixelData());

				delete image;
			}
		}
	}

	API3DUniformBase::InitModifiable();
}

void API3DUniformDataTexture::Activate(unsigned int a_Location)
{
	if (myTextureGLIndex == (unsigned int)-1) // create texture in first predraw
	{
		return;
	}

	GLPRINT("%s %d>%d\n", getName().c_str(), a_Location, (int)myTextureChannel);
	glUniform1i(a_Location, myTextureChannel);
	RendererOpenGL* renderer = static_cast<RendererOpenGL*>(ModuleRenderer::theGlobalRenderer);  // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	renderer->ActiveTextureChannel(myTextureChannel); 
	renderer->BindTexture(RENDERER_TEXTURE_2D, myTextureGLIndex);

}

void	API3DUniformDataTexture::NotifyUpdate(const unsigned int  labelid)
{

	API3DUniformBase::NotifyUpdate(labelid);

}


bool	API3DUniformDataTexture::Deactivate(unsigned int a_Location)
{
	if (myTextureGLIndex == (unsigned int)-1) // create texture in first predraw
	{
		return false;
	}
	RendererOpenGL* renderer = static_cast<RendererOpenGL*>(ModuleRenderer::theGlobalRenderer);  // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	renderer->ActiveTextureChannel(myTextureChannel);
	renderer->BindTexture(RENDERER_TEXTURE_2D, 0);
	return true;
}



API3DUniformDataTexture::~API3DUniformDataTexture()
{
	
}

API3DUniformGeneratedTexture::API3DUniformGeneratedTexture(const kstl::string& name, CLASS_NAME_TREE_ARG) : API3DUniformBase(name, PASS_CLASS_NAME_TREE_ARG)
, myTextureChannel(*this, false, "Channel", 0)
, mySize(*this, true, "Size")
, myScale(*this, true, "Scale", 0.5f)
, myPersistence(*this, true, "Persistence", 0.5f)
, myOctaveCount(*this, true, "OctaveCount", 3)
, myTextureGLIndex((unsigned int)-1)
{
	myTextureChannel.changeNotificationLevel(Owner);
}

void API3DUniformGeneratedTexture::Activate(unsigned int a_Location)
{
	if (myTextureGLIndex == (unsigned int)-1) // create texture in first predraw
	{
		Generate();
	}

	GLPRINT("%s %d>%d\n", getName().c_str(), a_Location, (int)myTextureChannel);
	glUniform1i(a_Location, myTextureChannel);
	RendererOpenGL* renderer = static_cast<RendererOpenGL*>(ModuleRenderer::theGlobalRenderer);  // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	renderer->ActiveTextureChannel(myTextureChannel);
	renderer->BindTexture(RENDERER_TEXTURE_3D, myTextureGLIndex);
}

void	API3DUniformGeneratedTexture::NotifyUpdate(const unsigned int  labelid)
{
	
	API3DUniformBase::NotifyUpdate(labelid);
	
}


bool	API3DUniformGeneratedTexture::Deactivate(unsigned int a_Location)
{
	RendererOpenGL* renderer = static_cast<RendererOpenGL*>(ModuleRenderer::theGlobalRenderer);  // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	renderer->ActiveTextureChannel(myTextureChannel);
	renderer->BindTexture(RENDERER_TEXTURE_3D, 0);
	return true;
}

API3DUniformGeneratedTexture::~API3DUniformGeneratedTexture()
{
	ModuleSceneGraph* scenegraph = static_cast<ModuleSceneGraph*>(KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex));
	scenegraph->AddDefferedItem((void*)myTextureGLIndex, DefferedAction::DESTROY_TEXTURE);
}

void API3DUniformGeneratedTexture::Generate()
{
#ifndef GL_ES2
	RendererOpenGL* renderer = static_cast<RendererOpenGL*>(RendererOpenGL::theGlobalRenderer); // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	int Sizes[3];

	Sizes[0] = (int)mySize[0];
	Sizes[1] = (int)mySize[1];
	Sizes[2] = (int)mySize[2];

	// always generate 3 dimension, but check if second and third are valid
	if (Sizes[1] < 1)
	{
		Sizes[1] = 1;
	}
	if (Sizes[2] < 1)
	{
		Sizes[2] = 1;
	}

	unsigned char* textureRaw = new unsigned char[Sizes[2] * Sizes[1] * Sizes[0]];
	unsigned char*	writeTextureRaw = textureRaw;

	int i, j, k;
	for (k = 0; k < Sizes[2]; k++)
	{
		for (j = 0; j < Sizes[1]; j++)
		{
			for (i = 0; i < Sizes[0]; i++)
			{
				*writeTextureRaw = (unsigned char)scaled_octave_noise_3d((float)myOctaveCount, myPersistence, myScale, -128.0f, 127.0f, (float)i, (float)j, (float)k);
				++writeTextureRaw;
			}
		}
	}

	renderer->CreateTexture(1, &myTextureGLIndex);

	//glBindTexture(GL_TEXTURE_3D, myTextureGLIndex);
	renderer->ActiveTextureChannel(myTextureChannel);
	renderer->BindTexture(RENDERER_TEXTURE_3D, myTextureGLIndex);
	renderer->TextureParameteri(RENDERER_TEXTURE_3D, RENDERER_TEXTURE_MIN_FILTER, RENDERER_LINEAR);
	renderer->TextureParameteri(RENDERER_TEXTURE_3D, RENDERER_TEXTURE_MAG_FILTER, RENDERER_LINEAR);
	renderer->TextureParameteri(RENDERER_TEXTURE_3D, RENDERER_TEXTURE_WRAP_S, RENDERER_REPEAT);
	renderer->TextureParameteri(RENDERER_TEXTURE_3D, RENDERER_TEXTURE_WRAP_T, RENDERER_REPEAT);
	renderer->TextureParameteri(RENDERER_TEXTURE_3D, RENDERER_TEXTURE_WRAP_R, RENDERER_REPEAT);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, Sizes[0], Sizes[1], Sizes[2], 0, GL_RED, GL_UNSIGNED_BYTE, textureRaw);

	delete[] textureRaw;
#endif
}


API3DUniformMatrixArray::API3DUniformMatrixArray(const kstl::string& name, CLASS_NAME_TREE_ARG) : API3DUniformBase(name, PASS_CLASS_NAME_TREE_ARG)
, myArraySize(*this, false, "ArraySize", 16)
, myMatrixArrayAccess(*this, false, "MatrixArray")
, myMatrixArray(0)
{
	NotifyUpdate(myArraySize.getLabelID().toUInt());

	myArraySize.changeNotificationLevel(Owner);

	
}

void	API3DUniformMatrixArray::NotifyUpdate(const unsigned int  labelid)
{
	API3DUniformBase::NotifyUpdate(labelid);

	if(labelid == myArraySize.getLabelID())
	{
		if (myMatrixArray)
		{
			delete[] myMatrixArray;
		}
		myMatrixArray = new Matrix4x4[myArraySize];
		for (int i = 0; i < myArraySize; i++)
		{
			myMatrixArray[i].SetIdentity();
		}
		myMatrixArrayAccess.SetBuffer(myMatrixArray, myArraySize * sizeof(Matrix4x4), false);
	}
	
}

void	API3DUniformMatrixArray::Activate(unsigned int a_Location)
{
	GLPRINT("%s (matrix) %d\n", getName().c_str(), a_Location);
	glUniformMatrix4fv(a_Location, myArraySize, false, &((myMatrixArray[0]).e[0][0]));
}

API3DUniformMatrixArray::~API3DUniformMatrixArray()
{
	if (myMatrixArray)
	{
		delete[] myMatrixArray;
		myMatrixArray = 0;
	}
}



