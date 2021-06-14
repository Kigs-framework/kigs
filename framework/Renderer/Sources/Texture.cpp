#include "PrecompiledHeaders.h"

#include "Texture.h"
#include "TextureFileManager.h"
#include "JSonFileParser.h"
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

void Texture::NotifyUpdate(const unsigned int  labelid )
{
	if (labelid == mForceNearest.getLabelID())
	{
		changeUserFlag(hasNearestPixelSet, mForceNearest);
	}
	Drawable::NotifyUpdate(labelid); // always call parent so that notifyupdate signal is called
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
				if (registered.get() != this)
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

		changeUserFlag(hasNearestPixelSet, mForceNearest);
		mForceNearest.changeNotificationLevel(Owner);
	}
}

// for some type of classes when we want don't want duplicated instances (textures, shaders...)
// return an already existing instance equivalent of this
CMSP Texture::getSharedInstance()
{
	auto textureManager = KigsCore::Singleton<TextureFileManager>();

	if (textureManager->HasTexture(mFileName.const_ref()))
	{
		SP<Texture> shared = textureManager->GetTexture(mFileName.const_ref());
		
		return shared;
	}
	return SharedFromThis();
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

void SpriteSheetData::sortAnimation(CoreItemSP& _FrameVector)
{
	std::string AnimeName;
	std::string CurrentName;

	std::map<std::string, std::map<std::string, SpriteSheetFrameData*>> currentAnimList;

	for (auto it : _FrameVector)
	{
		CoreItemSP L_Frame = it["filename"];
		CurrentName = L_Frame->toString();

		std::vector<std::string>  str = SplitStringByCharacter(CurrentName, '/');

		//create Frame
		auto L_FrameInfo = std::make_unique<SpriteSheetFrameData>();

		{
			CoreItemSP L_map = it["frame"];

			if (L_map->size())
			{
				L_FrameInfo->FramePos_X = *L_map["x"];
				L_FrameInfo->FramePos_Y = *L_map["y"];

				L_FrameInfo->FrameSize_X = *L_map["w"];
				L_FrameInfo->FrameSize_Y = *L_map["h"];
			}
		}

		{
			CoreItemSP L_map = it["sourceSize"];

			if (L_map->size())
			{
				L_FrameInfo->SourceSize_X = *L_map["w"];
				L_FrameInfo->SourceSize_Y = *L_map["h"];
			}
		}

		{
			CoreItemSP L_map = it["spriteSourceSize"];

			if (L_map->size())
			{
				L_FrameInfo->Decal_X = *L_map["x"];
				L_FrameInfo->Decal_Y = *L_map["y"];
			}
		}

		{
			L_FrameInfo->Rotated = *it["rotated"];
		}

		{
			L_FrameInfo->Trimmed = *it["trimmed"];
		}

		// add in anim list if needed
		if (str.size() == 2)
		{
			AnimeName = str[0];
			std::string framename = str[1];
			auto& FrameList = currentAnimList[AnimeName];
			FrameList[framename] = L_FrameInfo.get();
		}
		mSpriteResources->mAllFrameList[CurrentName] = std::move(L_FrameInfo);
	}

	for (auto& anim : currentAnimList)
	{
		auto& FrameVector = mSpriteResources->mAnimationList[anim.first];
		for (auto& f : anim.second)
		{
			FrameVector.push_back(f.second);
		}
	}

}

bool	SpriteSheetData::Init(const std::string& json, std::string& texturename)
{
	JSonFileParser L_JsonParser;
	CoreItemSP L_Dictionary = L_JsonParser.Get_JsonDictionary(json);
	if (L_Dictionary)
	{
		mSpriteResources = MakeRefCounted<SpriteSheetDataResource>();
		CoreItemSP	L_vFrames = L_Dictionary["frames"];

		sortAnimation(L_vFrames);

		CoreItemSP L_Meta = L_Dictionary["meta"];

		if (L_Meta)
		{
			texturename = (std::string)L_Meta["image"];
		}
		mJSonFilename = json;
		mSpriteResources->mTextureFilename = texturename;
		
		return true;
	}
	return false;
}

//  remove dynamic attributes and disconnect events
void	SpriteSheetData::Destroy(CoreModifiable* toDowngrade, bool toDowngradeDeleted)
{

}
