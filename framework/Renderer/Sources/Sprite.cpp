#include "PrecompiledHeaders.h"

#include "Sprite.h"
#include "Texture.h"
#include "TextureFileManager.h"
#include "Core.h"

using namespace Kigs::Draw;

IMPLEMENT_CLASS_INFO(Sprite)

Sprite::Sprite(const std::string& name,CLASS_NAME_TREE_ARG) : Drawable(name,PASS_CLASS_NAME_TREE_ARG),
mDisplacement(*this,true,"Displacement",KIGS_SPRITE_DEFAULT_DISPLACEMENT),
mPosition(*this,false,"Position",-0.5f,-0.5f),
mSize(*this,false,"Size",1.0f,1.0f),
mTexUV(*this,false,"TexUV",0.0f,0.0f,1.0f,1.0f),
mColor(*this,false,"Color",1.0f,1.0f,1.0f,1.0f)
{

}


Sprite::~Sprite()
{

}