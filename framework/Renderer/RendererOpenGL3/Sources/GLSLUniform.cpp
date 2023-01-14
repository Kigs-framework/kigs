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

using namespace Kigs::Draw;

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

API3DUniformBase::API3DUniformBase(const std::string& name, CLASS_NAME_TREE_ARG) 
	: Drawable(name, PASS_CLASS_NAME_TREE_ARG)
, mUniName(*this, false, "Name", "")
,mID(-1)
{
	mRenderPassMask = 0xFFFFFFFF;
}

void	API3DUniformBase::InitModifiable()
{
	Drawable::InitModifiable();

	std::string strKey;
	mUniName.getValue(strKey,this);
	mID = CharToID::GetID(strKey);

	mUniName.changeNotificationLevel(Owner);
}

void	API3DUniformBase::NotifyUpdate(const unsigned int  labelid)
{
	if (labelid == mUniName.getLabelID())
	{
		std::string strKey;
		mUniName.getValue(strKey,this);
		mID = CharToID::GetID(strKey);
		//printf("%s >> %u\n", strKey.c_str(), mID);
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
API3DUniformInt::API3DUniformInt(const std::string& name, CLASS_NAME_TREE_ARG) : API3DUniformBase(name, PASS_CLASS_NAME_TREE_ARG)
, mValue(*this, false, "Value", -1)
{
	
}

void	API3DUniformInt::Activate(unsigned int a_Location)
{
	GLPRINT("%s %d>%d\n", getName().c_str(), a_Location, (int)mValue);
	glUniform1i(a_Location, mValue);
}

void	API3DUniformInt::NotifyUpdate(const unsigned int  labelid)
{
	
	
		API3DUniformBase::NotifyUpdate(labelid);
	
}


//////////////////////////////////// Float ///////////////////////////////////////////
API3DUniformFloat::API3DUniformFloat(const std::string& name, CLASS_NAME_TREE_ARG) : API3DUniformBase(name, PASS_CLASS_NAME_TREE_ARG)
, mValue(*this, false, "Value", -1)
{
	
}

void	API3DUniformFloat::Activate(unsigned int a_Location)
{
	GLPRINT("%s %d>%0.2f\n", getName().c_str(), a_Location, (float)mValue);
	glUniform1f(a_Location, mValue);
}

void	API3DUniformFloat::NotifyUpdate(const unsigned int  labelid)
{
	
		API3DUniformBase::NotifyUpdate(labelid);
	
}

//////////////////////////////////// Float 2 ///////////////////////////////////////////
API3DUniformFloat2::API3DUniformFloat2(const std::string& name, CLASS_NAME_TREE_ARG) : API3DUniformBase(name, PASS_CLASS_NAME_TREE_ARG)
, mValue(*this, false, "Value",0.0f,0.0f)
{
	
}

void	API3DUniformFloat2::Activate(unsigned int a_Location)
{
	GLPRINT("%s %d>(%0.2f:%0.2f)\n", getName().c_str(), a_Location, mValue[0], mValue[1]);
	glUniform2f(a_Location, mValue[0], mValue[1]);
}

void	API3DUniformFloat2::NotifyUpdate(const unsigned int  labelid)
{
	
	API3DUniformBase::NotifyUpdate(labelid);
	
}


//////////////////////////////////// Float 3 ///////////////////////////////////////////
API3DUniformFloat3::API3DUniformFloat3(const std::string& name, CLASS_NAME_TREE_ARG) : API3DUniformBase(name, PASS_CLASS_NAME_TREE_ARG)
, mValue(*this, false, "Value",0.0f,0.0f,0.0f)
, mNormalize(*this, false, "Normalize", false)
{

}

void API3DUniformFloat3::InitModifiable()
{
	API3DUniformBase::InitModifiable();

	if (mNormalize)
		Normalize();

	mNormalize.changeNotificationLevel(Owner);
}

void API3DUniformFloat3::NotifyUpdate(const unsigned int labelid)
{
	if (!IsInit())
		return;

	if (labelid == mNormalize.getLabelID())
	{
		if (mNormalize)
			Normalize();
	}
	else if (labelid == mValue.getLabelID())
	{
		if (mNormalize)
			Normalize();
	}
	else
		API3DUniformBase::NotifyUpdate(labelid);
}

void API3DUniformFloat3::Normalize()
{
	Vector3D v(mValue[0], mValue[1], mValue[2]);
	v.Normalize();

	mValue[0] = v.x;
	mValue[1] = v.y;
	mValue[2] = v.z;
}

void	API3DUniformFloat3::Activate(unsigned int a_Location)
{
	GLPRINT("%s %d>(%0.2f:%0.2f:%0.2f)\n", getName().c_str(), a_Location, mValue[0], mValue[1], mValue[2]);
	glUniform3f(a_Location, mValue[0], mValue[1], mValue[2]);
}

//////////////////////////////////// Float 4 ///////////////////////////////////////////
API3DUniformFloat4::API3DUniformFloat4(const std::string& name, CLASS_NAME_TREE_ARG) : API3DUniformBase(name, PASS_CLASS_NAME_TREE_ARG)
, mValue(*this, false, "Value",0.0f,0.0f,0.0f,0.0f)
{
	mValue.changeNotificationLevel(Owner);
}

void	API3DUniformFloat4::Activate(unsigned int a_Location)
{
	GLPRINT("%s %d>(%0.2f:%0.2f:%0.2f:%0.2f)\n", getName().c_str(), a_Location, mValue[0], mValue[1], mValue[2], mValue[3]);
	glUniform4f(a_Location, mValue[0], mValue[1], mValue[2], mValue[3]);
}

void	API3DUniformFloat4::NotifyUpdate(const unsigned int  labelid)
{
	
	API3DUniformBase::NotifyUpdate(labelid);
	
}

API3DUniformTexture::API3DUniformTexture(const std::string& name, CLASS_NAME_TREE_ARG) : API3DUniformBase(name, PASS_CLASS_NAME_TREE_ARG)
, mChannel(*this, false, "Channel", 0)
, mTextureName(*this, false, "TextureName", "")
, mAttachedTexture(0)
{
	mChannel.changeNotificationLevel(Owner);
}

void	API3DUniformTexture::InitModifiable()
{
	if (!IsInit())
	{
		if ((mTextureName.const_ref()) != "")
		{
			auto textureManager = KigsCore::Singleton<TextureFileManager>();
			mAttachedTexture = textureManager->GetTexture(mTextureName.const_ref(), false);
			if (mAttachedTexture)
				if (!mAttachedTexture->IsInit())
				{
					mAttachedTexture->setValue("ForcePow2", true);
					mAttachedTexture->setValue("HasMipmap", true);
					mAttachedTexture->Init();
				}
		}
	}

	API3DUniformBase::InitModifiable();
}

void API3DUniformTexture::Activate(unsigned int a_Location)
{	
	if (mAttachedTexture)
	{
		GLPRINT("%s %d>%d\n", getName().c_str(), a_Location, (int)mChannel);
		glUniform1i(a_Location, mChannel);
		RendererOpenGL* renderer = static_cast<RendererOpenGL*>(ModuleRenderer::mTheGlobalRenderer); // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
		
		renderer->ActiveTextureChannel(mChannel);
		renderer->BindTexture(RENDERER_TEXTURE_2D, static_cast<OpenGLTexture*>(mAttachedTexture.get())->GetGLID());
	}
}

void	API3DUniformTexture::NotifyUpdate(const unsigned int  labelid)
{
	
	API3DUniformBase::NotifyUpdate(labelid);
	
}


bool	API3DUniformTexture::Deactivate(unsigned int a_Location)
{
	RendererOpenGL* renderer = static_cast<RendererOpenGL*>(ModuleRenderer::mTheGlobalRenderer);  // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	renderer->ActiveTextureChannel(mChannel);
	renderer->BindTexture(RENDERER_TEXTURE_2D, 0);
	return true;
}

bool API3DUniformTexture::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	if (item->isSubType(Texture::mClassID)) // if texture, don't call father addItem
	{
		mAttachedTexture = item;
		return true;
	}

	return API3DUniformBase::addItem(item, pos PASS_LINK_NAME(linkName));

}

bool API3DUniformTexture::removeItem(const CMSP& item DECLARE_LINK_NAME)
{
	if (item->isSubType(Texture::mClassID)) // if texture, don't call father removeItem
	{
		if (item == mAttachedTexture)
		{
			mAttachedTexture = 0;
			return true;
		}

		return true;
	}

	return SceneNode::removeItem(item  PASS_LINK_NAME(linkName));
}


API3DUniformTexture::~API3DUniformTexture()
{
	mAttachedTexture = nullptr;
}


API3DUniformDataTexture::API3DUniformDataTexture(const std::string& name, CLASS_NAME_TREE_ARG) : API3DUniformBase(name, PASS_CLASS_NAME_TREE_ARG)
, mChannel(*this, false, "Channel", 0)
, mTextureName(*this, false, "TextureName", "")
, mTextureGLIndex(-1)
{
	mChannel.changeNotificationLevel(Owner);
}

void	API3DUniformDataTexture::InitModifiable()
{
	if (!IsInit())
	{
		if ((mTextureName.const_ref()) != "")
		{
			auto pathManager = KigsCore::Singleton<File::FilePathManager>();
			SmartPointer<File::FileHandle> fullfilenamehandle = pathManager->FindFullName(mTextureName.const_ref());
			if (fullfilenamehandle)
			{
				SP<Pict::TinyImage> image = Pict::TinyImage::CreateImage(fullfilenamehandle.get());
				RendererOpenGL* renderer = static_cast<RendererOpenGL*>(ModuleRenderer::mTheGlobalRenderer);  // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
				renderer->CreateTexture(1, &mTextureGLIndex);
				//glBindTexture(GL_TEXTURE_3D, mTextureGLIndex);
				renderer->ActiveTextureChannel(mChannel);
				renderer->BindTexture(RENDERER_TEXTURE_2D, mTextureGLIndex);
				renderer->TextureParameteri(RENDERER_TEXTURE_2D, RENDERER_TEXTURE_MIN_FILTER, RENDERER_NEAREST);
				renderer->TextureParameteri(RENDERER_TEXTURE_2D, RENDERER_TEXTURE_MAG_FILTER, RENDERER_NEAREST);
				renderer->TextureParameteri(RENDERER_TEXTURE_2D, RENDERER_TEXTURE_WRAP_S, RENDERER_CLAMP_TO_EDGE);
				renderer->TextureParameteri(RENDERER_TEXTURE_2D, RENDERER_TEXTURE_WRAP_T, RENDERER_CLAMP_TO_EDGE);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->GetWidth(), image->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image->GetPixelData());
			}
		}
	}

	API3DUniformBase::InitModifiable();
}

