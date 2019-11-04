#ifndef _JOYSTICKDEVICE_H_
#define _JOYSTICKDEVICE_H_

#include "InputDevice.h"

// ****************************************
// * JoystickDevice class
// * --------------------------------------
/**
 * \file	JoystickDevice.h
 * \class	JoystickDevice
 * \ingroup Input
 * \brief	joystick management
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
 * theMouse=theInputModule->GetJoystick();<br>
 * </span>
 * <span class="comment">Get a button state :</span><br>
 * <span class="code">
 * theInputModule->GetJoystick(<i>joystick_index</i>)->getButtonState(<i>button_id</i>);<br>
 * </span>
 * <span class="comment">DO NOT FORGET MODULE UPDATE</span><br>
 * </dd></dl>
 */
// ****************************************
class JoystickDevice : public InputDevice
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(JoystickDevice,InputDevice,Input)
	
	/**
	 * \brief	constructor
	 * \fn 		JoystickDevice(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	JoystickDevice(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	/**
	 * \brief	update the joystick
	 * \fn 		virtual void	Update();
	 */
	void	UpdateDevice() override {};

	/**
	 * \brief	button count for this joystick
	 * \fn 		int				getButtonsCount()
	 * \return	the number of buttons
	 */
	int				getButtonsCount(){return myButtonsCount;}
	
	/**
	 * \brief	axis count for this joystick
	 * \fn 		int				getAxisCount()
	 * \return	the number of axis
	 */
	int				getAxisCount(){return myAxisCount;}
	
	/**
	 * \brief	POV count for this joystick
	 * \fn 		int				getPovCount()
	 * \return	the number of POV
	 */
	int				getPovCount(){return myPovCount;}
	
	/**
	 * \brief	retreive given button state
	 * \fn 		int				getButtonState(int button)
	 * \param	button : given button
	 * \return	state of the given button
	 */
	int				getButtonState(int button)
	{
		return GetItemiState(button);
	}

	/**
	 * \brief	retreive the x axis state
	 * \fn 		kfloat				getXAxisState(int index)
	 * \param	index : index of the x axis
	 * \return	state of the x axis
	 */
	kfloat				getXAxisState(int index);

	/**
	 * \brief	retreive the y axis state
	 * \fn 		kfloat				getYAxisState(int index)
	 * \param	index : index of the y axis
	 * \return	state of the y axis
	 */
	kfloat				getYAxisState(int index);

	/**
	 * \brief	retreive the z axis state
	 * \fn 		kfloat				getZAxisState(int index)
	 * \param	index : index of the z axis
	 * \return	state of the y axis
	 */
	kfloat				getZAxisState(int index);

	/**
	 * \brief	retreive the POV state
	 * \fn 		kfloat				getPOVState(int index)
	 * \param	index : index of POV
	 * \return	state of POV
	 */
	int				getPOVState(int index)
	{
		return GetItemiState(myButtonsCount+myAxisCount+index);
	}

	/**
	 * \brief	retreive the y axis state
	 * \fn 		kfloat				getYAxisState(int index)
	 * \param	index : index of the y axis
	 * \return	state of the y axis
	 */
	kfloat				getRorationState(int index)
	{
		return GetItemfState(myButtonsCount+myAxisCount+myPovCount+index);
	}

protected:
	//! number of button
	unsigned int myButtonsCount;
	//! number of axis
	unsigned int myAxisCount;
	//! number of pov
	unsigned int myPovCount;

	/**
	 * \brief	destructor
	 * \fn 		~JoystickDevice();
	 */
	virtual ~JoystickDevice();
};

#endif //_JOYSTICKDEVICE_H_
