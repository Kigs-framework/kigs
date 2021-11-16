#ifndef _MODULEINPUT_H_
#define _MODULEINPUT_H_

#include "ModuleBase.h"
#include "MouseDevice.h"
#include "AttributePacking.h"
#include "TouchInputEventManager.h"

class MouseDevice;
class SpatialInteractionDevice;
class JoystickDevice;
class KeyboardDevice;
class AccelerometerDevice;
class MultiTouchDevice;
class GeolocationDevice;
class GyroscopeDevice;
class CompassDevice;


SP<ModuleBase> PlatformInputModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);

/*! \defgroup Input Input module
 *  manage keyboards, pad, mouse...
*/

// ****************************************
// * WindowClick class
// * --------------------------------------
/**
* \file	ModuleInput.h
* \class	WindowClick
* \ingroup Input
* \brief Manage mouse click for a given window.
*
*/
// ****************************************
class WindowClick
{
public:
	/**
	 * \brief	constructor
	 * \fn 		WindowClick(CoreModifiable*	w)
	 * \param	w : watched window
	 */
	WindowClick(CoreModifiable*	w)
	{
		mWindow = w;
		mButtonCount = 3;
	
		mPosX = new kfloat[mButtonCount];
		mPosY = new kfloat[mButtonCount];
		mTmpPosX = new kfloat[mButtonCount];
		mTmpPosY = new kfloat[mButtonCount];
		
		for(int i=0; i<mButtonCount; i++)
		{
			mPosX[i] = -1;
			mPosY[i] = -1;
			mTmpPosX[i] = -1;
			mTmpPosY[i] = -1;
		}
	}

	/**
	 * \brief	destructor
	 * \fn 		~WindowClick()
	 */
	~WindowClick()
	{
		delete [] mPosX;
		delete [] mPosY;
		delete [] mTmpPosX;
		delete [] mTmpPosY;
	};

	/**
	 * \brief	update the WindowClick
	 * \fn 		void update()
	 *
	 * set the values stored in the tmpArray in the position array and reset them to -1
	 */
	void update()
	{
		for(int i=0; i<mButtonCount; i++)
		{
			mPosX[i] = mTmpPosX[i];
			mPosY[i] = mTmpPosY[i];
			mTmpPosX[i] = -1;
			mTmpPosY[i] = -1;
		}
	}

	/**
	 * \brief	set the position of the click for a given button in the tmpArray
	 * \fn 		void setPos(int buttonId, kfloat X, kfloat Y)
	 * \param	buttonId : id of the given button
	 * \param	X : position on X axis [0.0, 1.0]
	 * \param	Y : position on Y axis [0.0, 1.0]
	 */
	void setPos(int buttonId, kfloat X, kfloat Y)
	{
		mTmpPosX[buttonId] = X;
		mTmpPosY[buttonId] = Y;
	}

	/**
	 * \brief	get the position of the click for a given button
	 * \fn 		void getPos(int buttonId, kfloat &X, kfloat &Y)
	 * \param	buttonId : id of the given button
	 * \param	X : position on X axis [0.0, 1.0] (out param)
	 * \param	Y : position on Y axis [0.0, 1.0] (out param)
	 */
	void getPos(int buttonId, kfloat &X, kfloat &Y)
	{
		X = mPosX[buttonId];
		Y = mPosY[buttonId];
	}
	
	/**
	 * \brief	check if the WindowClick watch the given window
	 * \fn 		bool isMyWindow(CoreModifiable*	w)
	 * \param	w : checked window
	 * \return	TRUE if the WindowClick watch the given window
	 */
	bool isMyWindow(CoreModifiable*	w)
	{
		return w==mWindow;
	}

private: 
	//! array of X position for all button
	kfloat				*mPosX;
	//! array of Y position for all button
	kfloat				*mPosY;
	//! array of X position for all button
	kfloat				*mTmpPosX;
	//! array of Y position for all button
	kfloat				*mTmpPosY;

	//! number of mouse button (default 3)
	int					mButtonCount;
	//! link to the window
	CoreModifiable*		mWindow;
};

// ****************************************
// * ModuleInput class
// * --------------------------------------
/**
 * \file	ModuleInput.h
 * \class	ModuleInput
 * \ingroup Input
 * \ingroup Module
 * \brief	Generic module for input management.
 */
// ****************************************
class ModuleInput : public ModuleBase
{
public:
	DECLARE_CLASS_INFO(ModuleInput,ModuleBase,Input)

	/**
	 * \brief	constructor
	 * \fn 		ModuleInput(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	ModuleInput(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	/**
	 * \brief		initialize module 
	 * \fn			void Init(Core* core, const kstl::vector<CoreModifiableAttribute*>* params);  
	 * \param		core : link to the core, NOT NULL
	 * \param		params : list of parameters
	 */
	void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;
	 
	/**
	 * \brief		close module 
	 * \fn			void Close(); 
	 */
	void Close() override;

	/**
	 * \brief		update module 
	 * \fn			virtual void Update(const Timer& timer, void* addParam);
	 * \param		timer : timer global
	 */
	void Update(const Timer& timer, void* addParam) override;

	//! return mouse if any	
	/**
	 * \brief		update module
	 * \fn			MouseDevice*		GetMouse()
	 * \param		timer : timer global
	 */
	MouseDevice*		GetMouse(){return mMouse;}

