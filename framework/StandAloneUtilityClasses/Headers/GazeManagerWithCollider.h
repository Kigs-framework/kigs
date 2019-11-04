#ifndef _GazeManager_H_
#define _GazeManager_H_

#include "GazeManager.h"
#include "maReference.h"
#include "CollisionManager.h"

class OpenGLHoloUI2DLayer;
class Plane;

class	ColliderGazeManager : public GazeManager
{
public:
	DECLARE_CLASS_INFO(ColliderGazeManager, GazeManager, App)
	DECLARE_CONSTRUCTOR(ColliderGazeManager)

	void Update(const Timer&  timer, void* /*addParam*/) override;

	bool GetHit(Hit &hit, double dist = 0, u32 mask = (u32)-1) override;
	int GetHitsInRange(kstl::vector<Hit>& hits, double distBegin, double distend, u32 mask = (u32)-1) override;

	
protected:
	kstl::vector<Hit> myHitList;

	maReference myCollisionManager;
};
#endif