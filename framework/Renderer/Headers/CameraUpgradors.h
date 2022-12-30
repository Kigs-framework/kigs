#pragma once
#include "Camera.h"

namespace Kigs
{

	namespace Draw
	{
		// ****************************************
		// * OrbitCameraUp class
		// * --------------------------------------
		/**
		 * \file	CameraUpgradors.h
		 * \class	OrbitCameraUp
		 * \ingroup Renderer
		 * \brief	Camera updragor to control the camera with mouse/keyboard.
		 */
		 // ****************************************
		class OrbitCameraUp : public Upgrador<Camera>
		{

			// create and init Upgrador if needed and add dynamic attributes
			virtual void	Init(CoreModifiable* toUpgrade) override;

			// destroy UpgradorData and remove dynamic attributes 
			virtual void	Destroy(CoreModifiable* toDowngrade, bool toDowngradeDeleted) override;

			START_UPGRADOR(OrbitCameraUp);
			UPGRADOR_METHODS(OrbitCamNotifyUpdate, ManageDirectTouch, GetDistanceForInputSort);

			enum class Operation
			{
				None,
				Orbit,
				Move
			};


			Operation GetCurrentOperation() const
			{
				return mCurrentOperation;
			}

			maVect3DF* mOrbitPoint;
			maVect3DF* mOrbitDirection;
			maVect3DF* mOrbitUp;
			maFloat* mOrbitDistance;

			maBool* mEnabled;
			maBool* mOrbitNeedCtrl;

			Operation mCurrentOperation = Operation::None;

			v3f mOperationStartInputPosition;
			v3f mOperationStartOrbitPosition;
			v3f mOperationStartOrbitView;
			v3f mOperationStartOrbitUp;

			bool mIsDirty = true;
			double mLastTime = 0.0;
			bool mHovered = false;

		};
	}
}

