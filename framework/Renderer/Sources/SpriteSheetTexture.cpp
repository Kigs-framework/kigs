#include "PrecompiledHeaders.h"
#include "SpriteSheetTexture.h"
#include "JSonFileParser.h"
#include "CoreValue.h"
#include "Texture.h"
#include "TextureFileManager.h"

IMPLEMENT_CLASS_INFO(SpriteSheetTexture);

SpriteSheetTexture::~SpriteSheetTexture()
{
	auto& textureManager = KigsCore::Singleton<TextureFileManager>();
	textureManager->UnloadTexture(this);
}

//-----------------------------------------------------------------------------------------------------
//InitModifiable

void SpriteSheetTexture::InitModifiable()
{
	// avoid double init
	if (IsInit())
	{
		return;
	}
	ParentClassType::InitModifiable();
	if (IsInit())
	{
		if (mFileName.const_ref() != "")
		{
			//Load JsonFile
			JSonFileParser L_JsonParser;
			CoreItemSP L_Dictionary = L_JsonParser.Get_JsonDictionary(mFileName);
			if (!L_Dictionary.isNil())
			{
				CoreItemSP	L_vFrames = L_Dictionary["frames"];

				SortAnimation(L_vFrames);

				CoreItemSP L_Meta = L_Dictionary["meta"];

				if (!L_Meta.isNil())
				{
					std::string L_TextureName;
					auto L_Value = L_Meta["image"];
					L_Value->getValue(L_TextureName);

					// load texture
					auto& textureManager = KigsCore::Singleton<TextureFileManager>();
					mTexture = textureManager->GetTexture(L_TextureName);
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------------------------------
//Get_Frame

const SpriteSheetFrame*	SpriteSheetTexture::Get_Frame(const std::string& _value)
{
	auto found = mAllFrameList.find(_value);
	if (found != mAllFrameList.end())
	{
		return found->second.get();
	}

	return nullptr;
}


//-----------------------------------------------------------------------------------------------------
//Get_AnimFrame

const SpriteSheetFrame*	SpriteSheetTexture::Get_AnimFrame(const std::string& _value, unsigned int _frameNumber)
{
	auto found = mAnimationList.find(_value);
	if (found != mAnimationList.end())
	{
		return found->second.at(_frameNumber);
	}

	return nullptr;
}

//-----------------------------------------------------------------------------------------------------
//Get_SourceSize

void SpriteSheetTexture::Get_SourceSize(const std::string& _Animation, int& Ssx, int &Ssy)
{
	if (mAnimationList.empty()) return;
	
	auto it = mAnimationList.find(_Animation);
	if (it == mAnimationList.end()) it = mAnimationList.begin();

	Ssx = it->second.at(0)->SourceSize_X;
	Ssy = it->second.at(0)->SourceSize_Y;
}

//-----------------------------------------------------------------------------------------------------
//Get_FrameNumber

unsigned int SpriteSheetTexture::Get_FrameNumber(const std::string& _Animation)
{
	if (mAnimationList.empty()) return 0;
	auto it = mAnimationList.find(_Animation);
	if (it == mAnimationList.end()) it = mAnimationList.begin();
	return it->second.size();
}

//-----------------------------------------------------------------------------------------------------
//Get_AnimationList

std::set<std::string> SpriteSheetTexture::Get_AnimationList()
{
	std::set<std::string> result;
	for (auto& kv : mAnimationList)
	{
		result.insert(kv.first);
	}
	return result;
}

//-----------------------------------------------------------------------------------------------------
//SortAnimation

void SpriteSheetTexture::SortAnimation(CoreItemSP& _FrameVector)
{
	std::vector<std::string>  str;
	std::string AnimeName;
	std::string CurrentName;

	for (auto it : _FrameVector)
	{
		CoreItemSP L_Frame = it["filename"];
		CurrentName = L_Frame->toString();

		std::vector<std::string>  str = SplitStringByCharacter(CurrentName, '/');
		
		//create Frame
		auto L_FrameInfo = std::make_unique<SpriteSheetFrame>();
		
		{
			CoreItemSP L_map = it["frame"];

			if (L_map->size())
			{
				L_FrameInfo->FramePos_X = L_map["x"];
				L_FrameInfo->FramePos_Y = L_map["y"];

				L_FrameInfo->FrameSize_X = L_map["w"];
				L_FrameInfo->FrameSize_Y = L_map["h"];
			}
		}

		{
			CoreItemSP L_map = it["sourceSize"];

			if (L_map->size())
			{
				L_FrameInfo->SourceSize_X = L_map["w"];
				L_FrameInfo->SourceSize_Y = L_map["h"];
			}
		}

		{
			CoreItemSP L_map = it["spriteSourceSize"];

			if (L_map->size())
			{
				L_FrameInfo->Decal_X = L_map["x"];
				L_FrameInfo->Decal_Y = L_map["y"];
			}
		}

		{
			L_FrameInfo->Rotated = it["rotated"];
		}

		{
			L_FrameInfo->Trimmed = it["trimmed"];
		}

		// add in anim list if needed
		if (str.size() == 2)
		{
			AnimeName = str[0];
			auto& FrameVector = mAnimationList[AnimeName];
			FrameVector.push_back(L_FrameInfo.get());
		}
		mAllFrameList[CurrentName] = std::move(L_FrameInfo);
	}
}

// for some type of classes when we want don't want duplicated instances (textures, shaders...)
// return an already existing instance equivalent of this
CMSP	SpriteSheetTexture::getSharedInstance()
{
	auto& textureManager = KigsCore::Singleton<TextureFileManager>();

	if (textureManager->HasTexture(mFileName.const_ref()))
	{
		return textureManager->GetSpriteSheetTexture(mFileName.const_ref());
	}
	return CMSP(this, GetRefTag{});
}