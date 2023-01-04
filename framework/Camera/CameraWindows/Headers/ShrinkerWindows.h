#pragma once

#include "CoreModifiable.h"
#include "GenericShrinker.h"

namespace Kigs
{

	namespace Camera
	{
		// ****************************************
		// * ShrinkerWindows class
		// * --------------------------------------
		/*!  \class ShrinkerWindows
			 reduce camera frame size
		*/
		// ****************************************


		class ShrinkerWindows : public GenericShrinker
		{
		public:


			DECLARE_CLASS_INFO(ShrinkerWindows, GenericShrinker, CameraModule)

				//! constructor
				ShrinkerWindows(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			virtual void	Process();
			//! destructor
			virtual ~ShrinkerWindows();

		protected:


			virtual	void AllocateFrameBuffers();
			virtual	void FreeFrameBuffers();

			virtual	void	InitModifiable();

			void	RGBShrink();
			void	YUVShrink();

		};

	}
}