	SpatialInteractionDevice*		GetSpatialInteraction() { return mSpatialInteraction; }

	//! return multiTouch if any	
	/**
	* \brief		update module
	* \fn			MultiTouchDevice*		GetMultiTouch()
	* \param		timer : timer global
	*/
	MultiTouchDevice*		GetMultiTouch(){ return mMultiTouch; }

	/**
	 * \brief		return joystick by index
	 * \fn			JoystickDevice*		GetJoystick(int index);
	 * \param		index : index of the Joystick
	 * \return		the Joystick
	 */
	JoystickDevice*		GetJoystick(int index);

	/**
	 * \brief		return keyboard if any	
	 * \fn			KeyboardDevice*		GetKeyboard()
	 * \return		the keyboard
	 */
	KeyboardDevice*		GetKeyboard(){return mKeyboard;}

	/**
	 * \brief		return joystick count	
	 * \fn			unsigned int	GetJoystickCount()
	 * \return		Joystick count
	 */	
	unsigned int	GetJoystickCount(){return (unsigned int)mJoysticks.size();}
	
	AccelerometerDevice* GetAccelerometer(){return mAccelerometer;}   
	GeolocationDevice* GetGeolocation(){ return mGeolocation; }
	GyroscopeDevice* GetGyroscope(){ return mGyroscope; }
	CompassDevice* GetCompass() { return mCompass; }
	
   /**
	 * \fn 		bool addItem(const CMSP&  item,ItemPosition pos DECLARE_DEFAULT_LINK_NAME );
	 * \brief	method to manage CoreModifiable Tree : add a son
	 * \param	item : item to add
	 * \param	linkName : name of the added item
	 * \return	TRUE if the item is added, FALSE otherwise
	 */
	bool addItem(const CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;
   
   /**
	 * \fn 		static void WindowClickEvent(CoreModifiable *w, int buttonId, kfloat X, kfloat Y);
	 * \brief	function called when a window get a mouse click
	 * \param	w : clicked window
	 * \param	buttonId : Id of the button
	 * \param	X : position on X axis [0.0, 1.0] 
	 * \param	Y : position on Y axis [0.0, 1.0] 
	 */
	static void WindowClickEvent(CoreModifiable *w, int buttonId, kfloat X, kfloat Y, bool isDown);
	
   /**
	 * \fn 		static void WindowDestroyEvent(CoreModifiable *w);
	 * \brief	function called when a window is destroyed
	 * \param	w : clicked window
	 */
	static void WindowDestroyEvent(CoreModifiable *w);

   /**
	 * \fn 		bool getActiveWindowPos(CoreModifiable *w, int buttonId, kfloat &X, kfloat &Y);
	 * \brief	get the position of a click for a given button on a given window
	 * \param	w : given window
	 * \param	buttonId : Id of the button
	 * \param	X : position on X axis [0.0, 1.0] (out param)
	 * \param	Y : position on Y axis [0.0, 1.0] (out param)
	 */
	bool getActiveWindowPos(CoreModifiable *w, MouseDevice::MOUSE_BUTTONS buttonId, kfloat &X, kfloat &Y);
	
   /**
	 * \fn 		void addWindowClick(WindowClick *w) 
	 * \brief	add a WindowClick to the list
	 * \param	w : watched window
	 */
	void addWindowClick(WindowClick *w) { mActiveWindows.push_back(w); }
   /**
	 * \fn 		void removeWindowClick(WindowClick *w)
	 * \brief	remove a WindowClick to the list
	 * \param	w : watched window
	 */
	void removeWindowClick(WindowClick *w) { mActiveWindows.remove(w); }

   /**
	 * \fn 		WindowClick* getWindowClick(CoreModifiable* w)
	 * \brief	get the WindowClick which watch a given Window
	 * \param	w : watched window
	 * \return	the WindowClick, NULL if there is no WindowClick for this Window
	 */
	WindowClick* getWindowClick(CoreModifiable* w)
	{
		if(mActiveWindows.empty())
			return 0;

		kstl::list<WindowClick*>::iterator it;
		for(it=mActiveWindows.begin(); it!=mActiveWindows.end(); ++it)
		{
			WindowClick* theWC=(WindowClick*)*it;
			if(theWC->isMyWindow(w))
				return theWC;
		}
		return 0;
	}


	void registerTouchEvent(CoreModifiable* item, const kstl::string& calledMethod,const kstl::string& eventName,unsigned int EmptyFlag);

	WRAP_METHODS(registerTouchEvent);

	SP<TouchInputEventManager>	getTouchManager()
	{
		return mTouchManager;
	}

protected:
	MouseDevice*					mMouse = nullptr;
	SpatialInteractionDevice*		mSpatialInteraction = nullptr;
	MultiTouchDevice*				mMultiTouch = nullptr;
	KeyboardDevice*					mKeyboard = nullptr;
	AccelerometerDevice*			mAccelerometer = nullptr;
	GeolocationDevice*				mGeolocation = nullptr;
	GyroscopeDevice* 				mGyroscope = nullptr;
	CompassDevice* 					mCompass = nullptr;
	std::vector<JoystickDevice*>	mJoysticks;

	//! list of WindowClick
	std::list<WindowClick*>		mActiveWindows;

	SP<TouchInputEventManager>	mTouchManager;

	std::vector<CMSP> mVirtualSensors;
};

#endif //_MODULEINPUT_H_
