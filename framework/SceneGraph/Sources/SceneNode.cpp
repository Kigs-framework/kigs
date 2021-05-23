#include "PrecompiledHeaders.h"

#include "SceneNode.h"
#include "ModuleRenderer.h"
#include "Core.h"
#include "Drawable.h"



IMPLEMENT_CLASS_INFO(SceneNode)

SceneNode::SceneNode(const kstl::string& name,CLASS_NAME_TREE_ARG) : CoreModifiable(name,PASS_CLASS_NAME_TREE_ARG)
{
}


bool SceneNode::IsInScene() const
{
	auto parent = this;
	while (parent)
	{
		if (parent->isSubType("Scene3D")) return true;
		parent = (SceneNode*)parent->getFirstParent("SceneNode");
	}
	return false;
}

bool SceneNode::IsVisibleInScene()
{
	auto parent = this;
	while (parent)
	{
		if (!parent->IsRenderable()) return false;
		parent = (SceneNode*)parent->getFirstParent("SceneNode");
	}
	return true;
}



bool	SceneNode::addItem(const CMSP& item,ItemPosition pos DECLARE_LINK_NAME )
{
	// update drawing needs flag if drawable
	if(item->isSubType(Drawable::mClassID))
	{
		item->as<Drawable>()->UpdateDrawingNeeds();
	}

	return CoreModifiable::addItem(item, pos PASS_LINK_NAME(linkName));
}

bool SceneNode::removeItem(const CMSP& item DECLARE_LINK_NAME)
{
	// update drawing needs flag if drawable
	if(item->isSubType(Drawable::mClassID))
	{
		item->as<Drawable>()->UpdateDrawingNeeds();
	}

	return CoreModifiable::removeItem(item PASS_LINK_NAME(linkName));
}