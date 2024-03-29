#pragma once

#include "CoreModifiableAttribute.h"
#include "CoreModifiable.h"
#include <stdlib.h>

// ****************************************
// * AlphaMask class
// * --------------------------------------
/**
* \file	AlphaMask.h
* \class	AlphaMask
* \ingroup 2DLayers
* \brief	Picture mask for touch management.
*
* Obsolete ?
*/
// ****************************************

namespace Kigs
{
	namespace Draw2D
	{
		using namespace Kigs::Core;
		class AlphaMask : public CoreModifiable
		{
		public:
			DECLARE_CLASS_INFO(AlphaMask, CoreModifiable, 2DLayers)
				DECLARE_INLINE_CONSTRUCTOR(AlphaMask) {}


			virtual bool CheckTo(float x, float y);

		protected:

			void InitModifiable() override;

			void CreateMask(u8* pixelData, int stride, int pixel_size);

			float					mThreshold = 0.0f;
			std::string				mTextureName = "";

			WRAP_ATTRIBUTES(mThreshold, mTextureName)

			std::vector<u8> mTab;

			v2i mSize;
		};
	}
}
