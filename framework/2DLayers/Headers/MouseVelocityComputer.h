#pragma once

#include "CoreModifiable.h"

namespace Kigs
{
	namespace Draw2D
	{
		using namespace Kigs::Core;
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
			~MouseVelocityComputer();
			MouseVelocityComputer(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			using CoreModifiable::Init;
			void Init(double time);
			void StoreDisplacement(float dx, float dy, double time);
			void ComputeVelocity(float& vx, float& vy);

		private:
			float* mVelocityX;
			float* mVelocityY;
			int		mVelocityIndex;
			int		mVelocityCount;
			double mOldTime;
			const static int cMaxIndex = 5;
		};

	}
}