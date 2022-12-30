#pragma once

#include "UIDrawableItem.h"

namespace Kigs
{
	namespace Draw
	{
		class Texture;
	}
	namespace Draw2D
	{

		// ****************************************
		// * UISliderFill class
		// * --------------------------------------
		/**
		* \file	UISliderFill.h
		* \class	UISliderFill
		* \ingroup 2DLayers
		* \brief	TODO. Manage a slider.
		*/
		// ****************************************

		class UISliderFill : public UIDrawableItem
		{
		public:
			DECLARE_CLASS_INFO(UISliderFill, UIDrawableItem, 2DLayers);

			/**
			 * \brief	constructor
			 * \param	name : instance name
			 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
			 */
			UISliderFill(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			inline SP<Draw::Texture>	GetVoidTexture() { return mVoidTexturePointer; }
			inline SP<Draw::Texture>  GetStartTexture() { if (mStartPositionX > -1 && mStartPositionY > -1) return mStartTexturePointer; else return nullptr; }
			inline SP<Draw::Texture>  GetMiddleTexture() { if (mMiddlePositionX > -1 && mMiddlePositionY > -1) return mMiddleTexturePointer; else return nullptr; }
			inline SP<Draw::Texture>  GetEndTexture() { if (mEndPositionX > -1 && mEndPositionY > -1) return mEndTexturePointer; else return nullptr; }

			inline void			Get_StartPosition(int& X, int& Y) { X = mStartPositionX; Y = mStartPositionY; }
			inline void			Get_MiddlePosition(int& X, int& Y) { X = mMiddlePositionX; Y = mMiddlePositionY; }
			inline void			Get_MiddleSize(int& X, int& Y) { X = mMiddleSizeX; Y = mMiddleSizeY; }
			inline void			Get_EndPosition(int& X, int& Y) { X = mEndPositionX; Y = mEndPositionY; }
			void				ComputeInitialElementsPosition();

			void				ChangeTexture(std::string voidtexturename, std::string texturename = "", std::string overtexturename = "", std::string downtexturename = "");

		protected:

			void		InitModifiable()override;
			void		NotifyUpdate(const unsigned int /* labelid */)override;
			bool		isAlpha(float X, float Y)override;

			void		RecomputeElementsPosition(float dockX, float dockY, float AnchorX, float AnchorY, int posx, int posy, unsigned int sizeX, unsigned int sizeY, int& Xresult, int& Yresult);

			maString			mVoidTexture;
			maString			mStartTexture;
			maString			mMiddleTexture;
			maString			mEndTexture;

			//! Direction of Slider "Vertical" or "Horizontal")
			maEnum<2>			mDirection;

			SP<Draw::Texture>			mStartTexturePointer;
			SP<Draw::Texture>			mMiddleTexturePointer;
			SP<Draw::Texture>			mEndTexturePointer;
			SP<Draw::Texture>			mVoidTexturePointer;

			int					mStartPositionX;
			int					mStartPositionY;
			int					mMiddlePositionX;
			int					mMiddlePositionY;
			int					mMiddleSizeX;
			int					mMiddleSizeY;
			int					mEndPositionX;
			int					mEndPositionY;
		};

	}
}