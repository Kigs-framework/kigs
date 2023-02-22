#pragma once

#include "Drawable.h"
#include "Texture.h"
#include "UIVerticesInfo.h"

namespace Kigs
{

	namespace Draw
	{
		class Texture;

		// ****************************************
		// * Holo3DPanel class
		// * --------------------------------------
		/**
		 * \file	Holo3DPanel.h
		 * \class	Holo3DPanel
		 * \ingroup Renderer
		 * \brief	Draw a textured quad or circular panel.
		 *
		 * Used by Hololens menus.
		 * Do we need this specific class ?
		 *
		 */
		 // ****************************************

		class Holo3DPanel : public Drawable
		{
		public:
			DECLARE_CLASS_INFO(Holo3DPanel, Drawable, Renderer);
			DECLARE_CONSTRUCTOR(Holo3DPanel);


			bool addItem(const CMSP& item, ItemPosition pos = Last) override;
			bool removeItem(const CMSP& item) override;

			void SetTextureID(unsigned int id) { mTextureID = id; }
			void SetTexture(SP<Texture> t);

			bool Draw(TravState* travstate) override;

		protected:

			void InitModifiable() override;

			SmartPointer<Texture> mTexture;
			unsigned int mTextureID = -1;

			Draw2D::UIVerticesInfo mVI;

			bool				mTwoSided = false;
			s32					mCullMode = 1;

			v2f					mOffset = { 0.0f, 0.0f };
			/*
				Size:
					For a plane, SizeX, SizeY
					For a circle Radius, NumberOfPoints
			*/
			v2f					mSize = { 0.0f, 0.0f };

			v2f					mUVStart = { 0.0f, 0.0f };
			v2f					mUVEnd = { 1.0f, 1.0f };

			v3f					mUp = { 0.0f, 1.0f, 0.0f };
			v3f					mNormal = { 0.0f, 0.0f, 1.0f };

			WRAP_ATTRIBUTES(mTwoSided, mCullMode, mOffset, mSize, mUVStart, mUVEnd, mUp, mNormal);

			maEnum<3> mDepthTest = BASE_ATTRIBUTE(DepthTest, "NoChange", "Enabled", "Disabled");
			maEnum<3> mDepthWrite = BASE_ATTRIBUTE(DepthWrite, "NoChange", "Enabled", "Disabled");
			maEnum<2> mShape = BASE_ATTRIBUTE(Shape, "Plane", "Circle");

		};

	}
}
