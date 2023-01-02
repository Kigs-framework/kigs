// **********************************************************************
// * FILE  : ABoneSystem.h
// * GROUP : Animation Module
// *---------------------------------------------------------------------
// * PURPOSE : Bone Animation System
// * COMMENT :
// *---------------------------------------------------------------------
// * DATES     : 24/05/2000
// **********************************************************************

#pragma once

#include "ASystem.h"
#include "ABonesDefines.h"
#include "Bones/APRSStream.h"
namespace Kigs
{
	namespace Scene
	{
		class  Node3D;
	}
	namespace Anim
	{
		/*! \defgroup BoneAnimation Bone Animation submodule
		 *  \ingroup Animation
		 *  Bone animation submodule
		 */


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

			std::string    GetChannelType() override
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


			void				UpdateBoneMatrices(SP<AObjectSkeletonResource> skeleton);
			virtual				~ABoneSystem();

		protected:


			// for animation localToGlobalManagement

			void				SearchParentNode3D();

			SP<Scene::Node3D>					mParentNode3D;
			Matrix4x4* mBoneMatrixArray;
			SP<AObjectSkeletonResource> mSkeleton;

		};

	}
}

