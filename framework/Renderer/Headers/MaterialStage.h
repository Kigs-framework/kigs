#pragma once
  
#include "SceneGraphDefines.h"
#include "Drawable.h"

#ifndef NO_MULTISTAGE_RENDERING
#ifndef DO_MULTISTAGE_RENDERING
#error	"missing ScenegraphDefines.h"
#endif

namespace Kigs
{

	namespace Draw
	{

		class Texture;
		// ****************************************
		// * MaterialStage class
		// * --------------------------------------
		/**
		* \file	MaterialStage.h
		* \class	MaterialStage
		* \ingroup Renderer
		* \brief	Base class for material stage.
		*
		*/
		// ****************************************
		class MaterialStage : public Drawable
		{
		public:
			friend class Material;

			DECLARE_CLASS_INFO(MaterialStage, Drawable, Renderer);
			DECLARE_CONSTRUCTOR(MaterialStage);

			unsigned int GetSelfDrawingNeeds() override
			{
				return ((unsigned int)Need_Predraw) | ((unsigned int)Need_Postdraw);
			}
			bool PreDraw(TravState*) override;
			bool PostDraw(TravState* travstate) override;

		protected:
			DECLARE_METHOD(ResetContext);

			COREMODIFIABLE_METHODS(ResetContext);

			s32				mTexEnv =  0;
			u32				mTextureChannel = 0;

			WRAP_ATTRIBUTES(mTexEnv, mTextureChannel);
		};

	}
}

#endif