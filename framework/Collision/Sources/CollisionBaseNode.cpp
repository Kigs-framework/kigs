#include <CollisionBaseNode.h>
#include <CoreModifiable.h>

IMPLEMENT_CLASS_INFO(CollisionBaseNode)

IMPLEMENT_CONSTRUCTOR(CollisionBaseNode)
, CollisionBaseObject()
, mHit(*this, false, "HasHit", false)
{
	mIsCoreModifiable = true;
};

void CollisionBaseNode::InitModifiable()
{
	Drawable::InitModifiable();
	mBoundingBox.SetEmpty();
}

