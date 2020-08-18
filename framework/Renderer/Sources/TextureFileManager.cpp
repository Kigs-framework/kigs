#include "PrecompiledHeaders.h"

#include "TextureFileManager.h"
#include <stdio.h>
#include "Core.h"
#include "Texture.h"
#include "SpriteSheetTexture.h"
#include "AttributePacking.h"

IMPLEMENT_CLASS_INFO(TextureFileManager)

//! constructor
TextureFileManager::TextureFileManager(const kstl::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG){}

//! destructor
TextureFileManager::~TextureFileManager()
{
	//UnloadAllTexture();
}

bool TextureFileManager::HasTexture(const kstl::string& fileName)
{
	if (myTextureMap.find(fileName) != myTextureMap.end())
		return true;
	return false;
}

void TextureFileManager::AddTexture(const kstl::string& fileName, CoreModifiable* Tex)
{
	if (!HasTexture(fileName))
	{
		myTextureMap[fileName] = Tex;
	}
	else
	{
		KIGS_WARNING("texture already registered in Texture", 2);
	}
}


void TextureFileManager::AddTexture(const kstl::string& fileName, CMSP& Tex)
{
	if (!HasTexture(fileName))
	{
		myTextureMap[fileName] = Tex.get();
	}
	else
	{
		KIGS_WARNING("texture already registered in Texture", 2);
	}
}

CMSP TextureFileManager::CreateTexture(const kstl::string& textureName)
{
	SP<Texture> Tex = KigsCore::GetInstanceOf(textureName, "Texture", true);
	AddTexture(textureName,(CMSP&) Tex);
	return Tex;
}


CMSP TextureFileManager::CreateSpriteSheetTexture(const kstl::string& textureName)
{
	CMSP L_pSpriteSheet = KigsCore::GetInstanceOf(textureName, "SpriteSheetTexture");
	AddTexture(textureName, L_pSpriteSheet);
	return L_pSpriteSheet;
}

SP<Texture> TextureFileManager::GetTexture(const kstl::string& fileName, bool doInit)
{
	// crash in editor when creating an UIImage
	/*if (fileName == "")
		return NULL;*/

	SP<Texture> pTex(nullptr);

	// already loaded ?
	if (HasTexture(fileName))
	{
		pTex = CMSP(myTextureMap[fileName], GetRefTag{});
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


SP<Texture> TextureFileManager::GetTexture(const kstl::string& fileName, const kstl::string& a_textureName, bool doInit)
{
	SP<Texture> pTex;

	// already loaded ?
	if (HasTexture(a_textureName))
	{
		pTex = CMSP(myTextureMap[a_textureName], GetRefTag{});
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


SP<SpriteSheetTexture> TextureFileManager::GetSpriteSheetTexture(const kstl::string& fileName)
{
	if (fileName == "")
		return NULL;

	SP<SpriteSheetTexture> L_pSpriteSheet;

	// already loaded ?
	if (HasTexture(fileName))
	{
		L_pSpriteSheet = CMSP(myTextureMap[fileName], GetRefTag{});
	}
	else
	{
		L_pSpriteSheet = CreateSpriteSheetTexture(fileName);
		L_pSpriteSheet->setValue(LABEL_TO_ID(FileName), fileName);
		L_pSpriteSheet->Init();
	}

	return L_pSpriteSheet;
}
 

void TextureFileManager::ResetAllTexture()
{
	kstl::map<kstl::string, CoreModifiable*>::iterator	it = myTextureMap.begin();
	for (; it != myTextureMap.end(); ++it)
	{
		if ((*it).second->isSubType(Texture::mClassID))
		{
			((Texture*)(*it).second)->SetFlag(Texture::isDirtyContext);
			(*it).second->ReInit();
		}
	}
}

void TextureFileManager::UnloadAllTexture()
{
	myTextureMap.clear();
}

void TextureFileManager::UnloadTexture(Texture* Tex)
{
	// search this texture
	kstl::map<kstl::string, CoreModifiable*>::iterator	it;
	for (it = myTextureMap.begin(); it != myTextureMap.end(); ++it)
	{
		if (it->second == Tex)
		{
			myTextureMap.erase(it);
			break;
		}
	}
}

void TextureFileManager::UnloadTexture(SpriteSheetTexture* Tex)
{
	// search this texture

	kstl::map<kstl::string, CoreModifiable*>::iterator	it;

	for (it = myTextureMap.begin(); it != myTextureMap.end(); ++it)
	{
		if ((*it).second == Tex)
		{
			myTextureMap.erase(it);
			break;
		}
	}
}


void TextureFileManager::ClearCache()
{
	myTextureMap.clear();
}