void API3DUniformDataTexture::Activate(unsigned int a_Location)
{
	if (mTextureGLIndex == (unsigned int)-1) // create texture in first predraw
	{
		return;
	}

	GLPRINT("%s %d>%d\n", getName().c_str(), a_Location, (int)mChannel);
	glUniform1i(a_Location, mChannel);
	RendererOpenGL* renderer = static_cast<RendererOpenGL*>(ModuleRenderer::mTheGlobalRenderer);  // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	renderer->ActiveTextureChannel(mChannel); 
	renderer->BindTexture(RENDERER_TEXTURE_2D, mTextureGLIndex);

}

void	API3DUniformDataTexture::NotifyUpdate(const unsigned int  labelid)
{

	API3DUniformBase::NotifyUpdate(labelid);

}


bool	API3DUniformDataTexture::Deactivate(unsigned int a_Location)
{
	if (mTextureGLIndex == (unsigned int)-1) // create texture in first predraw
	{
		return false;
	}
	RendererOpenGL* renderer = static_cast<RendererOpenGL*>(ModuleRenderer::mTheGlobalRenderer);  // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	renderer->ActiveTextureChannel(mChannel);
	renderer->BindTexture(RENDERER_TEXTURE_2D, 0);
	return true;
}



API3DUniformDataTexture::~API3DUniformDataTexture()
{
	
}

