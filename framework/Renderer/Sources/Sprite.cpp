#include "PrecompiledHeaders.h"

#include "Sprite.h"
#include "Texture.h"
#include "TextureFileManager.h"
#include "Core.h"

IMPLEMENT_CLASS_INFO(Sprite)

Sprite::Sprite(const kstl::string& name,CLASS_NAME_TREE_ARG) : Drawable(name,PASS_CLASS_NAME_TREE_ARG),
mDisplacement(*this,true,LABEL_AND_ID(Displacement),KIGS_SPRITE_DEFAULT_DISPLACEMENT),
mPosition(*this,false,LABEL_AND_ID(Position),-0.5f,-0.5f),
mSize(*this,false,LABEL_AND_ID(Size),1.0f,1.0f),
mTexUV(*this,false,LABEL_AND_ID(TexUV),0.0f,0.0f,1.0f,1.0f),
mColor(*this,false,LABEL_AND_ID(Color),KFLOAT_CONST(1.f),KFLOAT_CONST(1.f),KFLOAT_CONST(1.f),KFLOAT_CONST(1.f))
{

}


Sprite::~Sprite()
{

}