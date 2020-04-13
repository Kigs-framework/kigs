#include "PrecompiledHeaders.h"

#include "Texture.h"
#include "TextureFileManager.h"
#include "Core.h"

IMPLEMENT_CLASS_INFO(Texture)

Texture::Texture(const kstl::string& name, CLASS_NAME_TREE_ARG) : Drawable(name, PASS_CLASS_NAME_TREE_ARG)
,myWidth(*this, false, "Width", 0)
,myHeight(*this, false, "Height", 0)
,myTransparencyType(*this, false, "TransparencyType", -1)
,myHasMipmap(*this, false, "HasMipmap", false)
,myForceNearest(*this, false, "ForceNearest", false)
,myFileName(*this, false, "FileName", "")
,myTextureType(*this, false, "TextureType", TEXTURE_2D)
,myRepeatU(*this, false, "Repeat_U", true)
,myRepeatV(*this, false, "Repeat_V", true)
,myIsDynamic(*this, true, "IsDynamic", false)
,myForcePow2(*this, true, "ForcePow2", false)
,myPow2Width(1)
,myPow2Height(1)
,myPixelSize(0)
,myIsText(false)
{
	//myTraceRef = true;
}

Texture::~Texture()
{
	auto& textureManager = KigsCore::Singleton<TextureFileManager>();

	textureManager->UnloadTexture(this);
}

// special case for FBO linked texture
void	Texture::InitForFBO()
{
	myPow2Width = myWidth;
	myPow2Height = myHeight;
	Init();
	ComputeRatio();
}

void	Texture::InitModifiable()
{
	// avoid double init
	if (_isInit)
	{
		return;
	}
	Drawable::InitModifiable();
	if (_isInit)
	{
		if (myFileName.const_ref() != "")
		{
			if (!myIsText && !Load()) // text can be load
			{
#ifdef _DEBUG		
				printf("%s not loaded !!!\n", myFileName.c_str());
#endif
				UninitModifiable();
			}

			auto& textureManager = KigsCore::Singleton<TextureFileManager>();

			if (textureManager->HasTexture(myFileName.const_ref()))
			{
#ifdef _DEBUG		
				SP<Texture> registered=textureManager->GetTexture(myFileName.const_ref());
				if (registered != this)
				{
				//	KIGS_ERROR("texture with same name already in cache", 1);
					printf("texture with same name (%s) already in cache \n", myFileName.const_ref().c_str());
				}
#endif
			}
			else
			{
				textureManager->AddTexture(myFileName.const_ref(), this);
			}
			ComputeRatio();
		}
	}
}

// for some type of classes when we want don't want duplicated instances (textures, shaders...)
// return an already existing instance equivalent of this
CMSP	Texture::getSharedInstance()
{
	auto& textureManager = KigsCore::Singleton<TextureFileManager>();

	if (textureManager->HasTexture(myFileName.const_ref()))
	{
		SP<Texture> shared = textureManager->GetTexture(myFileName.const_ref());
		
		return shared;
	}
	return  CMSP(this, GetRefTag{});
}