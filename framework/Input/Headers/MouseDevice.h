#ifndef _MOUSEDEVICE_H_
#define _MOUSEDEVICE_H_

#include "InputDevice.h"


// ****************************************
// * MouseDevice class
// * --------------------------------------
/**
 * \class	MouseDevice
 * \file	MouseDevice.h
 * \ingroup Input
 * \brief	Base class to manage generic mouse device.
 */
 // ****************************************
class MouseDevice : public InputDevice
{
public:
	//! buttons list
	enum MOUSE_BUTTONS
	{
		LEFT=0,
		RIGHT,
		MIDDLE
	};

	DECLARE_ABSTRACT_CLASS_INFO(MouseDevice,InputDevice,Input)
	
	/**
	 * \brief	constructor
	 * \fn 		MouseDevice(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	MouseDevice(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	/**
	 * \brief	button count for this mouse
	 * \fn 		int				getButtonsCount()
	 * \return	the number of buttons
	 */
	int				getButtonsCount(){return mButtonsCount;}

	/**
	 * \brief	wheel count for this mouse
	 * \fn 		int				getWheelCount()
	 * \return	the wheel count for this mouse
	 */
	int				getWheelCount(){return mWheelCount;}

	/**
	 * \brief	wheel roll (Z axis) for this mouse
	 * \fn 		int				getWheelRoll()
	 * \return	the wheel roll (Z axis) for this mouse
	 */
	float				getWheelRoll(){return mDZ;}

	/**
	 * \brief	delta for wheel roll (Z axis) for this mouse
	 * \fn 		int				getWheelRollDelta()
	 * \return	delta for the wheel roll (Z axis) for this mouse
	 */
	float				getWheelRollDelta(){return mDZ - mPreviousDZ;}

	/**
	 * \brief	retreive current position
	 * \fn 		void			getPos(float& posX,float& posY)
	 * \param	posX : position on x axis (in/out param)
	 * \param	posY : position on y axis (in/out param)
	 */
	void			getPos(float& posX,float& posY){posX=mPosX; posY=mPosY;}

	v2f				GetPos() const { return v2f{ mPosX, mPosY }; }

	//! retreive movement
	/**
	 * \brief	retreive movement
	 * \fn 		void			getMouvement(float& dX,float& dY)
	 * \param	dX : movement on x axis (in/out param)
	 * \param	dY : movement on y axis (in/out param)
	 */
	void			getMouvement(float& dX,float& dY){dX=mDX; dY=mDY;}

	/**
	 * \brief	compute current position
	 * \fn 		virtual void	Update();
	 */
	void	UpdateDevice() override;

	/**
	 * \brief	retreive given button state
	 * \fn 		int				getButtonState(MOUSE_BUTTONS button)
	 * \param	button : given button
	 * \return	state of the given button
	 */
	int getButtonState(MOUSE_BUTTONS button)
	{
		return GetItemiState(2+mWheelCount+button);
	}
	
	void setButtonState(MOUSE_BUTTONS button, int val);

protected:
	/**
	 * \brief	destructor
	 * \fn 		~MouseDevice();
	 */
	virtual ~MouseDevice(); 

	//! wheel count
	int		mWheelCount;
	//! button count
	int		mButtonsCount;

	//! mouse cursor current position on x axis
	maFloat	mPosX;
	//! mouse cursor current position on y axis
	maFloat	mPosY;

	//! mouse movement on x axis
	float	mDX;
	//! mouse movement on y axis
	float	mDY;
	//! wheel movement
	float	mDZ;
	//! delta for wheel movement
	float	mPreviousDZ;
};

#endif //_MOUSEDEVICE_H_
