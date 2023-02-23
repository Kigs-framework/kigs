#pragma once

#include "UIDrawableItem.h"
#include "maUSString.h"
#include "Texture.h"

namespace Kigs
{

	namespace Draw2D
	{
		// ****************************************
		// * UISlider class
		// * --------------------------------------
		/**
		* \file	UISlider.h
		* \class	UISlider
		* \ingroup 2DLayers
		* \brief	Manage a slider.
		*/
		// ****************************************

		class UISlider : public UIDrawableItem
		{
		public:
			DECLARE_CLASS_INFO(UISlider, UIDrawableItem, 2DLayers);

			/**
			 * \brief	constructor
			 * \param	name : instance name
			 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
			 */
			UISlider(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
			virtual ~UISlider();

			Draw::Texture*	GetTexture();
			void			ChangeTexture(std::string texturename, std::string overtexturename = "", std::string downtexturename = "");
			bool			isPressed() const { return mIsDown; }
			void			ForcedChangeState(bool isdown) { mIsDown = isdown; }
			void			ForcedRatio(unsigned int value);
			void			ResetSlider();
			int				Get_CurrentRatio() const { return mCurrentRatio; }
			void			Set_CurrentRatio(int ratio) { mCurrentRatio = ratio; }
			int				Get_InitialRatio() const { return mInitialRatio; }
			int				Get_Multiplier() { return 100 / mInitialRatio; }
			void			ReloadTexture();

		protected:

			void InitModifiable()override;
			void NotifyUpdate(const unsigned int /* labelid */)override;

			//bool TriggerMouseMove(bool over, float MouseDeltaX, float MouseDeltaY) override;
			//bool TriggerMouseClick(int buttonState, int buttonEvent, int X, int Y, bool & catchClick) override;
			bool isAlpha(float X, float Y)override;

			bool				mIsDown;
			bool				mIsMouseOver;

			SmartPointer<Draw::Texture>	mUpTexturePointer;
			SmartPointer<Draw::Texture>	mDownTexturePointer;
			SmartPointer<Draw::Texture>	mOverTexturePointer;

			s32						mInitialRatio=0;
			usString				mParameter = std::string("");
			std::string				mUpTexture = "";
			std::string				mDownTexture = "";
			std::string				mOverTexture = "";
			std::string				mClickUpAction = "";
			std::string				mMoveAction = "";

			WRAP_ATTRIBUTES(mInitialRatio, mParameter, mUpTexture, mDownTexture, mOverTexture, mClickUpAction, mMoveAction);

			//! Direction of Slider "Vertical" or "Horizontal")
			maEnumInit<2>		mDirection;
			int					mInitialPosX;
			int					mInitialPosY;
			int					mSlideLength;
			int					mCurrentRatio;
		};

	}
}