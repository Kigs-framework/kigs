#include "UI/UIScrollingMultilineText.h"
#include "TextureFileManager.h"
#include "Texture.h"
#include "AlphaMask.h"
#include "LocalizationManager.h"
#include "Timer.h"

using namespace Kigs::Draw2D;

IMPLEMENT_CLASS_INFO(UIScrollingMultilineText)

UIScrollingMultilineText::UIScrollingMultilineText(const std::string& name,CLASS_NAME_TREE_ARG) : 
	UIScrollingText(name, PASS_CLASS_NAME_TREE_ARG)
{
	mMaxWidth = 128;
}
