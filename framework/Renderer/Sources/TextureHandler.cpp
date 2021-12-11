

#include "TextureHandler.h"
#include "TextureFileManager.h"
#include "Core.h"

#include "MiniInstanceFactory.h"
#include "CoreBaseApplication.h"


IMPLEMENT_CLASS_INFO(TextureHandler)


TextureHandler::TextureHandler(const kstl::string& name, CLASS_NAME_TREE_ARG) : ParentClassType(name, PASS_CLASS_NAME_TREE_ARG)
{
	// we want TextureHandler to be alert on texture change as soon as possible
	mTextureName.changeNotificationLevel(Owner);
}

TextureHandler::~TextureHandler()
{

}

void	TextureHandler::InitModifiable()
{
	// avoid double init
	if (_isInit)
	{
		return;
	}
	ParentClassType::InitModifiable();
	if (_isInit)
	{
		if (((std::string)mTextureName) != "")
		{
			changeTexture();
		}
		if (mPushUVMatrix)
		{
			setUserFlag(pushUVMatrix);
		}
		mPushUVMatrix.changeNotificationLevel(Owner);
	}
}



bool	TextureHandler::initFromSpriteSheet(const std::string& jsonfilename)
{
	auto textureManager = KigsCore::Singleton<TextureFileManager>();
	auto spritesheetResource = textureManager->GetTextureFromSpriteSheetJSON(jsonfilename);

	if (mTexture && spritesheetResource)
	{
		if (mTexture->getValue<std::string>("TextureName") == spritesheetResource->mTextureFilename) // same texture, just returns
		{
			return false;
		}
	}

	// if texture was already init in texture manager, just get texture
	if (spritesheetResource && spritesheetResource->mTextureFilename != "")
	{
		// load texture
		mTexture = textureManager->GetTexture(spritesheetResource->mTextureFilename);
		return true;
	}

	// else create the upgrador 
	SpriteSheetData* newspritesheet = (SpriteSheetData*)KigsCore::Instance()->GetUpgradorFactory()->CreateClassInstance("SpriteSheetData");
	if (newspritesheet)
	{
		std::string getTextureName;
		if (newspritesheet->Init(jsonfilename, getTextureName))
		{
			mTexture = textureManager->GetTexture(getTextureName);

			if ((!newspritesheet->isOK()) || !mTexture)
			{
				delete newspritesheet;
				newspritesheet = nullptr;
				return true;
			}
			textureManager->InsertSpriteSheetJSONReference(jsonfilename, newspritesheet->getResource());

			mTexture->Upgrade(newspritesheet);
		}
	}
	return true;
}

bool	TextureHandler::initFromPicture(const std::string& picfilename)
{

	auto textureManager = KigsCore::Singleton<TextureFileManager>();
	SP<Texture> loaded=	textureManager->GetTexture(picfilename);

	if (loaded.get() == mTexture.get()) // nothing changed
	{
		return false;
	}
	setTexture(loaded);
	return true;
}



bool TextureHandler::changeTexture()
{
	std::string texname = mTextureName;
	bool hasChanged = false;
	// check texture type
	auto arr = SplitStringByCharacter(mTextureName, ':');
	if (arr.size() > 1) // use a sprite in a spritesheet 
	{
		hasChanged=initFromSpriteSheet(arr[0]);
		SpriteSheetData* currentspritesheet = mTexture->getSpriteSheetData();
		if (currentspritesheet)
		{
			const SpriteSheetFrameData* ssdata=currentspritesheet->getFrame(arr[1]);
			if (ssdata)
			{
				setCurrentFrame(ssdata);
			}
			else // is it an animation ?
			{
				const std::vector<SpriteSheetFrameData*>* ssanim = currentspritesheet->getAnimation(arr[1]);
				if (ssanim)
				{
					Upgrade("AnimationUpgrador");
					setCurrentFrame((*ssanim)[0]);
				}
			}

		}
		
	}
	else
	{
		if (texname.find(".json") != std::string::npos) // load a spritesheet
		{
			hasChanged = initFromSpriteSheet(texname);
			Upgrade("AnimationUpgrador");
		}
		else
		{
			hasChanged = initFromPicture(texname);
		}
	}

	if (mTexture)
	{
		refreshTextureInfos();
	
		if (!mTexture->IsInit())
		{
			KigsCore::Connect(mTexture.get(), "PostInit", this, "textureWasInit");
		}
		
		KigsCore::Connect(mTexture.get(), "NotifyUpdate", this, "TextureNotifyUpdate");
		
	}
	return true;
}

