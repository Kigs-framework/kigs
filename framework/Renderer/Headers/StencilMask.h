#pragma once

#include "Drawable.h"

namespace Kigs
{

	namespace Draw
	{
		// ****************************************
		// * StencilMask class
		// * --------------------------------------
		/**
		* \file	StencilMask.h
		* \class	StencilMask
		* \ingroup Renderer
		* \brief Manage stencil buffer mask.
		*
		*/
		// ****************************************

		class StencilMask : public Drawable
		{
		public:
			DECLARE_CLASS_INFO(StencilMask, Drawable, ModuleName);
			DECLARE_INLINE_CONSTRUCTOR(StencilMask) {}


		private:

			virtual	unsigned int	GetSelfDrawingNeeds() override
			{
				return ((unsigned int)Need_Predraw) | ((unsigned int)Need_Postdraw);
			}

			virtual bool PreDraw(TravState*) override;
			virtual bool PostDraw(TravState*) override;

			bool	mIgnoreDepthTest = false;
			s32		mReferenceValue =  1;
			u32		mMask = 0xFFFFFFFF;

			WRAP_ATTRIBUTES(mIgnoreDepthTest, mReferenceValue,mMask);

			maEnum<3> mMaskUsage = BASE_ATTRIBUTE(MaskUsage, "Build", "Use", "Ignore");
			maEnum<2> mMaskMode = BASE_ATTRIBUTE(MaskMode, "IncrWrap", "Replace");
		};
	}
}