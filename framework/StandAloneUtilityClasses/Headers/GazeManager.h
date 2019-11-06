#ifndef __GAZEMANAGER_H__
#define __GAZEMANAGER_H__

#include "maReference.h"

#include "Intersection.h"

class Node3D;
class Camera;
class CollisionManager;

/*
struct GazeHit
{
	double					HitDistance = 1000;
	Point3D					HitPosition;
	Vector3D				HitNormal;
	CoreModifiable *		HitActor;
	Node3D *				HitNode;
	unsigned int			HitFlag;

	void Clear()
	{
		HitDistance = 1000;
		HitPosition.Set(0.0f, 0.0f, 0.0f);
		HitNormal.Set(0.0f, 0.0f, 0.0f);
		HitActor = nullptr;
		HitNode = nullptr;
	}
};
*/
extern bool gIsVR;
class GazeManager : public CoreModifiable
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(GazeManager, CoreModifiable, StandAlone);
	DECLARE_INLINE_CONSTRUCTOR(GazeManager)
	, myGazeCamera(*this, true, "GazeCamera")
	, myGazeFeedback(*this, true, "GazeFeedback")
	{
		if (gIsVR)
			mUseMotionControllerAsGaze = true;
	}

	virtual void GetGazeRay(Point3D& start, Vector3D& dir, Vector3D& up);
	virtual bool GetHit(Hit &hit, double dist = 0, u32 mask = (u32)-1) = 0;
	virtual int GetHitsInRange(kstl::vector<Hit> &hits, double distBegin, double distend, u32 mask = (u32)-1) = 0;

	void SetShowCursor(bool enabled);

protected:
	maReference myGazeCamera;
	maReference myGazeFeedback;

	Hit myHit;

	maBool myIsRecursive=BASE_ATTRIBUTE(Recursive,false);
	maBool mFixedNormal = BASE_ATTRIBUTE(FixedNormal, false);

	maBool mUseMotionControllerAsGaze = BASE_ATTRIBUTE(UseMotionControllerAsGaze, false);
};
#endif
