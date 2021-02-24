#include "Box2DBody.h"
#include "Box2DBindModule.h"

IMPLEMENT_CLASS_INFO(Box2DBody)

IMPLEMENT_CONSTRUCTOR(Box2DBody)
{

}

Box2DBody::~Box2DBody()
{
	if (mBody)
	{
		Box2DBindModule* b2dmodule = (Box2DBindModule*)KigsCore::GetModule("Box2DBindModule");
		b2dmodule->GetWorld()->DestroyBody(mBody);
	}
	mBody = nullptr;
}

void Box2DBody::InitModifiable()
{
	if (_isInit) // already init ? just return
	{
		return;
	}
	
	Box2DBindModule* b2dmodule = (Box2DBindModule*)KigsCore::GetModule("Box2DBindModule");
	// init structure from CoreModifiable attributes
	b2BodyDef currentBodyDef;
	currentBodyDef.type =static_cast<b2BodyType>((int)mType);
	currentBodyDef.position = tob2Vec2(mPosition);
	currentBodyDef.angle = (float)mAngle;
	currentBodyDef.linearVelocity = tob2Vec2(mLinearVelocity);
	currentBodyDef.angularVelocity = (float)mAngularVelocity;
	currentBodyDef.linearDamping = (float)mLinearDamping;
	currentBodyDef.angularDamping = (float)mAngularDamping;
	currentBodyDef.allowSleep = (bool)mAllowSleep;
	currentBodyDef.awake = (bool)mAwake;
	currentBodyDef.fixedRotation = (bool)mFixedRotation;
	currentBodyDef.bullet = (bool)mBullet;
	currentBodyDef.enabled = (bool)mEnabled;
	currentBodyDef.gravityScale = (float)mGravityScale;
	currentBodyDef.userData.pointer =(uintptr_t) this;

	mBody = b2dmodule->GetWorld()->CreateBody(&currentBodyDef);


	mType.changeNotificationLevel(Owner);
	mPosition.changeNotificationLevel(Owner);
	mAngle.changeNotificationLevel(Owner);
	mLinearVelocity.changeNotificationLevel(Owner);
	mAngularVelocity.changeNotificationLevel(Owner);
	mLinearDamping.changeNotificationLevel(Owner);
	mAngularDamping.changeNotificationLevel(Owner);
	mAllowSleep.changeNotificationLevel(Owner);
	mAwake.changeNotificationLevel(Owner);
	mFixedRotation.changeNotificationLevel(Owner);
	mBullet.changeNotificationLevel(Owner);
	mEnabled.changeNotificationLevel(Owner);
	mGravityScale.changeNotificationLevel(Owner);

	Node2D::InitModifiable();
}

void Box2DBody::NotifyUpdate(const unsigned int labelID)
{

	if (labelID == mType.getID())
	{
		mBody->SetType(static_cast<b2BodyType>((int)mType));
	}
	else if (labelID == mPosition.getID())
	{
		mBody->SetTransform(tob2Vec2(mPosition), mBody->GetAngle());
	}
	else if (labelID == mAngle.getID())
	{
		mBody->SetTransform(mBody->GetPosition(), (float)mAngle);
	}
	else if (labelID == mLinearVelocity.getID())
	{
		mBody->SetLinearVelocity(tob2Vec2(mLinearVelocity));
	}
	else if (labelID == mAngularVelocity.getID())
	{
		mBody->SetAngularVelocity((float)mAngularVelocity);
	}
	else if (labelID == mLinearDamping.getID())
	{
		mBody->SetLinearDamping((float)mLinearDamping);
	}
	else if (labelID == mAngularDamping.getID())
	{
		mBody->SetAngularDamping((float)mAngularDamping);
	}
	else if (labelID == mAllowSleep.getID())
	{
		mBody->SetSleepingAllowed((bool)mAllowSleep);
	}
	else if (labelID == mAwake.getID())
	{
		mBody->SetAwake((bool)mAwake);
	}
	else if (labelID == mFixedRotation.getID())
	{
		mBody->SetFixedRotation((bool)mFixedRotation);
	}
	else if (labelID == mBullet.getID())
	{
		mBody->SetBullet((bool)mBullet);
	}
	else if (labelID == mEnabled.getID())
	{
		mBody->SetEnabled((bool)mEnabled);
	}
	else if (labelID == mGravityScale.getID())
	{
		mBody->SetGravityScale((float)mGravityScale);
	}
	
	Node2D::NotifyUpdate(labelID);
}

