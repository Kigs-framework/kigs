#include "UI/UIPanel.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIPanel, UIPanel, 2DLayers);
IMPLEMENT_CLASS_INFO(UIPanel)

UIPanel::UIPanel(const kstl::string& name,CLASS_NAME_TREE_ARG) : 
UIDrawableItem(name, PASS_CLASS_NAME_TREE_ARG)
{}

bool UIPanel::isAlpha(float X, float Y)
{
	return false;
}