void	TextureHandler::textureWasInit()
{
	KigsCore::Disconnect(mTexture.get(), "PostInit", this, "textureWasInit");
	refreshTextureInfos();
}

void TextureHandler::NotifyUpdate(const unsigned int  labelid)
{
	if (labelid == mTextureName.getID())
	{
		changeTexture();
	}
	else if (labelid == mPushUVMatrix.getID())
	{
		changeUserFlag(pushUVMatrix, mPushUVMatrix);
	}
	ParentClassType::NotifyUpdate(labelid);
	
}

void TextureHandler::TextureNotifyUpdate(CoreModifiable* sender, const unsigned int  labelid )
{
	if (labelid == mTexture->mForceNearest.getID())
	{
		refreshSizeAndUVs(mCurrentFrame);
		NotifyUpdate(labelid); // launch notify update signal
	}
}


SP<Texture>	TextureHandler::GetEmptyTexture(const std::string& name)
{
	if (mTexture || (name == ""))
	{
		return mTexture;
	}

	mTexture = KigsCore::GetInstanceOf(name, "Texture");
	mTexture->Init();

	return mTexture;
}


void	TextureHandler::setCurrentFrame(const SpriteSheetFrameData* ssf)
{
	mCurrentFrame = ssf;

	refreshSizeAndUVs(ssf);

	// notify parent that something changed
	NotifyUpdate(0);
}


v2f	TextureHandler::getDrawablePos(const v2f& pos)
{
	if (mCurrentFrame)
	{
		if (mCurrentFrame->Trimmed)
		{
			v2f result(mCurrentFrame->Decal_X + pos.x * mCurrentFrame->FrameSize_X, mCurrentFrame->Decal_Y + pos.y * mCurrentFrame->FrameSize_Y);
			result*=v2f(1.0f / mCurrentFrame->SourceSize_X, 1.0f / mCurrentFrame->SourceSize_Y);
			return result;
		}
	}
	return pos;
}


void	TextureHandler::refreshSizeAndUVs(const SpriteSheetFrameData* ssf)
{
	bool perfectPix = mTexture->isUserFlagSet(Texture::hasNearestPixelSet);
	mUVTexture.SetIdentity();

	v2f uvSize;

	if (ssf)
	{
		mUVTexture.e[0][2] = (ssf->FramePos_X) * mOneOnPower2Size.x;
		mUVTexture.e[1][2] = (ssf->FramePos_Y) * mOneOnPower2Size.y;

		if (ssf->Rotated)
		{
			// 2d 90° rotation matrix
			mUVTexture.e[0][0] = 0.0f;
			mUVTexture.e[0][1] = -1.0f;
			mUVTexture.e[1][0] = 1.0f;
			mUVTexture.e[1][1] = 0.0f;
			mUVTexture.e[0][2] += ssf->FrameSize_Y * mOneOnPower2Size.x;
			uvSize.x = ssf->FrameSize_Y;
			uvSize.y = ssf->FrameSize_X;
		}
		else
		{
			uvSize.x = ssf->FrameSize_X;
			uvSize.y = ssf->FrameSize_Y;
		}
		

		mSize.x = ssf->SourceSize_X;
		mSize.y = ssf->SourceSize_Y;
	}
	else
	{
		mTexture->GetSize(mSize.x, mSize.y);
		uvSize = mSize;
	}

	if (!perfectPix)
	{
		if (ssf && ssf->Rotated)
		{
			mUVTexture.e[0][2] -= 0.5f * mOneOnPower2Size.x;
		}
		else
		{
			mUVTexture.e[0][2] += 0.5f * mOneOnPower2Size.x;
		}
		mUVTexture.e[1][2] += 0.5f * mOneOnPower2Size.y;
		uvSize.x -= 1.0f;
		uvSize.y -= 1.0f;
	}

	uvSize *= mOneOnPower2Size;
	
	mUVTexture.e[0][0] *= uvSize.x;
	mUVTexture.e[1][0] *= uvSize.y;
	mUVTexture.e[0][1] *= uvSize.x;
	mUVTexture.e[1][1] *= uvSize.y;
}

void	TextureHandler::refreshTextureInfos()
{
	unsigned int Pow2X, Pow2Y;
	mTexture->GetPow2Size(Pow2X, Pow2Y);
	mOneOnPower2Size.x = 1.0f / (float)Pow2X;
	mOneOnPower2Size.y = 1.0f / (float)Pow2Y;
	
	refreshSizeAndUVs(mCurrentFrame);
}



