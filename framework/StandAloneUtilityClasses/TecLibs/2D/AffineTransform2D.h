#pragma once

#include "TecLibs/Tec3D.h"

namespace Kigs
{
	namespace Maths
	{
		// utility class to maintain a 2D transform
		class AffineTransform2D
		{
		public:
			inline AffineTransform2D()
			{
			}
			inline ~AffineTransform2D()
			{}

		protected:

			Point2D	mTranslate;
			Point2D	mScale;
			Float	mRotation;
		};

	}
}