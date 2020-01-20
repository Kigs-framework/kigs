#ifndef _MODULEINPUT_H_
#define _MODULEINPUT_H_

#include "ModuleBase.h"
#include "MouseDevice.h"
#include "AttributePacking.h"
#include "TouchInputEventManager.h"

class MouseDevice;
class GazeDevice;
class JoystickDevice;
class KeyboardDevice;
class AccelerometerDevice;
class MultiTouchDevice;
class GeolocationDevice;
class GyroscopeDevice;
class CompassDevice;


ModuleBase* PlatformInputModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);

/*! \defgroup Input Input module
 *  manage keyboards, pad, mouse...
*/

// ****************************************
// * WindowClick class
// * --------------------------------------
/**
 * \class	WindowClick
 * \ingroup Input
 * \brief	this class manage mouse click for a window
 * \author	ukn
 * \version ukn
 * \date	ukn
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
		pWindow = w;
		buttonCount = 3;
	
		myPosX = new kfloat[buttonCount];
		myPosY = new kfloat[buttonCount];
		tmpPosX = new kfloat[buttonCount];
		tmpPosY = new kfloat[buttonCount];
		
		for(int i=0; i<buttonCount; i++)
		{
			myPosX[i] = -1;
			myPosY[i] = -1;
			tmpPosX[i] = -1;
			tmpPosY[i] = -1;
		}
	}

	/**
	 * \brief	destructor
	 * \fn 		~WindowClick()
	 */
	~WindowClick()
	{
		delete [] myPosX;
		delete [] myPosY;
		delete [] tmpPosX;
		delete [] tmpPosY;
	};

	/**
	 * \brief	update the WindowClick
	 * \fn 		void update()
	 *
	 * set the values stored in the tmpArray in the position array and reset them to -1
	 */
	void update()
	{
		for(int i=0; i<buttonCount; i++)
		{
			myPosX[i] = tmpPosX[i];
			myPosY[i] = tmpPosY[i];
			tmpPosX[i] = -1;
			tmpPosY[i] = -1;
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
		tmpPosX[buttonId] = X;
		tmpPosY[buttonId] = Y;
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
		X = myPosX[buttonId];
		Y = myPosY[buttonId];
	}
	
	/**
	 * \brief	check if the WindowClick watch the given window
	 * \fn 		bool isMyWindow(CoreModifiable*	w)
	 * \param	w : checked window
	 * \return	TRUE if the WindowClick watch the given window
	 */
	bool isMyWindow(CoreModifiable*	w)
	{
		return w==pWindow;
	}

private: 
	//! array of X position for all button
	kfloat				*myPosX;
	//! array of Y position for all button
	kfloat				*myPosY;
	//! array of X position for all button
	kfloat				*tmpPosX;
	//! array of Y position for all button
	kfloat				*tmpPosY;

	//! number of mouse button (default 3)
	int					buttonCount;
	//! link to the window
	CoreModifiable*		pWindow;
};

// ****************************************
// * ModuleInput class
// * --------------------------------------
/**
 * \file	ModuleInput.h
 * \class	ModuleInput
 * \ingroup Input
 * \ingroup Module
 * \brief	this class is the module manager class
 * \author	ukn
 * \version ukn
 * \date	ukn
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
	MouseDevice*		GetMouse(){return myMouse;}

	GazeDevice*		GetGaze() { return myGaze; }

	//! return multiTouch if any	
	/**
	* \brief		update module
	* \fn			MultiTouchDevice*		GetMultiTouch()
	* \param		timer : timer global
	*/
	MultiTouchDevice*		GetMultiTouch(){ return myMultiTouch; }

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
	KeyboardDevice*		GetKeyboard(){return myKeyboard;}

	/**
	 * \brief		return joystick count	
	 * \fn			unsigned int	getJoystickCount()
	 * \return		oystick count
	 */	
	unsigned int	getJoystickCount(){return myJoystickCount;}
	
	AccelerometerDevice* GetAccelerometer(){return myAccelerometer;}   
	GeolocationDevice* GetGeolocation(){ return myGeolocation; }
	GyroscopeDevice* GetGyroscope(){ return myGyroscope; }
	CompassDevice* GetCompass() { return myCompass; }
	
   /**
	 * \fn 		bool addItem(CMSP&  item,ItemPosition pos DECLARE_DEFAULT_LINK_NAME );
	 * \brief	method to manage CoreModifiable Tree : add a son
	 * \param	item : item to add
	 * \param	linkName : name of the added item
	 * \return	TRUE if the item is added, FALSE otherwise
	 */
	bool addItem(CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;
   
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
	void addWindowClick(WindowClick *w) { activeWindow.push_back(w); }
   /**
	 * \fn 		void removeWindowClick(WindowClick *w)
	 * \brief	remove a WindowClick to the list
	 * \param	w : watched window
	 */
	void removeWindowClick(WindowClick *w) { activeWindow.remove(w); }

   /**
	 * \fn 		WindowClick* getWindowClick(CoreModifiable* w)
	 * \brief	get the WindowClick which watch a given Window
	 * \param	w : watched window
	 * \return	the WindowClick, NULL if there is no WindowClick for this Window
	 */
	WindowClick* getWindowClick(CoreModifiable* w)
	{
		if(activeWindow.empty())
			return 0;

		kstl::list<WindowClick*>::iterator it;
		for(it=activeWindow.begin(); it!=activeWindow.end(); ++it)
		{
			WindowClick* theWC=(WindowClick*)*it;
			if(theWC->isMyWindow(w))
				return theWC;
		}
		return 0;
	}


	void registerTouchEvent(CoreModifiable* item, const kstl::string& calledMethod,const kstl::string& eventName,unsigned int EmptyFlag);

	WRAP_METHODS(registerTouchEvent);

	SP<TouchInputEventManager>&	getTouchManager()
	{
		return myTouchManager;
	}

protected:
	/**
	 * \brief	destructor
	 * \fn 		~ModuleInput();
	 */
	virtual ~ModuleInput();	

	//! mouse
	MouseDevice*				myMouse;
	//! gaze device
	GazeDevice*					myGaze = nullptr;
	//! multi touch
	MultiTouchDevice*			myMultiTouch;
	//! joystick array
	JoystickDevice**			myJoysticks;
	//! keyboard
	KeyboardDevice*				myKeyboard;

	AccelerometerDevice *		myAccelerometer;

	GeolocationDevice *			myGeolocation;

	GyroscopeDevice * 			myGyroscope;

	CompassDevice * 			myCompass;

	//! list of WindowClick
	kstl::list<WindowClick*>	activeWindow;

	//! joystick count
	unsigned int				myJoystickCount;

	SP<TouchInputEventManager>	myTouchManager;
};

#endif //_MODULEINPUT_H_
