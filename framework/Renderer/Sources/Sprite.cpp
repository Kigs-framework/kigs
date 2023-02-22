#include "PrecompiledHeaders.h"

#include "Sprite.h"
#include "Texture.h"
#include "TextureFileManager.h"
#include "Core.h"

using namespace Kigs::Draw;

IMPLEMENT_CLASS_INFO(Sprite)

Sprite::Sprite(const std::string& name,CLASS_NAME_TREE_ARG) : Drawable(name,PASS_CLASS_NAME_TREE_ARG)
{
	setInitParameter("Displacement", true);
}


Sprite::~Sprite()
{

}