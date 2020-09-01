#ifndef _RENDERINGSCREEN_H_
#define _RENDERINGSCREEN_H_

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "AttributePacking.h"
#include "TravState.h"
#include "ModuleInput.h"
#include "TouchInputEventManager.h"
#include "Texture.h"

#include "kstlvector.h"

class Texture;
class Window;

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
	* \fn 		void	GetRotation(kfloat& rot)
	* \param	rot : rotation of the screen(in/out param)
	*/
	void	GetRotation(kfloat& rot)
	{
		rot = mRotation;
	}

	/**
	* \brief	get the size of the screen
	* \fn 		void	GetSize(kfloat& sizeX,kfloat& sizeY)
	* \param	sizeX : size on the x axis (in/out param)
	* \param	sizeY : size on the y axis (in/out param)
	*/
	void	GetSize(kfloat& sizeX, kfloat& sizeY)
	{
		if (mUseFBO)
		{
			sizeX = (kfloat)mFBOSizeX; sizeY = (kfloat)mFBOSizeY;
		}
		else
		{
			sizeX = (kfloat)(unsigned int)mSizeX; sizeY = (kfloat)(unsigned int)mSizeY;
		}
	}

	/**
	* \brief	get the design size of the screen : the resolution the graphics shown on screen were designed for
	* \fn 		void	GetDesignSize(kfloat& sizeX,kfloat& sizeY)
	* \param	dsizeX : design size on the x axis (in/out param)
	* \param	dsizeY : design size on the y axis (in/out param)
	*/
	void	GetDesignSize(kfloat& dsizeX, kfloat& dsizeY) { dsizeX = (kfloat)(unsigned int)mDesignSizeX; dsizeY = (kfloat)(unsigned int)mDesignSizeY; }


	/**
	* \brief	check if the screen has been initialized
	* \fn 		bool	IsInit()
	* \return	TRUE if the screen has been initialized, FALSE if not
	*/
	bool	IsInit() { return mIsInit; }

	/**
	* \brief	resize the screen
	* \fn 		virtual void	Resize(kfloat sizeX,kfloat sizeY)=0;
	* \param	sizeX : new size on the x axis
	* \param	sizeY : new size on the y axis
	*/
	virtual void	Resize(kfloat sizeX, kfloat sizeY) = 0;

	/**
	* \brief	Set the target window handle of the rendering. If null, the Window name passed through "ParentWindowName" will be used.
	* \fn 		virtual void	SetWindowByHandle(void *PtrToHandle) = 0;
	* \param	PtrToHandle : window handle
	*/
	virtual void	SetWindowByHandle(void *PtrToHandle) = 0;

	/**
	* \brief	get the target window handle of the rendering.
	* \fn 		virtual void*	GetContextHandle()=0;
	*/
	virtual void*	GetContextHandle() = 0;

	void SetBackDropColor(kfloat r, kfloat g, kfloat b);

	virtual void FetchPixels(int x, int y, int width, int height, void *pRGBAPixels) = 0;
	virtual void FetchDepth(int x, int y, int width, int height, float *pDepthPixels) = 0;
	virtual void FetchDepth(int x, int y, int width, int height, unsigned int *pDepthPixels) = 0;

	void NotifyUpdate(const unsigned int  labelid) override;

	void GetMousePosInScreen(int posx, int posy, kfloat& sposx, kfloat& sposy);
	void GetMouseMoveInScreen(kfloat posx, kfloat posy, kfloat& sposx, kfloat& sposy);
	void GetMousePosInDesignScreen(int posx, int posy, kfloat& sposx, kfloat& sposy);
	void GetMouseRatioInScreen(int posx, int posy, kfloat& sposx, kfloat& sposy);
	void GetMouseRatio(int posx, int posy, kfloat& sposx, kfloat& sposy);


	v2f GlobalMousePositionToDesignPosition(v2i pos);

	bool MouseIsInScreen(int posx, int posy);

	void ManageFade(TravState*);

	SP<Texture> GetFBOTexture() 
	{
		if(mUseFBO)
			return mFBOTexture; 

		return nullptr;
	}

	bool IsHolographic() { return mIsStereo; }

	/**
	* \brief	convert touch pos in local "touch support" coordinates
	* \fn 		bool GetDataInTouchSupport(const touchPosInfos& posin, touchPosInfos& pout);
	* Convert posin : "global touch coordinates" to pout : local "touch support" coordinates
	*/
	bool GetDataInTouchSupport(const touchPosInfos& posin, touchPosInfos& pout);

	bool IsValidTouchSupport(Vector3D posin);

	DECLARE_METHOD(ResetContext);

	COREMODIFIABLE_METHODS(ResetContext);

protected:
	// recompute mDesignCoefX,mDesignCoefY;
	void	InitModifiable() override;

	void Update(const Timer&  timer, void* /*addParam*/) override;

	//! TRUE if the screen has been initialized
	bool mIsInit;

	//! size of a pixel in bit
	maInt mBitsPerPixel;
	//! size of the Z value in bit
	maInt mBitsPerZ;

	maInt mBitsForStencil;

	//!  size of the screen on x axis
	maUInt mSizeX;
	//!  size of the screen on y axis
	maUInt mSizeY;
	//!  design size of the screen on x axis 
	maUInt mDesignSizeX;
	//!  design size of the screen on y axis
	maUInt mDesignSizeY;
	//! ?
	maBool mIsOffScreen;
	//! name of the parent windows
	maString mParentWindowName;
	//! TRUE if I wait for the VSync
	maBool mVSync;
	//! brightness : between -16 (black) and 16 (white)
	maFloat mBrightness;
	float mOldBrightness;
	//! back drop color
	maVect3DF mBackDropColor;
	//! screen orientation (for handheld devices)
	maFloat mRotation;
	//! indicate the screen don't need clear (camera should still clear their part of screen)
	maBool mDontClear;
	//! double buffer on/off
	maBool mNeedDoubleBuffer;

	maBool mIsStereo = BASE_ATTRIBUTE(IsStereo, false);

	Window* mParentWindow;

	void				RecomputeDesignCoef();

	kfloat				mDesignCoefX,mDesignCoefY;

	bool				mWasActivated;

	maBool				mUseFBO;

	maInt				mFBOSizeX;
	maInt				mFBOSizeY;

	SP<Texture>			mFBOTexture;

	virtual void		setCurrentContext() = 0;
};

#endif //_RENDERINGSCREEN_H_
