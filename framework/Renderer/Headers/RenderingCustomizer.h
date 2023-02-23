#pragma once 

#include "Drawable.h"

namespace Kigs
{

	namespace Draw
	{

		// ****************************************
		// * RenderingCustomizer class
		// * --------------------------------------
		/**
		* \file	RenderingCustomizer.h
		* \class	RenderingCustomizer
		* \ingroup Renderer
		* \brief Change some rendering states during draw
		*
		*/
		// ****************************************
		class RenderingCustomizer : public Drawable
		{
		public:
			DECLARE_CLASS_INFO(RenderingCustomizer, Drawable, ModuleName);
			DECLARE_INLINE_CONSTRUCTOR(RenderingCustomizer) {}

		private:

			unsigned int GetSelfDrawingNeeds() override
			{
				return ((unsigned int)Need_Predraw) | ((unsigned int)Need_Postdraw);
			}

			virtual bool PreDraw(TravState*) override;
			virtual bool PostDraw(TravState*) override;


			int mLastCullMode = -1;
			int mLastDepthTest = -1;

			bool mNeedPop = false;

			s32		mOverrideCullMode = -1;
			s32		mOverrideDepthTest = -1;
			s32		mOverrideDepthWrite = -1;

			WRAP_ATTRIBUTES(mOverrideCullMode, mOverrideDepthTest, mOverrideDepthWrite);

			void SaveState(TravState* state);
			void RestoreState(TravState* state);
		};

	}
}