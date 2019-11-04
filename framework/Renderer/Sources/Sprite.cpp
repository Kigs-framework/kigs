#include "PrecompiledHeaders.h"

#include "Sprite.h"
#include "Texture.h"
#include "TextureFileManager.h"
#include "Core.h"

IMPLEMENT_CLASS_INFO(Sprite)

Sprite::Sprite(const kstl::string& name,CLASS_NAME_TREE_ARG) : Drawable(name,PASS_CLASS_NAME_TREE_ARG),
m_TextureFileName(*this,true,LABEL_AND_ID(TextureFileName),""),
m_Displacement(*this,true,LABEL_AND_ID(Displacement),KIGS_SPRITE_DEFAULT_DISPLACEMENT),
m_PosX(*this,false,LABEL_AND_ID(PosX),KFLOAT_CONST(-0.5f)),
m_PosY(*this,false,LABEL_AND_ID(PosY),KFLOAT_CONST(-0.5f)),
m_SizeX(*this,false,LABEL_AND_ID(SizeX),KFLOAT_CONST(1.0f)),
m_SizeY(*this,false,LABEL_AND_ID(SizeY),KFLOAT_CONST(1.0f)),
m_Texu1(*this,false,LABEL_AND_ID(Texu1),KFLOAT_CONST(0.0f)),
m_Texu2(*this,false,LABEL_AND_ID(Texu2),KFLOAT_CONST(1.0f)),
m_Texv1(*this,false,LABEL_AND_ID(Texv1),KFLOAT_CONST(0.0f)),
m_Texv2(*this,false,LABEL_AND_ID(Texv2),KFLOAT_CONST(1.0f)),
m_Color(*this,false,LABEL_AND_ID(Color),KFLOAT_CONST(1.f),KFLOAT_CONST(1.f),KFLOAT_CONST(1.f),KFLOAT_CONST(1.f))
{
	myTexture=0;
//	m_FlagMask=FLAG_NONE;
}

void Sprite::InitModifiable()
{
	//TextureFileManager*	fileManager=(TextureFileManager*)KigsCore::GetSingleton("TextureFileManager");
	myTexture = NULL;

	changeTexture(m_TextureFileName);
	Drawable::InitModifiable();
}

Sprite::~Sprite()
{
	if(myTexture)
	{
		myTexture->Destroy();
	}
}

void Sprite::changeTexture(kstl::string FileName)
{
	TextureFileManager*	fileManager=(TextureFileManager*)KigsCore::GetSingleton("TextureFileManager");
	if(myTexture)
	{
		myTexture->Destroy();
	}
	m_TextureFileName = FileName;
	myTexture=fileManager->GetTexture(m_TextureFileName);
	
	if (myTexture)
	{
		myTexture->SetRepeatUV(false, false); //because of bilinear filtering on the borders...
		if (!myTexture->IsInit()) 
			myTexture->Init();
		myTexture->GetRef();
	}
}

