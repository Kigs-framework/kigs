#ifndef _COORDINATESYSTEM_H_
#define _COORDINATESYSTEM_H_

#include "Node3D.h"
#include "Upgrador.h"

// upgrade Node3D with rotation / position / scale accessors 
class CoordinateSystemUp : public Upgrador<Node3D>
{
	// create and init Upgrador if needed and add dynamic attributes
	virtual void	Init(CoreModifiable* toUpgrade) override;

	// destroy UpgradorData and remove dynamic attributes 
	virtual void	Destroy(CoreModifiable* toDowngrade) override;

	START_UPGRADOR(CoordinateSystemUp);
	UPGRADOR_METHODS(CoordinateSystemNotifyUpdate,AngAxisRotate,globalMoveNode,localMoveNode);

	static void toEuler(float x, float y, float z, float angle, float& heading, float& attitude, float& bank);

	CoreModifiableAttribute* mScale;
	CoreModifiableAttribute* mRot;
	CoreModifiableAttribute* mPos;

	bool					mWasChanged=false;
	bool					mWasdAutoUpdate = false;
}; 

// upgrade Node3D with "pivot" features : 
// - rotation axis defined with a 3D point and a 3D vector
// - an rotation angle
class PivotUp : public Upgrador<Node3D>
{
	// create and init Upgrador if needed and add dynamic attributes
	virtual void	Init(CoreModifiable* toUpgrade) override;

	// destroy UpgradorData and remove dynamic attributes 
	virtual void	Destroy(CoreModifiable* toDowngrade) override;

	START_UPGRADOR(PivotUp);
	UPGRADOR_METHODS(PivotNotifyUpdate);

	CoreModifiableAttribute* mPivotPosition;
	CoreModifiableAttribute* mPivotAxis;
	CoreModifiableAttribute* mAngle;
	CoreModifiableAttribute* mIsGlobal;

	Matrix3x4				mInitMatrix;

	bool					mWasChanged = false;
	bool					mWasdAutoUpdate = false;
};

#endif //_COORDINATESYSTEM_H_
