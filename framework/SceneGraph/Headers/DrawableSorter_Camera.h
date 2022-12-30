#pragma once


#include "DrawableSorter.h"

namespace Kigs
{
	namespace Draw
	{
		class Camera;
	}
	namespace Scene
	{
		class TravState;


		// ****************************************
		// * DrawableSorter_Camera class
		// * --------------------------------------
		/**
		 * \file	DrawableSorter_Camera.h
		 * \class	DrawableSorter_Camera
		 * \ingroup SceneGraph
		 * \brief	Sort drawables back to front.
		 */
		 // ****************************************
		class DrawableSorter_Camera : public DrawableSorter
		{
		public:
			void SetCamera(Draw::Camera* pCam);
			void CompleteInformationFor(DrawableSorterItem* pItem, TravState* state);

		protected:
			v3f mCameraPosition;
		};

	}
}