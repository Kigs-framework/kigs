#ifndef _MOUSEDEVICE_H_
#define _MOUSEDEVICE_H_

#include "InputDevice.h"

// ****************************************
// * MouseDevice class
// * --------------------------------------
/**
 * \file	MouseDevice.h
 * \class	MouseDevice	
 * \ingroup Input
 * \brief	manage a mouse
 * \author	ukn
 * \version ukn
 * \date	ukn
 * 
 * <dl class="dependency"><dt><b>Dependency:</b></dt><dd>ModuleInput</dd></dl>
 * <dl class="exemple"><dt><b>Exemple:</b></dt><dd>
 * <span class="comment"> Load the module :</span><br>
 * <span class="code">
 * theInputModule=new ModuleInput(<i>instance_name</i>);<br>
 * theInputModule->Init(KigsCore::Instance(),0);<br>
 * </span>
 * <span class="comment"> Get the object :</span><br>
 * <span class="code">
 * theMouse=theInputModule->GetMouse();<br>
 * </span>
 * <span class="comment">Get a button state :</span><br>
 * <span class="code">
 * theInputModule->GetMouse()->getButtonState(<i>mouse_button_id</i>);<br>
 * <span class="comment">// 'mouse_button_id' is a MOUSE_BUTTONS</span><br>
 * </span>
 * <span class="comment">DO NOT FORGET MODULE UPDATE</span><br>
 * </dd></dl>
 *
 * <dl class="exported"><dt><b>Exported parameters :</b></dt><dd>
 * <table>
 * <tr><td>kfloat</td><td><strong>PosX</strong> :</td><td>current mouse position on x axis</td></tr>	
 * <tr><td>kfloat</td><td><strong>PosY</strong> :</td><td>current mouse position on y axis</td></tr>
 * </table>
 * </dd></dl>
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
	 * \fn 		MouseDevice(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	MouseDevice(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	/**
	 * \brief	button count for this mouse
	 * \fn 		int				getButtonsCount()
	 * \return	the number of buttons
	 */
	int				getButtonsCount(){return myButtonsCount;}

	/**
	 * \brief	wheel count for this mouse
	 * \fn 		int				getWheelCount()
	 * \return	the wheel count for this mouse
	 */
	int				getWheelCount(){return myWheelCount;}

	/**
	 * \brief	wheel roll (Z axis) for this mouse
	 * \fn 		int				getWheelRoll()
	 * \return	the wheel roll (Z axis) for this mouse
	 */
	kfloat				getWheelRoll(){return myDZ;}

	/**
	 * \brief	delta for wheel roll (Z axis) for this mouse
	 * \fn 		int				getWheelRollDelta()
	 * \return	delta for the wheel roll (Z axis) for this mouse
	 */
	kfloat				getWheelRollDelta(){return myDZ - myPreviousDZ;}

	/**
	 * \brief	retreive current position
	 * \fn 		void			getPos(kfloat& posX,kfloat& posY)
	 * \param	posX : position on x axis (in/out param)
	 * \param	posY : position on y axis (in/out param)
	 */
	void			getPos(kfloat& posX,kfloat& posY){posX=myPosX; posY=myPosY;}

	v2f				GetPos() const { return v2f{ myPosX, myPosY }; }

	//! retreive movement
	/**
	 * \brief	retreive movement
	 * \fn 		void			getMouvement(kfloat& dX,kfloat& dY)
	 * \param	dX : movement on x axis (in/out param)
	 * \param	dY : movement on y axis (in/out param)
	 */
	void			getMouvement(kfloat& dX,kfloat& dY){dX=myDX; dY=myDY;}

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
		return GetItemiState(2+myWheelCount+button);
	}
	
	void setButtonState(MOUSE_BUTTONS button, int val);

protected:
	/**
	 * \brief	destructor
	 * \fn 		~MouseDevice();
	 */
	virtual ~MouseDevice(); 

	//! wheel count
	int		myWheelCount;
	//! button count
	int		myButtonsCount;

	//! mouse cursor current position on x axis
	maFloat	myPosX;
	//! mouse cursor current position on y axis
	maFloat	myPosY;

	//! mouse movement on x axis
	kfloat	myDX;
	//! mouse movement on y axis
	kfloat	myDY;
	//! wheel movement
	kfloat	myDZ;
	//! delta for wheel movement
	kfloat	myPreviousDZ;
};

#endif //_MOUSEDEVICE_H_
