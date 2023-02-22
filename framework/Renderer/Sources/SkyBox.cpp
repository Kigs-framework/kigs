#include "PrecompiledHeaders.h"

#include "Core.h"
#include "SkyBox.h"
#include "TextureFileManager.h"

using namespace Kigs::Draw;

IMPLEMENT_CLASS_INFO(SkyBox)

// the texture base name is used to find all the six textures
// by adding _# where # is 1 to 6
// 1 = XMAX = LEFT ?
// 2 = XMIN = RIGHT ?
// 3 = YMAX = FRONT ?
// 4 = YMIN = BACK ?
// 5 = ZMAX	= TOP
// 6 = ZMIN = BOTTOM

SkyBox::SkyBox(const std::string& name,CLASS_NAME_TREE_ARG) : Drawable(name,PASS_CLASS_NAME_TREE_ARG)
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
	mTexture->setValue("ForcePow2",true);
	mTexture->setValue("TextureType",Texture::TEXTURE_CUBE_MAP);
	mTexture->Init();

	mBBoxMin.Set((-FLT_MAX), (-FLT_MAX), (-FLT_MAX)); 
	mBBoxMax.Set(FLT_MAX, FLT_MAX, FLT_MAX);
}
