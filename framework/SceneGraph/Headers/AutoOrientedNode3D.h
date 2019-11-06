#ifndef _AUTOORIENTEDNODE3D_H_
#define _AUTOORIENTEDNODE3D_H_

#include "Node3D.h"

// ****************************************
// * AutoOrientedNode3D class
// * --------------------------------------
/**
 * \file	AutoOrientedNode3D.h
 * \class	AutoOrientedNode3D
 * \ingroup SceneGraph
 * \brief	.
 * \author	ukn
 * \version ukn
 * \date	ukn
 *
 * Exported parameters :<br>
 * <ul>
 * <li>
 *		string <strong>TargetName</strong> : target name used to search target in scenegraph
 * </li>
 * <li>
 *		kfloat <strong>PositionX</strong> : position  x
 * </li>
 * <li>
 *		kfloat <strong>PositionY</strong> : position  y
 * </li>
 * <li>
 *		kfloat <strong>PositionZ</strong> : position  z
 * </li>
 * <li>
 *		kfloat <strong>OrientedAxis[3]</strong> : define the axis pointing to the target : X,Y or Z
 * </li>
 * <li>
 *		kfloat <strong>PseudoConstantAxis[3]</strong> : define the pseudo constant axis (probably up vector) : X,Y or Z
 * </li>
 * <li>
 *		kfloat <strong>PseudoConstantAxisDir[3]</strong> : the pseudo constant axis vector coordinate
 * </li>
 * </ul>
 */
// ****************************************
class AutoOrientedNode3D : public Node3D
{
public:
	DECLARE_CLASS_INFO(AutoOrientedNode3D,Node3D,SceneGraph)
	DECLARE_CONSTRUCTOR(AutoOrientedNode3D);
	
	void setPosition(kfloat posX,kfloat posY,kfloat posZ);
	void getPosition(kfloat& posX,kfloat& posY,kfloat& posZ) const;

protected:
	/**
	 * \brief	overload Node3D TravCull, call DoOrientation before classic Node3D::TravCull
	 * \fn 		virtual void	TravCull(TravState* state);
	 * \param	state : TravState to cull
	 */
	void	TravCull(TravState* state) override;

	/**
	 * \brief	recompute this Node3D orientation according to current position and target
	 * \fn 		void	DoOrientation();
	 */
	void	DoOrientation();

	//! target name used to search target in scenegraph
	maString	myTargetName;
	//! position x
	maFloat		myPosX;
	//! position y
	maFloat		myPosY;
	//! position z
	maFloat		myPosZ;
	//! define the axis pointing to the target : X,Y or Z
	maEnum<3>	myOrientedAxis;
	//! define the pseudo constant axis (probably up vector) : X,Y or Z
	maEnum<3>	myPseudoConstantAxis;
	//! the pseudo constant axis vector coordinate
	maVect3DF	myPseudoConstantAxisDir;

	//! found target
	Node3D*		myCurrentTarget; 
	
	//! last computed target position 
	Point3D		myLastTargetPos;
}; 

#endif //_AUTOORIENTEDNODE3D_H_
