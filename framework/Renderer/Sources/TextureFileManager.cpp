#include "PrecompiledHeaders.h"

#include "TextureFileManager.h"
#include <stdio.h>
#include "Core.h"
#include "Texture.h"
#include "AttributePacking.h"

using namespace Kigs::Draw;

IMPLEMENT_CLASS_INFO(TextureFileManager)

//! constructor
TextureFileManager::TextureFileManager(const std::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG){}

//! destructor
TextureFileManager::~TextureFileManager()
{
	//UnloadAllTexture();
}

bool TextureFileManager::HasTexture(const std::string& fileName)
{
	if (mTextureMap.find(fileName) != mTextureMap.end())
		return true;
	return false;
}

void TextureFileManager::AddTexture(const std::string& fileName, CoreModifiable* Tex)
{
	if (!HasTexture(fileName))
	{
		mTextureMap[fileName] = Tex;
	}
	else
	{
		KIGS_WARNING("texture already registered in Texture", 2);
	}
}


void TextureFileManager::AddTexture(const std::string& fileName, const CMSP& Tex)
{
	if (!HasTexture(fileName))
	{
		mTextureMap[fileName] = Tex.get();
	}
	else
	{
		KIGS_WARNING("texture already registered in Texture", 2);
	}
}

CMSP TextureFileManager::CreateTexture(const std::string& textureName)
{
	SP<Texture> Tex = KigsCore::GetInstanceOf(textureName, "Texture", true);
	AddTexture(textureName, Tex);
	return Tex;
}


SP<Texture> TextureFileManager::GetTexture(const std::string& fileName, bool doInit)
{
	SP<Texture> pTex(nullptr);

	// already loaded ?
	if (HasTexture(fileName))
	{
		pTex = mTextureMap[fileName]->SharedFromThis();
	}
	else
	{
		pTex = CreateTexture(fileName);
		pTex->setValue("FileName", fileName);
		if (doInit)
			pTex->Init();
	}

	return pTex;
}


SP<Texture> TextureFileManager::GetTexture(const std::string& fileName, const std::string& a_textureName, bool doInit)
{
	SP<Texture> pTex;

	// already loaded ?
	if (HasTexture(a_textureName))
	{
		pTex = mTextureMap[a_textureName]->SharedFromThis();
	}
	else
	{
		pTex = CreateTexture(a_textureName);
		pTex->setValue("FileName", fileName);
		if (doInit)
			pTex->Init();
	}

	return pTex;
}


void TextureFileManager::ResetAllTexture()
{
	std::map<std::string, CoreModifiable*>::iterator	it = mTextureMap.begin();
	for (; it != mTextureMap.end(); ++it)
	{
		if ((*it).second->isSubType(Texture::mClassID))
		{
			((Texture*)(*it).second)->setUserFlag(Texture::isDirtyContext);
			(*it).second->ReInit();
		}
	}
}

void TextureFileManager::UnloadAllTexture()
{
	mTextureMap.clear();
}

void TextureFileManager::UnloadTexture(Texture* Tex)
{
	// search this texture
	std::map<std::string, CoreModifiable*>::iterator	it;
	for (it = mTextureMap.begin(); it != mTextureMap.end(); ++it)
	{
		if (it->second == Tex)
		{
			mTextureMap.erase(it);
			break;
		}
	}
}

void TextureFileManager::ClearCache()
{
	mTextureMap.clear();
}
