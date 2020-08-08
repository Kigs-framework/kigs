#include <CollisionBaseNode.h>
#include <CoreModifiable.h>

IMPLEMENT_CLASS_INFO(CollisionBaseNode)

IMPLEMENT_CONSTRUCTOR(CollisionBaseNode)
, CollisionBaseObject()
, myHit(*this, false, "HasHit", false)
{
	IsCoreModifiable = true;
};

void CollisionBaseNode::InitModifiable()
{
	Drawable::InitModifiable();
	myBoundingBox.SetEmpty();
}

