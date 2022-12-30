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

			maEnum<3> MaskUsage = BASE_ATTRIBUTE(MaskUsage, "Build", "Use", "Ignore");
			maInt ReferenceValue = BASE_ATTRIBUTE(ReferenceValue, 1);
			maUInt Mask = BASE_ATTRIBUTE(Mask, 0xFFFFFFFF);
			maEnum<2> MaskMode = BASE_ATTRIBUTE(MaskMode, "IncrWrap", "Replace");
			maBool IgnoreDepthTest = BASE_ATTRIBUTE(IgnoreDepthTest, false);
		};
	}
}