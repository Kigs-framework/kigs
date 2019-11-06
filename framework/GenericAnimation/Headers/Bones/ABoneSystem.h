// **********************************************************************
// * FILE  : ABoneSystem.h
// * GROUP : Animation Module
// *---------------------------------------------------------------------
// * PURPOSE : Bone Animation System
// * COMMENT :
// *---------------------------------------------------------------------
// * DATES     : 24/05/2000
// **********************************************************************

#ifndef __ABONESYSTEM_H__
#define __ABONESYSTEM_H__

#include "ASystem.h"
#include "ABonesDefines.h"
#include "Bones/APRSStream.h"

/*! \defgroup BoneAnimation Bone Animation submodule 
 *  \ingroup Animation
 *  Bone animation submodule
 */

class  Node3D;

// ****************************************
// * ABoneSystem  class
// * --------------------------------------
/*!
    System managing bone animation 
    \ingroup BoneAnimation
*/ 
// ****************************************


class ABoneSystem : public ASystem<PRSKey>
{
	DECLARE_CLASS_INFO(ABoneSystem, ASystem<PRSKey>, Animation);
public:
	
	DECLARE_CONSTRUCTOR(ABoneSystem);
	
	void InitSystem() override {};

	// ******************************
	// * Animate
	// *-----------------------------
	/*!  call the root channel animate
	*/
	// ******************************
	
	void    Animate(ATimeValue t) override;
	
	// ******************************
	// * SetupDraw
	// *-----------------------------
	/*!  call the root channel animate
	*/
	// ******************************
	void    SetupDraw() override;
	
	void DrawSkeletonRec(int current_index, const Matrix3x4& parent_transform, const Matrix3x4& root, bool need_draw);
	
	
	// ******************************
	// * GetChannelType
	// *-----------------------------
	/*!  return a classID for the default channel type
	*/
	// ******************************
	
	kstl::string    GetChannelType() override
	{
		return "ABoneChannel";
	}
	
	
	
	// *******************
	// * UseOwnHierarchy
	// * -----------------
	/*!  return true
	*/
	// *******************
	
	bool    UseOwnHierarchy() override
	{
		return true;
	};
	
	void		InitLocalToGlobalData() override;
	
	void		ApplyLocalToGlobalData() override;
	
	
	void				UpdateBoneMatrices(AObjectSkeletonResource* skeleton);
	
	protected:
	
	virtual				~ABoneSystem();
	
	// for animation localToGlobalManagement
	
	void				SearchParentNode3D();
	
	Node3D*				  myParentNode3D;
	Matrix4x4*			   myBoneMatrixArray;
	AObjectSkeletonResource* mySkeleton;
	
};


#endif //__ABONESYSTEM_H__


