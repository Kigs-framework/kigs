#include "Box2DBindModule.h"
#include "Box2DBody.h"
#include "Box2DShape.h"
#include "Timer.h"

#include "box2d/b2_api.h"


IMPLEMENT_CLASS_INFO(Box2DBindModule);

IMPLEMENT_CONSTRUCTOR(Box2DBindModule) 
,mWorld(b2Vec2(((v2f)mGravity).x, ((v2f)mGravity).y))
{
	
}

//! module init, register CollisionManager and BSphere objects
void Box2DBindModule::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
    BaseInit(core,"Box2DBind",params);

    mGravity.changeNotificationLevel(Owner);

	DECLARE_FULL_CLASS_INFO(core, Box2DBody, Box2DBody, Box2DBind);
	DECLARE_FULL_CLASS_INFO(core, Box2DShape, Box2DShape, Box2DBind);
}

//! module close
void Box2DBindModule::Close()
{
    BaseClose();
	
}    

void Box2DBindModule::NotifyUpdate(const unsigned int labelID)
{
	if (labelID == mGravity.getID())
	{
		v2f	g = mGravity;
		mWorld.SetGravity(b2Vec2(g.x, g.y));
	}
	ModuleBase::NotifyUpdate(labelID);
}

void Box2DBindModule::Update(const Timer& timer, void* addParam)
{
	double t = timer.GetTime();
	double dt = t - mLastUpdateTime;
	mLastUpdateTime = t;
	if (dt > mMaxDT)
	{
		dt = mMaxDT;
	}
	mWorld.Step(dt, 8, 3);
}

