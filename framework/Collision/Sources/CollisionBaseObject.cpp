#include <CollisionBaseObject.h>
#include <CoreModifiable.h>

CollisionBaseObject::CollisionBaseObject()
{
#ifdef KIGS_TOOLS
	debugColor.Set(0, 255 * (float)rand() / (float)RAND_MAX, 255 * (float)rand() / (float)RAND_MAX);
#endif
};