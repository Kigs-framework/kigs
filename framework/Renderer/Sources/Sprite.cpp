#include "PrecompiledHeaders.h"

#include "Sprite.h"
#include "Texture.h"
#include "TextureFileManager.h"
#include "Core.h"

IMPLEMENT_CLASS_INFO(Sprite)

Sprite::Sprite(const kstl::string& name,CLASS_NAME_TREE_ARG) : Drawable(name,PASS_CLASS_NAME_TREE_ARG),
mTextureFileName(*this,true,LABEL_AND_ID(TextureFileName),""),
mDisplacement(*this,true,LABEL_AND_ID(Displacement),KIGS_SPRITE_DEFAULT_DISPLACEMENT),
mPosX(*this,false,LABEL_AND_ID(PosX),KFLOAT_CONST(-0.5f)),
mPosY(*this,false,LABEL_AND_ID(PosY),KFLOAT_CONST(-0.5f)),
mSizeX(*this,false,LABEL_AND_ID(SizeX),KFLOAT_CONST(1.0f)),
mSizeY(*this,false,LABEL_AND_ID(SizeY),KFLOAT_CONST(1.0f)),
mTexu1(*this,false,LABEL_AND_ID(Texu1),KFLOAT_CONST(0.0f)),
mTexu2(*this,false,LABEL_AND_ID(Texu2),KFLOAT_CONST(1.0f)),
mTexv1(*this,false,LABEL_AND_ID(Texv1),KFLOAT_CONST(0.0f)),
mTexv2(*this,false,LABEL_AND_ID(Texv2),KFLOAT_CONST(1.0f)),
mColor(*this,false,LABEL_AND_ID(Color),KFLOAT_CONST(1.f),KFLOAT_CONST(1.f),KFLOAT_CONST(1.f),KFLOAT_CONST(1.f))
, mTexture(nullptr)
{

}

void Sprite::InitModifiable()
{

	mTexture = nullptr;

	changeTexture(mTextureFileName);
	Drawable::InitModifiable();
}

Sprite::~Sprite()
{

}

void Sprite::changeTexture(kstl::string FileName)
{
	auto& textureManager = KigsCore::Singleton<TextureFileManager>();

	mTextureFileName = FileName;
	mTexture = textureManager->GetTexture(mTextureFileName);
	
	if (mTexture)
	{
		mTexture->SetRepeatUV(false, false); //because of bilinear filtering on the borders...
		if (!mTexture->IsInit()) 
			mTexture->Init();
	}
}

