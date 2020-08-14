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
* \brief	base for screen object
* \author	ukn
* \version ukn
* \date	ukn
*
* Exported parameters :<br>
* <ul>
* <li>
*		int <strong>BitsPerPixel</strong> :
* </li>
* <li>
*		int <strong>BitsPerZ</strong> :
* </li>
* <li>
*		int <strong>SizeX</strong> : size of the screen on x axis
* </li>
* <li>
*		int <strong>SizeY</strong> : size of the screen on y axis
* </li>
* <li>
*		bool <strong>IsOverlay</strong> : TRUE if the screen is in overlay
* </li>
* <li>
*		bool <strong>Is3DOverlay</strong> : TRUE if the screen is in 3D overlay
* </li>
* <li>
*		bool <strong>IsOffScreen</strong> :
* </li>
* <li>
*		string <strong>ParentWindowName</strong> : name of the parent windows
* </li>
* <li>
*		bool <strong>VSync</strong> : TRUE if I wait for the VSync
* </li>
* <li>
*		int <strong>Brightness</strong> : brightness : between -16 (black) and 16 (white)
* </li>
* </ul>
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
		rot = myRotation;
	}

	/**
	* \brief	get the size of the screen
	* \fn 		void	GetSize(kfloat& sizeX,kfloat& sizeY)
	* \param	sizeX : size on the x axis (in/out param)
	* \param	sizeY : size on the y axis (in/out param)
	*/
	void	GetSize(kfloat& sizeX, kfloat& sizeY)
	{
		if (myUseFBO)
		{
			sizeX = (kfloat)myFBOSizeX; sizeY = (kfloat)myFBOSizeY;
		}
		else
		{
			sizeX = (kfloat)(unsigned int)mySizeX; sizeY = (kfloat)(unsigned int)mySizeY;
		}
	}

	/**
	* \brief	get the design size of the screen : the resolution the graphics shown on screen were designed for
	* \fn 		void	GetDesignSize(kfloat& sizeX,kfloat& sizeY)
	* \param	dsizeX : design size on the x axis (in/out param)
	* \param	dsizeY : design size on the y axis (in/out param)
	*/
	void	GetDesignSize(kfloat& dsizeX, kfloat& dsizeY) { dsizeX = (kfloat)(unsigned int)myDesignSizeX; dsizeY = (kfloat)(unsigned int)myDesignSizeY; }


	/**
	* \brief	check if the screen has been initialized
	* \fn 		bool	IsInit()
	* \return	TRUE if the screen has been initialized, FALSE if not
	*/
	bool	IsInit() { return myIsInit; }

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
		if(myUseFBO)
			return myFBOTexture; 

		return nullptr;
	}

	bool IsHolographic() { return myIsStereo; }

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
	// recompute myDesignCoefX,myDesignCoefY;
	void	InitModifiable() override;

	void Update(const Timer&  timer, void* /*addParam*/) override;

	//! TRUE if the screen has been initialized
	bool myIsInit;

	//! size of a pixel in bit
	maInt mBitsPerPixel;
	//! size of the Z value in bit
	maInt mBitsPerZ;

	maInt mBitsForStencil;

	//!  size of the screen on x axis
	maUInt mySizeX;
	//!  size of the screen on y axis
	maUInt mySizeY;
	//!  design size of the screen on x axis 
	maUInt myDesignSizeX;
	//!  design size of the screen on y axis
	maUInt myDesignSizeY;
	//! ?
	maBool myIsOffScreen;
	//Just for Nitro
	maBool myScreenOnTop;
	//! name of the parent windows
	maString myParentWindowName;
	//! TRUE if I wait for the VSync
	maBool myWaitVSync;
	//! brightness : between -16 (black) and 16 (white)
	maFloat myBrightness;
	float myOldBrightness;
	//! back drop color
	maVect3DF myBackDropColor;
	//! screen orientation (for handheld devices)
	maFloat myRotation;
	//! indicate the screen don't need clear (camera should still clear their part of screen)
	maBool myDontClear;
	//! double buffer on/off
	maBool myNeedDoubleBuffer;

	maBool myIsStereo = BASE_ATTRIBUTE(IsStereo, false);

	Window* MyParentWindow;

	void				RecomputeDesignCoef();

	kfloat				myDesignCoefX,myDesignCoefY;

	bool				myWasActivated;

	maBool				myUseFBO;

	maInt				myFBOSizeX;
	maInt				myFBOSizeY;

	SP<Texture>			myFBOTexture;

	virtual void		setCurrentContext() = 0;
};

#endif //_RENDERINGSCREEN_H_
