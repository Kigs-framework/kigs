#include "CollisionBaseObject.h"
#include "CoreModifiable.h"

using namespace Kigs::Collide;

CollisionBaseObject::CollisionBaseObject()
{
#ifdef KIGS_TOOLS
	mDebugColor.Set(0, 255 * (float)rand() / (float)RAND_MAX, 255 * (float)rand() / (float)RAND_MAX);
#endif
};