API3DUniformGeneratedTexture::API3DUniformGeneratedTexture(const std::string& name, CLASS_NAME_TREE_ARG) : API3DUniformBase(name, PASS_CLASS_NAME_TREE_ARG)
, mChannel(*this, false, "Channel", 0)
, mSize(*this, true, "Size")
, mScale(*this, true, "Scale", 0.5f)
, mPersistence(*this, true, "Persistence", 0.5f)
, mOctaveCount(*this, true, "OctaveCount", 3)
, mTextureGLIndex((unsigned int)-1)
{
	mChannel.changeNotificationLevel(Owner);
}

void API3DUniformGeneratedTexture::Activate(unsigned int a_Location)
{
	if (mTextureGLIndex == (unsigned int)-1) // create texture in first predraw
	{
		Generate();
	}

	GLPRINT("%s %d>%d\n", getName().c_str(), a_Location, (int)mChannel);
	glUniform1i(a_Location, mChannel);
	RendererOpenGL* renderer = static_cast<RendererOpenGL*>(ModuleRenderer::mTheGlobalRenderer);  // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	renderer->ActiveTextureChannel(mChannel);
	renderer->BindTexture(RENDERER_TEXTURE_3D, mTextureGLIndex);
}

void	API3DUniformGeneratedTexture::NotifyUpdate(const unsigned int  labelid)
{
	
	API3DUniformBase::NotifyUpdate(labelid);
	
}


