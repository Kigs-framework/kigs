#include "PrecompiledHeaders.h"
#include "HLSLUniform.h"
#include "Core.h"
#include "RendererDX11.h"
#include "DX11Texture.h"
#include "Shader.h"
#include "TravState.h"
#include "simplexnoise.h"
#include "TextureFileManager.h"
#include "FilePathManager.h"
#include "ModuleSceneGraph.h"


IMPLEMENT_CLASS_INFO(API3DUniformBase)
IMPLEMENT_CLASS_INFO(API3DUniformInt)
IMPLEMENT_CLASS_INFO(API3DUniformFloat)
IMPLEMENT_CLASS_INFO(API3DUniformFloat2)
IMPLEMENT_CLASS_INFO(API3DUniformFloat3)
IMPLEMENT_CLASS_INFO(API3DUniformFloat4)
IMPLEMENT_CLASS_INFO(API3DUniformTexture)
#if DX11
IMPLEMENT_CLASS_INFO(API3DUniformDataTexture)
IMPLEMENT_CLASS_INFO(API3DUniformGeneratedTexture)
#endif
IMPLEMENT_CLASS_INFO(API3DUniformMatrixArray)
IMPLEMENT_CLASS_INFO(API3DUniformBuffer)


API3DUniformBase::API3DUniformBase(const kstl::string& name, CLASS_NAME_TREE_ARG) 
	: Drawable(name, PASS_CLASS_NAME_TREE_ARG)
, mUniName(*this, false, "Name", "")
,mID(-1)
{
	mRenderPassMask = 0xFFFFFFFF;
}

void API3DUniformBase::InitModifiable()
{
	Drawable::InitModifiable();

	kstl::string strKey;
	mUniName.getValue(strKey);
	mID = CharToID::GetID(strKey);

	mUniName.changeNotificationLevel(Owner);
}

