#include "PrecompiledHeaders.h"

#include "Core.h"
#include "SkyBox.h"
#include "TextureFileManager.h"

IMPLEMENT_CLASS_INFO(SkyBox)

// the texture base name is used to find all the six textures
// by adding _# where # is 1 to 6
// 1 = XMAX = LEFT ?
// 2 = XMIN = RIGHT ?
// 3 = YMAX = FRONT ?
// 4 = YMIN = BACK ?
// 5 = ZMAX	= TOP
// 6 = ZMIN = BOTTOM

SkyBox::SkyBox(const kstl::string& name,CLASS_NAME_TREE_ARG) : Drawable(name,PASS_CLASS_NAME_TREE_ARG),
mSize(*this,false,LABEL_AND_ID(Size),KFLOAT_CONST(5000.0f)),
mFileName(*this,false,LABEL_AND_ID(FileName),"SkyBox.tga")
, mTexture(nullptr)
{
	
}

SkyBox::~SkyBox()
{

}

void SkyBox::InitModifiable()
{
	Drawable::InitModifiable();

	auto textureManager = KigsCore::Singleton<TextureFileManager>();

	mTexture = textureManager->GetTexture(mFileName, false);
	mTexture->setValue(LABEL_TO_ID(ForcePow2),true);
	mTexture->setValue(LABEL_TO_ID(TextureType),Texture::TEXTURE_CUBE_MAP);
	mTexture->Init();

	mBBoxMin.Set(KFLOAT_CONST_MIN, KFLOAT_CONST_MIN, KFLOAT_CONST_MIN); 
	mBBoxMax.Set(KFLOAT_CONST_MAX, KFLOAT_CONST_MAX, KFLOAT_CONST_MAX);
}
