#ifndef _MOUSEVELOCITYCOMPUTER_H_
#define _MOUSEVELOCITYCOMPUTER_H_

#include "CoreModifiable.h"

// ****************************************
// * MouseVelocityComputer class
// * --------------------------------------
/**
* \file	MouseVelocityComputer.h
* \class	MouseVelocityComputer
* \ingroup 2DLayers
* \brief	Keep track of past mouse mouvement to compute a velocity.
*
*  Used to give some inertia to mouse ( or touch ) dragged objects.
*/
// ****************************************
class MouseVelocityComputer : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(MouseVelocityComputer, CoreModifiable, 2DLayers);

	MouseVelocityComputer(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	using CoreModifiable::Init;
	void Init(kdouble time);
	void StoreDisplacement(kfloat dx, kfloat dy, kdouble time);
	void ComputeVelocity(kfloat &vx, kfloat &vy);

protected:
	~MouseVelocityComputer();

private:
	kfloat	*mVelocityX;
	kfloat	*mVelocityY;
	int		mVelocityIndex;
	int		mVelocityCount;
	kdouble mOldTime;
	const static int cMaxIndex = 5;
};

#endif