bool	API3DUniformGeneratedTexture::Deactivate(unsigned int a_Location)
{
	RendererOpenGL* renderer = static_cast<RendererOpenGL*>(ModuleRenderer::mTheGlobalRenderer);  // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	renderer->ActiveTextureChannel(mChannel);
	renderer->BindTexture(RENDERER_TEXTURE_3D, 0);
	return true;
}

API3DUniformGeneratedTexture::~API3DUniformGeneratedTexture()
{
	ModuleSceneGraph* scenegraph = static_cast<ModuleSceneGraph*>(KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex));
	scenegraph->AddDefferedItem((void*)(uintptr_t)mTextureGLIndex, DefferedAction::DESTROY_TEXTURE);
}

void API3DUniformGeneratedTexture::Generate()
{
#ifndef GL_ES2
	RendererOpenGL* renderer = static_cast<RendererOpenGL*>(ModuleRenderer::mTheGlobalRenderer); // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	int Sizes[3];

	Sizes[0] = (int)mSize[0];
	Sizes[1] = (int)mSize[1];
	Sizes[2] = (int)mSize[2];

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
				*writeTextureRaw = (unsigned char)scaled_octave_noise_3d((float)mOctaveCount, mPersistence, mScale, -128.0f, 127.0f, (float)i, (float)j, (float)k);
				++writeTextureRaw;
			}
		}
	}

	renderer->CreateTexture(1, &mTextureGLIndex);

	//glBindTexture(GL_TEXTURE_3D, mTextureGLIndex);
	renderer->ActiveTextureChannel(mChannel);
	renderer->BindTexture(RENDERER_TEXTURE_3D, mTextureGLIndex);
	renderer->TextureParameteri(RENDERER_TEXTURE_3D, RENDERER_TEXTURE_MIN_FILTER, RENDERER_LINEAR);
	renderer->TextureParameteri(RENDERER_TEXTURE_3D, RENDERER_TEXTURE_MAG_FILTER, RENDERER_LINEAR);
	renderer->TextureParameteri(RENDERER_TEXTURE_3D, RENDERER_TEXTURE_WRAP_S, RENDERER_REPEAT);
	renderer->TextureParameteri(RENDERER_TEXTURE_3D, RENDERER_TEXTURE_WRAP_T, RENDERER_REPEAT);
	renderer->TextureParameteri(RENDERER_TEXTURE_3D, RENDERER_TEXTURE_WRAP_R, RENDERER_REPEAT);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, Sizes[0], Sizes[1], Sizes[2], 0, GL_RED, GL_UNSIGNED_BYTE, textureRaw);

	delete[] textureRaw;
#endif
}


API3DUniformMatrixArray::API3DUniformMatrixArray(const std::string& name, CLASS_NAME_TREE_ARG) : API3DUniformBase(name, PASS_CLASS_NAME_TREE_ARG)
, mArraySize(*this, false, "ArraySize", 16)
, mMatrixArray(*this, false, "MatrixArray")
, mMatrixArrayPointer(0)
{
	NotifyUpdate(mArraySize.getLabelID().toUInt());

	mArraySize.changeNotificationLevel(Owner);

	
}

void	API3DUniformMatrixArray::NotifyUpdate(const unsigned int  labelid)
{
	API3DUniformBase::NotifyUpdate(labelid);

	if(labelid == mArraySize.getLabelID())
	{
		if (mMatrixArrayPointer)
		{
			delete[] mMatrixArrayPointer;
		}
		mMatrixArrayPointer = new Matrix4x4[mArraySize];
		for (int i = 0; i < mArraySize; i++)
		{
			mMatrixArrayPointer[i].SetIdentity();
		}
		mMatrixArray.SetBuffer(mMatrixArrayPointer, mArraySize * sizeof(Matrix4x4), false);
	}
	
}

void	API3DUniformMatrixArray::Activate(unsigned int a_Location)
{
	GLPRINT("%s (matrix) %d\n", getName().c_str(), a_Location);
	glUniformMatrix4fv(a_Location, mArraySize, false, &((mMatrixArrayPointer[0]).e[0][0]));
}

API3DUniformMatrixArray::~API3DUniformMatrixArray()
{
	if (mMatrixArrayPointer)
	{
		delete[] mMatrixArrayPointer;
		mMatrixArrayPointer = 0;
	}
}



