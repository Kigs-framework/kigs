#pragma once
#include "Node2D.h"
#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "maEnum.h"
#include "TecLibs/Tec3D.h"

#include <box2D/b2_body.h>

// ****************************************
// * Box2DBody class
// * --------------------------------------
/**
* \file	Box2DBody.h
* \class	Box2DBody
* \ingroup  Box2DBind
* \brief	Wrap a box2D body (<=> rigid body).
*
*/
// ****************************************

class Box2DBody : public Node2D
{
public:
	DECLARE_CLASS_INFO(Box2DBody, Node2D, Box2DBind)
	DECLARE_CONSTRUCTOR(Box2DBody);

protected:

	void InitModifiable() override;

	void NotifyUpdate(const unsigned int labelID) override;

	virtual ~Box2DBody();

	b2Body* mBody=nullptr;

	maEnum<3>	mType = BASE_ATTRIBUTE(Type, "Static", "Kinematic", "Dynamic");
	maVect2DF	mPosition = BASE_ATTRIBUTE(Position, 0.0f, 0.0f);
	maFloat		mAngle = BASE_ATTRIBUTE(Angle, 0.0f);
	maVect2DF	mLinearVelocity = BASE_ATTRIBUTE(LinearVelocity, 0.0f, 0.0f);
	maFloat		mAngularVelocity = BASE_ATTRIBUTE(AngularVelocity, 0.0f);
	maFloat 	mLinearDamping = BASE_ATTRIBUTE(LinearDamping, 0.0f);
	maFloat 	mAngularDamping = BASE_ATTRIBUTE(AngularDamping, 0.0f);
	maBool 		mAllowSleep = BASE_ATTRIBUTE(AllowSleep, true);
	maBool 		mAwake = BASE_ATTRIBUTE(AllowSleep, true);
	maBool 		mFixedRotation = BASE_ATTRIBUTE(FixedRotation, false);
	maBool 		mBullet = BASE_ATTRIBUTE(Bullet, false);
	maBool 		mEnabled = BASE_ATTRIBUTE(Enabled, true);
	maFloat 	mGravityScale = BASE_ATTRIBUTE(GravityScale, 1.0f);
};