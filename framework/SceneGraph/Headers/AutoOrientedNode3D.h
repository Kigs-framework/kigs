#ifndef _AUTOORIENTEDNODE3D_H_
#define _AUTOORIENTEDNODE3D_H_

#include "Node3D.h"
#include "Upgrador.h"

// ****************************************
// * AutoOrientedNode3DUp class
// * --------------------------------------
/**
 * \file	AutoOrientedNode3D.h
 * \class	AutoOrientedNode3DUp
 * \ingroup SceneGraph
 * \brief	Node3D updragor to auto orient node.
 */
 // ****************************************

class AutoOrientedNode3DUp : public Upgrador<Node3D>
{
protected:
	// create and init Upgrador if needed and add dynamic attributes
	virtual void	Init(CoreModifiable* toUpgrade) override;

	// destroy UpgradorData and remove dynamic attributes 
	virtual void	Destroy(CoreModifiable* toDowngrade) override;

	START_UPGRADOR(AutoOrientedNode3DUp);
	UPGRADOR_METHODS(AutoOrientedNotifyUpdate);

	// reference Node3D 
	CoreModifiableAttribute* mTarget;
	// axis pointing to target given as signed unit vector. ie : {1,0,0} or {-1,0,0} or {0,1,0}...
	CoreModifiableAttribute* mOrientedAxis;
	// define the pseudo constant axis (up vector) given as signed unit vector. ie : {1,0,0} or {-1,0,0} or {0,1,0}...
	// abs(PseudoConstantAxis) must be different of abs( OrientedAxis )
	CoreModifiableAttribute* mPseudoConstantAxis;
	// the pseudo constant axis (up) vector global coordinate
	CoreModifiableAttribute* mPseudoConstantAxisDir;

	bool					mWasdAutoUpdate = false;

	Node3D*					mCurrentTarget;
	Vector3D				mLastValidUpAxis;
}; 

#endif //_AUTOORIENTEDNODE3D_H_
