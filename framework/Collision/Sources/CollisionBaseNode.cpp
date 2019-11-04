#include <CollisionBaseNode.h>
#include <CoreModifiable.h>

IMPLEMENT_CLASS_INFO(CollisionBaseNode)

IMPLEMENT_CONSTRUCTOR(CollisionBaseNode)
, CollisionBaseObject()
, myHit(*this, false, "HasHit", false)
, refToLinkedItem(*this, true, "LinkedItem")
{
	IsCoreModifiable = true;
};

void CollisionBaseNode::InitModifiable()
{
	Drawable::InitModifiable();

	myBoundingBox.SetEmpty();

	LinkedItem = (CoreModifiable*)refToLinkedItem;
}

CoreModifiable* CollisionBaseNode::GetLinkedItem()
{
	if (LinkedItem)
		return LinkedItem;

	LinkedItem = (CoreModifiable*)refToLinkedItem;
	return LinkedItem;
}