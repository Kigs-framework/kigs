#include "PrecompiledHeaders.h"

#include "Texture.h"
#include "TextureFileManager.h"
#include "Core.h"

IMPLEMENT_CLASS_INFO(Texture)

Texture::Texture(const kstl::string& name, CLASS_NAME_TREE_ARG) : Drawable(name, PASS_CLASS_NAME_TREE_ARG)
,mWidth(*this, false, "Width", 0)
,mHeight(*this, false, "Height", 0)
,mTransparencyType(*this, false, "TransparencyType", -1)
,mHasMipmap(*this, false, "HasMipmap", false)
,mForceNearest(*this, false, "ForceNearest", false)
,mFileName(*this, false, "FileName", "")
,mTextureType(*this, false, "TextureType", TEXTURE_2D)
,mRepeat_U(*this, false, "Repeat_U", true)
,mRepeat_V(*this, false, "Repeat_V", true)
,mIsDynamic(*this, true, "IsDynamic", false)
,mForcePow2(*this, true, "ForcePow2", false)
,mPow2Width(1)
,mPow2Height(1)
,mPixelSize(0)
,mIsText(false)
{
	//mTraceRef = true;
}

Texture::~Texture()
{
	auto textureManager = KigsCore::Singleton<TextureFileManager>();

	textureManager->UnloadTexture(this);
}

// special case for FBO linked texture
void	Texture::InitForFBO()
{
	mPow2Width = mWidth;
	mPow2Height = mHeight;
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
		if (mFileName.const_ref() != "")
		{
			if (!mIsText && !Load()) // text can be load
			{
#ifdef _DEBUG		
				printf("%s not loaded !!!\n", mFileName.c_str());
#endif
				UninitModifiable();
			}

			auto textureManager = KigsCore::Singleton<TextureFileManager>();

			if (textureManager->HasTexture(mFileName.const_ref()))
			{
#ifdef _DEBUG		
				SP<Texture> registered=textureManager->GetTexture(mFileName.const_ref());
				if (registered != this)
				{
				//	KIGS_ERROR("texture with same name already in cache", 1);
					printf("texture with same name (%s) already in cache \n", mFileName.const_ref().c_str());
				}
#endif
			}
			else
			{
				textureManager->AddTexture(mFileName.const_ref(), this);
			}
			ComputeRatio();
		}
	}
}

// for some type of classes when we want don't want duplicated instances (textures, shaders...)
// return an already existing instance equivalent of this
CMSP	Texture::getSharedInstance()
{
	auto textureManager = KigsCore::Singleton<TextureFileManager>();

	if (textureManager->HasTexture(mFileName.const_ref()))
	{
		SP<Texture> shared = textureManager->GetTexture(mFileName.const_ref());
		
		return shared;
	}
	return  CMSP(this, GetRefTag{});
}

DEFINE_UPGRADOR_METHOD(SpriteSheetData, GetAnimationList)
{
	// TODO
	return false;
}

// do orientation
DEFINE_UPGRADOR_UPDATE(SpriteSheetData)
{

}

SpriteSheetData* Texture::getSpriteSheetData()
{
	return static_cast<SpriteSheetData*>(GetUpgrador("SpriteSheetData"));
}