// connect to events and create attributes
void	AnimationUpgrador::Init(CoreModifiable* toUpgrade)
{
	// Connect notify update
	KigsCore::Connect(toUpgrade, "NotifyUpdate", toUpgrade, "AnimationNotifyUpdate");

	mCurrentAnimation = (maString*)toUpgrade->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::STRING, "CurrentAnimation", "");
	mCurrentAnimation->changeNotificationLevel(Owner);
	mFramePerSecond = (maUInt*)toUpgrade->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::UINT, "FramePerSecond", 24);
	mFramePerSecond->changeNotificationLevel(Owner);
	mLoop = (maBool*)toUpgrade->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::BOOL, "Loop", false);
	mLoop->changeNotificationLevel(Owner);

	// check if already in auto update mode
	if (toUpgrade->isFlagAsAutoUpdateRegistered())
		mWasdAutoUpdate = true;
	else
		KigsCore::GetCoreApplication()->AddAutoUpdate(toUpgrade);
	
}

//  remove dynamic attributes and disconnect events
void	AnimationUpgrador::Destroy(CoreModifiable* toDowngrade, bool toDowngradeDeleted)
{
	if (toDowngradeDeleted) return;

	KigsCore::Disconnect(toDowngrade, "NotifyUpdate", toDowngrade, "AnimationNotifyUpdate");

	toDowngrade->RemoveDynamicAttribute("CurrentAnimation");
	toDowngrade->RemoveDynamicAttribute("FramePerSecond");
	toDowngrade->RemoveDynamicAttribute("Loop");

	if (!mWasdAutoUpdate)
		KigsCore::GetCoreApplication()->RemoveAutoUpdate(toDowngrade);
}

DEFINE_UPGRADOR_METHOD(AnimationUpgrador, Play)
{
	if (!params.empty())
	{
		u32 labelID;
		params[1]->getValue(labelID);

		/*if (GetUpgrador()->mTarget->getLabelID() == labelID)
		{
			GetUpgrador()->mCurrentTarget = (Node3D*)(CoreModifiable*)(*(maReference*)(GetUpgrador()->mTarget));
		}*/

	}
	return false;
}

void	AnimationUpgrador::NotifyUpdate(const unsigned int  labelid, TextureHandler* parent)
{
	if (labelid == mCurrentAnimation->getLabelID())
	{
		mCurrentFrame = 0;
		mElpasedTime = 0.0;

		SpriteSheetData* currentspritesheet = parent->mTexture->getSpriteSheetData();
		const std::vector<SpriteSheetFrameData*>* ssanim = currentspritesheet->getAnimation(*mCurrentAnimation);
		if(ssanim)
		{
			mFrameNumber = (unsigned int) ssanim->size();
			parent->setCurrentFrame((*ssanim)[mCurrentFrame]);
		}
	}
}

DEFINE_UPGRADOR_METHOD(AnimationUpgrador, AnimationNotifyUpdate)
{
	if (!params.empty())
	{
		u32 labelID;
		params[1]->getValue(labelID);
		AnimationUpgrador* currentAnim = static_cast<AnimationUpgrador*>(GetUpgrador());
		currentAnim->NotifyUpdate(labelID,this);
	}
	return false;
}

void	AnimationUpgrador::Update(const Timer& _timer, TextureHandler* parent)
{
	double L_delta = ((Timer&)_timer).GetDt(parent);
	u32 prevCurrentFrame = mCurrentFrame;
	if (mFrameNumber > 0)
	{
		mElpasedTime += L_delta;
		auto animationSpeed = 1.0f / (float)((unsigned int)(*mFramePerSecond));
		if (mElpasedTime >= animationSpeed)
		{
			int L_temp = (int)(mElpasedTime / animationSpeed);
			mCurrentFrame += L_temp;
			if ((bool)(*mLoop))
			{
				mCurrentFrame = mCurrentFrame % mFrameNumber;
			}
			else if (mCurrentFrame >= mFrameNumber)
			{
				mCurrentFrame = mFrameNumber - 1;
			}
			mElpasedTime -= animationSpeed * ((float)(L_temp));
		}
	}
	else
	{
		mElpasedTime = 0;
	}
	if (prevCurrentFrame != mCurrentFrame)
	{
		SpriteSheetData* currentspritesheet = parent->mTexture->getSpriteSheetData();
		const std::vector<SpriteSheetFrameData*>* ssanim = currentspritesheet->getAnimation(*mCurrentAnimation);
		if (ssanim)
		{
			parent->setCurrentFrame((*ssanim)[mCurrentFrame]);
		}
	}
}

DEFINE_UPGRADOR_UPDATE(AnimationUpgrador)
{
	AnimationUpgrador* currentAnim=static_cast<AnimationUpgrador*>(GetUpgrador());
	currentAnim->Update(timer,this);
	return false;
}
