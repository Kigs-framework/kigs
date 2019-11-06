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

CoreModifiable* TextureFileManager::CreateTexture(const kstl::string& textureName)
{
	Texture* Tex = (Texture*)(KigsCore::GetInstanceOf(textureName, "Texture", true));
	AddTexture(textureName, Tex);
	return Tex;
}


CoreModifiable* TextureFileManager::CreateSpriteSheetTexture(const kstl::string& textureName)
{
	CoreModifiable* L_pSpriteSheet = (KigsCore::GetInstanceOf(textureName, "SpriteSheetTexture"));
	AddTexture(textureName, L_pSpriteSheet);

	return L_pSpriteSheet;
}

Texture* TextureFileManager::GetTexture(const kstl::string& fileName, bool doInit)
{
	// crash in editor when creating an UIImage
	/*if (fileName == "")
		return NULL;*/

	CoreModifiable *pTex;

	// already loaded ?
	if (HasTexture(fileName))
	{
		pTex = myTextureMap[fileName];
		pTex->GetRef();
	}
	else
	{
		pTex = CreateTexture(fileName);
		pTex->setValue("FileName", fileName);
		if (doInit)
			pTex->Init();
	}

	return (Texture*)pTex;
}

SmartPointer<Texture> TextureFileManager::GetTextureManaged(const kstl::string& fileName, bool doInit)
{
	bool has_texture = HasTexture(fileName);
	SmartPointer<Texture> result = OwningRawPtrToSmartPtr(GetTexture(fileName, doInit));
	//if (!has_texture)
	//	result->GetRef();
	return result;
}

Texture* TextureFileManager::GetTexture(const kstl::string& fileName, const kstl::string& a_textureName, bool doInit)
{
	CoreModifiable *pTex;

	// already loaded ?
	if (HasTexture(a_textureName))
	{
		pTex = myTextureMap[a_textureName];
		pTex->GetRef();
	}
	else
	{
		pTex = CreateTexture(a_textureName);
		pTex->setValue("FileName", fileName);
		if (doInit)
			pTex->Init();
	}

	return (Texture*)pTex;
}


SpriteSheetTexture* TextureFileManager::GetSpriteSheetTexture(const kstl::string& fileName)
{
	if (fileName == "")
		return NULL;

	CoreModifiable* L_pSpriteSheet;

	// already loaded ?
	if (HasTexture(fileName))
	{
		L_pSpriteSheet = myTextureMap[fileName];
		L_pSpriteSheet->GetRef();
	}
	else
	{
		L_pSpriteSheet = CreateSpriteSheetTexture(fileName);
		L_pSpriteSheet->setValue(LABEL_TO_ID(FileName), fileName);
		L_pSpriteSheet->Init();
	}

	return (SpriteSheetTexture*)L_pSpriteSheet;
}


void TextureFileManager::ResetAllTexture()
{
	kstl::map<kstl::string, CoreModifiable*>::iterator	it = myTextureMap.begin();
	for (; it != myTextureMap.end(); ++it)
	{
		if ((*it).second->isSubType(Texture::myClassID))
		{
			static_cast<Texture*>((*it).second)->SetFlag(Texture::isDirtyContext);
			(*it).second->ReInit();
		}
	}
}

void TextureFileManager::UnloadAllTexture()
{
	kstl::map<kstl::string, CoreModifiable*>::iterator	it = myTextureMap.begin();
	while (it != myTextureMap.end())
	{
		CoreModifiable* t = (*it).second;

		myTextureMap.erase(it);
		//t->Destroy();
		it = myTextureMap.begin();
	}
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
			//Tex->Destroy();
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
			//Tex->Destroy();
			break;
		}
	}
}


void TextureFileManager::ClearCache()
{
	myTextureMap.clear();
}
