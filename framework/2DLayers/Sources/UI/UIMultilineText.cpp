#include "UI/UIMultilineText.h"
#include "TextureFileManager.h"
#include "Texture.h"
#include "AlphaMask.h"
#include "LocalizationManager.h"
#include "NotificationCenter.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIMultilineText, UIMultilineText, 2DLayers);
IMPLEMENT_CLASS_INFO(UIMultilineText)

UIMultilineText::UIMultilineText(const kstl::string& name, CLASS_NAME_TREE_ARG)
	:UIText(name, PASS_CLASS_NAME_TREE_ARG)
{
	myMaxSize = 128;
}
