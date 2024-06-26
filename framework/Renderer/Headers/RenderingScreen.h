#pragma once

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "AttributePacking.h"
#include "TravState.h"
#include "ModuleInput.h"
#include "TouchInputEventManager.h"
#include "Texture.h"

#include <vector>

namespace Kigs
{
	namespace Gui
	{
		class Window;
	}
	namespace Draw
	{
		class Texture;


		// ****************************************
		// * RenderingScreen class
		// * --------------------------------------
		/**
		* \file	RenderingScreen.h
		* \class	RenderingScreen
		* \ingroup Renderer
		* \brief Generic "draw surface".
		*
		*/
		// ****************************************
		class RenderingScreen : public CoreModifiable
		{
		public:
			DECLARE_ABSTRACT_CLASS_INFO(RenderingScreen, CoreModifiable, Renderer)
				DECLARE_CONSTRUCTOR(RenderingScreen);
			SIGNALS(Resized);
			WRAP_METHODS(IsValidTouchSupport, GetDataInTouchSupport, SetActive, Release);

			/**
			* \brief	active the screen
			* \fn 		virtual bool    SetActive(TravState* state);
			* \return	return true if rendering screen can be activated
			*/
			virtual bool    SetActive(TravState* state);

			/**
			* \brief	release the screen
			* \fn 		virtual void    Release()=0;
			*/
			virtual void    Release(TravState* state) = 0;


			/**
			* \brief	get the rotation of the screen
			* \fn 		void	GetRotation(float& rot)
			* \param	rot : rotation of the screen(in/out param)
			*/
			void	GetRotation(float& rot)
			{
				rot = mRotation;
			}

			/**
			* \brief	get the size of the screen
			* \fn 		void	GetSize(float& sizeX,float& sizeY)
			* \param	sizeX : size on the x axis (in/out param)
			* \param	sizeY : size on the y axis (in/out param)
			*/
			void	GetSize(float& sizeX, float& sizeY)
			{
				sizeX = (float)mSize[0]; sizeY = (float)mSize[1];
			}

			/**
			* \brief	get the design size of the screen : the resolution the graphics shown on screen were designed for
			* \fn 		void	GetDesignSize(float& sizeX,float& sizeY)
			* \param	dsizeX : design size on the x axis (in/out param)
			* \param	dsizeY : design size on the y axis (in/out param)
			*/
			void	GetDesignSize(float& dsizeX, float& dsizeY) { dsizeX = (float)mDesignSize[0]; dsizeY = (float)mDesignSize[1]; }


			/**
			* \brief	check if the screen has been initialized
			* \fn 		bool	IsInit()
			* \return	TRUE if the screen has been initialized, FALSE if not
			*/
			bool	IsInit() { return mIsInit; }

			/**
			* \brief	resize the screen
			* \fn 		virtual void	Resize(float sizeX,float sizeY)=0;
			* \param	sizeX : new size on the x axis
			* \param	sizeY : new size on the y axis
			*/
			virtual void	Resize(float sizeX, float sizeY) = 0;

			/**
			* \brief	Set the target window handle of the rendering. If null, the Window name passed through "ParentWindowName" will be used.
			* \fn 		virtual void	SetWindowByHandle(void *PtrToHandle) = 0;
			* \param	PtrToHandle : window handle
			*/
			virtual void	SetWindowByHandle(void* PtrToHandle) = 0;

			/**
			* \brief	get the target window handle of the rendering.
			* \fn 		virtual void*	GetContextHandle()=0;
			*/
			virtual void* GetContextHandle() = 0;

			void SetBackDropColor(float r, float g, float b);

			virtual void FetchPixels(int x, int y, int width, int height, void* pRGBAPixels) = 0;
			virtual void FetchDepth(int x, int y, int width, int height, float* pDepthPixels) = 0;
			virtual void FetchDepth(int x, int y, int width, int height, unsigned int* pDepthPixels) = 0;

			void NotifyUpdate(const unsigned int  labelid) override;

			void GetMousePosInScreen(int posx, int posy, float& sposx, float& sposy);
			void GetMouseMoveInScreen(float posx, float posy, float& sposx, float& sposy);
			void GetMousePosInDesignScreen(int posx, int posy, float& sposx, float& sposy);
			void GetMouseRatioInScreen(int posx, int posy, float& sposx, float& sposy);
			void GetMouseRatio(int posx, int posy, float& sposx, float& sposy);


			v2f GlobalMousePositionToDesignPosition(v2i pos);

			bool MouseIsInScreen(int posx, int posy);

			void ManageFade(TravState*);

			SP<Texture> GetFBOTexture()
			{
				if (mUseFBO)
					return mFBOTexture;

				return nullptr;
			}

			bool IsHolographic() { return mIsStereo; }

			/**
			* \brief	convert touch pos in local "touch support" coordinates
			* \fn 		bool GetDataInTouchSupport(const touchPosInfos& posin, touchPosInfos& pout);
			* Convert posin : "global touch coordinates" to pout : local "touch support" coordinates
			*/
			bool GetDataInTouchSupport(const Input::touchPosInfos& posin, Input::touchPosInfos& pout);

			bool IsValidTouchSupport(v3f posin);

			DECLARE_METHOD(ResetContext);

			COREMODIFIABLE_METHODS(ResetContext);

			Gui::Window* GetParentWindow() const { return mParentWindow; };

		protected:
			// recompute mDesignCoefX,mDesignCoefY;
			void	InitModifiable() override;

			void Update(const Timer& timer, void* /*addParam*/) override;

			//! TRUE if the screen has been initialized
			bool mIsInit;

			//! ?
			bool			mIsOffScreen = false;
			//! TRUE if I wait for the VSync
			bool			mVSync = false;
			//! indicate the screen don't need clear (camera should still clear their part of screen)
			bool			mDontClear = false;
			//! double buffer on/off
			bool			mNeedDoubleBuffer = true;

			bool			mIsStereo = false;

			bool			mResizeDesignSize = false;
			bool			mUseFBO = false;

			//! size of a pixel in bit
			s32				mBitsPerPixel = 32;
			//! size of the Z value in bit
			s32				mBitsPerZ = 16;

			s32				mBitsForStencil = 0;

			//! brightness : between -16 (black) and 16 (white)
			float			mBrightness = 0.0f;
			float			mOldBrightness;
			//! screen orientation (for handheld devices)
			float			mRotation = 0.0f;

			//!  size of the screen 
			v2f				mSize = { 256.0f , 256.0f };
			//!  design size of the screen  
			v2f				mDesignSize = { 256.0f , 256.0f };
			//! back drop color
			v3f				mBackDropColor = { 0.0f , 0.0f , 0.0f };

			//! name of the parent windows
			std::string		mParentWindowName = "";

			WRAP_ATTRIBUTES(mIsOffScreen, mVSync, mDontClear, mNeedDoubleBuffer, mIsStereo, mResizeDesignSize, mUseFBO, mBitsPerPixel, mBitsPerZ, mBitsForStencil, mBrightness, mRotation, mSize, mDesignSize, mBackDropColor, mParentWindowName)

			Gui::Window*		mParentWindow;

			void				RecomputeDesignCoef();

			v2f					mDesignCoef;

			bool				mWasActivated;

			SP<Texture>			mFBOTexture;

			virtual void		setCurrentContext() = 0;
		};
	}
}