void API3DUniformBase::NotifyUpdate(const unsigned int  labelid)
{
	if (labelid == mUniName.getLabelID())
	{
		kstl::string strKey;
		mUniName.getValue(strKey);
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

 void API3DUniformBase::ProtectedDestroy()
 {
	 ParentClassType::ProtectedDestroy();
	 if (mCBuffer)
	 {
		 mCBuffer->Release();
		 mCBuffer = nullptr;
	 }
 }

 void API3DUniformBase::CreateBufferIfNeeded()
 {
	 if (mCBBufferNeededSize > mCBBufferCurrentSize)
	 {
		 if (mCBuffer) mCBuffer->Release();

		 auto device = ModuleRenderer::mTheGlobalRenderer->as<RendererDX11>()->getDXInstance();
		 D3D11_BUFFER_DESC mBufferDesc;
		 mBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		 mBufferDesc.ByteWidth = mCBBufferNeededSize;
		 if (mBufferDesc.ByteWidth % 16 != 0) mBufferDesc.ByteWidth = (mBufferDesc.ByteWidth / 16 + 1) * 16;
		 mBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		 mBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		 mBufferDesc.MiscFlags = 0;
		 mBufferDesc.StructureByteStride = 0;
		 device->mDevice->CreateBuffer(&mBufferDesc, NULL, &mCBuffer);

		 mCBBufferCurrentSize = mBufferDesc.ByteWidth;
		 mCBBufferNeededSize = 0;
	 }
 }


//////////////////////////////////// Int ///////////////////////////////////////////
API3DUniformInt::API3DUniformInt(const kstl::string& name, CLASS_NAME_TREE_ARG) : API3DUniformBase(name, PASS_CLASS_NAME_TREE_ARG)
, mValue(*this, false, "Value", -1)
{
	mCBBufferNeededSize = sizeof(int);
}

void	API3DUniformInt::Activate(UniformList* ul)
{
	CreateBufferIfNeeded();

	auto device = ModuleRenderer::mTheGlobalRenderer->as<RendererDX11>()->getDXInstance();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	int* dataPtr;

	device->mDeviceContext->Map(mCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (int*)mappedResource.pData;
	*dataPtr = mValue;
	device->mDeviceContext->Unmap(mCBuffer, 0);

	if (ul->mLocation != 0xffffffff) device->mDeviceContext->VSSetConstantBuffers(ul->mLocation, 1, &mCBuffer);
	if (ul->mLocationFragment != 0xffffffff) device->mDeviceContext->PSSetConstantBuffers(ul->mLocationFragment, 1, &mCBuffer);
}

void	API3DUniformInt::NotifyUpdate(const unsigned int  labelid)
{
	API3DUniformBase::NotifyUpdate(labelid);	
}


//////////////////////////////////// Float ///////////////////////////////////////////
API3DUniformFloat::API3DUniformFloat(const kstl::string& name, CLASS_NAME_TREE_ARG) : API3DUniformBase(name, PASS_CLASS_NAME_TREE_ARG)
, mValue(*this, false, "Value", -1)
{
	mCBBufferNeededSize = sizeof(float);
}

void	API3DUniformFloat::Activate(UniformList* ul)
{
	CreateBufferIfNeeded();

	auto device = ModuleRenderer::mTheGlobalRenderer->as<RendererDX11>()->getDXInstance();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	float* dataPtr;

	device->mDeviceContext->Map(mCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (float*)mappedResource.pData;
	*dataPtr = mValue;
	device->mDeviceContext->Unmap(mCBuffer, 0);

	if (ul->mLocation != 0xffffffff) device->mDeviceContext->VSSetConstantBuffers(ul->mLocation, 1, &mCBuffer);
	if (ul->mLocationFragment != 0xffffffff) device->mDeviceContext->PSSetConstantBuffers(ul->mLocationFragment, 1, &mCBuffer);
}

void	API3DUniformFloat::NotifyUpdate(const unsigned int  labelid)
{
	
		API3DUniformBase::NotifyUpdate(labelid);
	
}

//////////////////////////////////// Float 2 ///////////////////////////////////////////
API3DUniformFloat2::API3DUniformFloat2(const kstl::string& name, CLASS_NAME_TREE_ARG) : API3DUniformBase(name, PASS_CLASS_NAME_TREE_ARG)
, mValue(*this, false, "Value",0.0f,0.0f)
{
	mCBBufferNeededSize = sizeof(v2f);
}

void	API3DUniformFloat2::Activate(UniformList* ul)
{
	CreateBufferIfNeeded();

	auto device = ModuleRenderer::mTheGlobalRenderer->as<RendererDX11>()->getDXInstance();

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	device->mDeviceContext->Map(mCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, mValue.getVector(), 2 * sizeof(float));
	device->mDeviceContext->Unmap(mCBuffer, 0);

	if (ul->mLocation != 0xffffffff) device->mDeviceContext->VSSetConstantBuffers(ul->mLocation, 1, &mCBuffer);
	if (ul->mLocationFragment != 0xffffffff) device->mDeviceContext->PSSetConstantBuffers(ul->mLocationFragment, 1, &mCBuffer);
}

void	API3DUniformFloat2::NotifyUpdate(const unsigned int  labelid)
{
	
	API3DUniformBase::NotifyUpdate(labelid);
	
}


//////////////////////////////////// Float 3 ///////////////////////////////////////////
API3DUniformFloat3::API3DUniformFloat3(const kstl::string& name, CLASS_NAME_TREE_ARG) : API3DUniformBase(name, PASS_CLASS_NAME_TREE_ARG)
, mValue(*this, false, "Value",0.0f,0.0f,0.0f)
, mNormalize(*this, false, "Normalize", false)
{
	mValue.changeNotificationLevel(Owner);
	mCBBufferNeededSize = sizeof(v3f);
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

void	API3DUniformFloat3::Activate(UniformList* ul)
{
	CreateBufferIfNeeded();

	auto device = ModuleRenderer::mTheGlobalRenderer->as<RendererDX11>()->getDXInstance();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	
	device->mDeviceContext->Map(mCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, mValue.getVector(), 3 * sizeof(float));
	device->mDeviceContext->Unmap(mCBuffer, 0);

	if (ul->mLocation != 0xffffffff) device->mDeviceContext->VSSetConstantBuffers(ul->mLocation, 1, &mCBuffer);
	if (ul->mLocationFragment != 0xffffffff) device->mDeviceContext->PSSetConstantBuffers(ul->mLocationFragment, 1, &mCBuffer);
}

//////////////////////////////////// Float 4 ///////////////////////////////////////////
API3DUniformFloat4::API3DUniformFloat4(const kstl::string& name, CLASS_NAME_TREE_ARG) : API3DUniformBase(name, PASS_CLASS_NAME_TREE_ARG)
, mValue(*this, false, "Value",0.0f,0.0f,0.0f,0.0f)
{
	mCBBufferNeededSize = sizeof(v4f);
}

void	API3DUniformFloat4::Activate(UniformList* ul)
{
	CreateBufferIfNeeded();

	auto device = ModuleRenderer::mTheGlobalRenderer->as<RendererDX11>()->getDXInstance();

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	device->mDeviceContext->Map(mCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, mValue.getVector(), 4 * sizeof(float));
	device->mDeviceContext->Unmap(mCBuffer, 0);

	if (ul->mLocation != 0xffffffff) device->mDeviceContext->VSSetConstantBuffers(ul->mLocation, 1, &mCBuffer);
	if (ul->mLocationFragment != 0xffffffff) device->mDeviceContext->PSSetConstantBuffers(ul->mLocationFragment, 1, &mCBuffer);
}

void	API3DUniformFloat4::NotifyUpdate(const unsigned int  labelid)
{
	
	API3DUniformBase::NotifyUpdate(labelid);
	
}

API3DUniformTexture::API3DUniformTexture(const kstl::string& name, CLASS_NAME_TREE_ARG) : API3DUniformBase(name, PASS_CLASS_NAME_TREE_ARG)
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
			SP<TextureFileManager>	fileManager = KigsCore::GetSingleton("TextureFileManager");
			mAttachedTexture = fileManager->GetTexture(mTextureName.const_ref(), false);
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

void API3DUniformTexture::Activate(UniformList* ul)
{	
	CreateBufferIfNeeded();

	if (mAttachedTexture)
	{
		auto device = ModuleRenderer::mTheGlobalRenderer->as<RendererDX11>()->getDXInstance();
		// TODO
		/*glUniform1i(a_Location, mChannel);
		ModuleSpecificRenderer* renderer = (ModuleRenderer::mTheGlobalRenderer); // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
		
		renderer->ActiveTextureChannel(mChannel);
		renderer->BindTexture(RENDERER_TEXTURE_2D, static_cast<OpenGLTexture*>(mAttachedTexture)->GetGLID());*/
	}
}

void	API3DUniformTexture::NotifyUpdate(const unsigned int  labelid)
{
	
	API3DUniformBase::NotifyUpdate(labelid);
	
}


bool	API3DUniformTexture::Deactivate(UniformList* ul)
{
	ModuleSpecificRenderer* renderer = (ModuleRenderer::mTheGlobalRenderer);  // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	renderer->ActiveTextureChannel(mChannel);
	renderer->BindTexture(RENDERER_TEXTURE_2D, 0);
	return true;
}

bool API3DUniformTexture::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	if (item->isSubType(Texture::mClassID)) // if texture, don't call father addItem
	{
		if (item == mAttachedTexture)
		{
			return true;
		}

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




#if DX11 // TODO ?

API3DUniformDataTexture::API3DUniformDataTexture(const kstl::string& name, CLASS_NAME_TREE_ARG) : API3DUniformBase(name, PASS_CLASS_NAME_TREE_ARG)
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

			FilePathManager* pathManager = (FilePathManager*)KigsCore::GetSingleton("FilePathManager");

			SmartPointer<FileHandle> fullfilenamehandle = pathManager->FindFullName(mTextureName.const_ref());
			if (fullfilenamehandle)
			{

				TinyImage* image = TinyImage::CreateImage(fullfilenamehandle.get());

				ModuleSpecificRenderer* renderer = (ModuleRenderer::mTheGlobalRenderer);  // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();

				renderer->CreateTexture(1, &mTextureGLIndex);

				renderer->ActiveTextureChannel(mChannel);
				renderer->BindTexture(RENDERER_TEXTURE_2D, mTextureGLIndex);
				renderer->TextureParameteri(RENDERER_TEXTURE_2D, RENDERER_TEXTURE_MIN_FILTER, RENDERER_NEAREST);
				renderer->TextureParameteri(RENDERER_TEXTURE_2D, RENDERER_TEXTURE_MAG_FILTER, RENDERER_NEAREST);
				renderer->TextureParameteri(RENDERER_TEXTURE_2D, RENDERER_TEXTURE_WRAP_S, RENDERER_CLAMP_TO_EDGE);
				renderer->TextureParameteri(RENDERER_TEXTURE_2D, RENDERER_TEXTURE_WRAP_T, RENDERER_CLAMP_TO_EDGE);

				// TODO
				//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->GetWidth(), image->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image->GetPixelData());

				delete image;
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

	glUniform1i(a_Location, mChannel);
	ModuleSpecificRenderer* renderer = (ModuleRenderer::mTheGlobalRenderer);  // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
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
	ModuleSpecificRenderer* renderer = (ModuleRenderer::mTheGlobalRenderer);  // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	renderer->ActiveTextureChannel(mChannel);
	renderer->BindTexture(RENDERER_TEXTURE_2D, 0);
	return true;
}



API3DUniformDataTexture::~API3DUniformDataTexture()
{
	
}

API3DUniformGeneratedTexture::API3DUniformGeneratedTexture(const kstl::string& name, CLASS_NAME_TREE_ARG) : API3DUniformBase(name, PASS_CLASS_NAME_TREE_ARG)
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

	
	glUniform1i(a_Location, mChannel);
	ModuleSpecificRenderer* renderer = (ModuleRenderer::mTheGlobalRenderer);  // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	renderer->ActiveTextureChannel(mChannel);
	renderer->BindTexture(RENDERER_TEXTURE_3D, mTextureGLIndex);
}

void	API3DUniformGeneratedTexture::NotifyUpdate(const unsigned int  labelid)
{
	
	API3DUniformBase::NotifyUpdate(labelid);
	
}


bool	API3DUniformGeneratedTexture::Deactivate(unsigned int a_Location)
{
	ModuleSpecificRenderer* renderer = (ModuleRenderer::mTheGlobalRenderer);  // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	renderer->ActiveTextureChannel(mChannel);
	renderer->BindTexture(RENDERER_TEXTURE_3D, 0);
	return true;
}

API3DUniformGeneratedTexture::~API3DUniformGeneratedTexture()
{
	ModuleSceneGraph* scenegraph = static_cast<ModuleSceneGraph*>(KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex));
	scenegraph->AddDefferedItem((void*)mTextureGLIndex, DefferedAction::DESTROY_TEXTURE);
}

void API3DUniformGeneratedTexture::Generate()
{
#ifndef GL_ES2
	ModuleSpecificRenderer* renderer = (ModuleSpecificRenderer::mTheGlobalRenderer); // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
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

#endif // DX11

API3DUniformMatrixArray::API3DUniformMatrixArray(const kstl::string& name, CLASS_NAME_TREE_ARG) : API3DUniformBase(name, PASS_CLASS_NAME_TREE_ARG)
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
		mCBBufferNeededSize = mArraySize * sizeof(Matrix4x4);

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

void	API3DUniformMatrixArray::Activate(UniformList* ul)
{
	CreateBufferIfNeeded();

	auto device = ModuleRenderer::mTheGlobalRenderer->as<RendererDX11>()->getDXInstance();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	device->mDeviceContext->Map(mCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &((mMatrixArrayPointer[0]).e[0][0]), mArraySize * sizeof(Matrix4x4));
	device->mDeviceContext->Unmap(mCBuffer, 0);

	if (ul->mLocation != 0xffffffff) device->mDeviceContext->VSSetConstantBuffers(ul->mLocation, 1, &mCBuffer);
	if (ul->mLocationFragment != 0xffffffff) device->mDeviceContext->PSSetConstantBuffers(ul->mLocationFragment, 1, &mCBuffer);

}

API3DUniformMatrixArray::~API3DUniformMatrixArray()
{
	if (mMatrixArrayPointer)
	{
		delete[] mMatrixArrayPointer;
		mMatrixArrayPointer = 0;
	}
}


API3DUniformBuffer::API3DUniformBuffer(const kstl::string& name, CLASS_NAME_TREE_ARG) : API3DUniformBase(name, PASS_CLASS_NAME_TREE_ARG)
{
}

void	API3DUniformBuffer::Activate(UniformList* ul)
{
	mCBBufferNeededSize = mValue.ref()->size();
	CreateBufferIfNeeded();

	auto device = ModuleRenderer::mTheGlobalRenderer->as<RendererDX11>()->getDXInstance();
	
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	device->mDeviceContext->Map(mCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, mValue.ref()->data(), mValue.ref()->size());
	device->mDeviceContext->Unmap(mCBuffer, 0);

	if (ul->mLocation != 0xffffffff) device->mDeviceContext->VSSetConstantBuffers(ul->mLocation, 1, &mCBuffer);
	if (ul->mLocationFragment != 0xffffffff) device->mDeviceContext->PSSetConstantBuffers(ul->mLocationFragment, 1, &mCBuffer);
}



