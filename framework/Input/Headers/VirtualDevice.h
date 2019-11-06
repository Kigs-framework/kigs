#ifndef _VIRTUALDEVICE_H_
#define _VIRTUALDEVICE_H_

#include "InputDevice.h"
 
// ****************************************
// * VirtualDevice class
// * --------------------------------------
/**
 * \file	VirtualDevice.h
 * \class	VirtualDevice
 * \ingroup Input
 * \brief	manage a virtual device (can map keyboard, pad or mouse DeviceItem)
 * \author	ukn
 * \version ukn
 * \date	ukn
 */
// ****************************************
class VirtualDevice : public InputDevice
{
public:
	DECLARE_CLASS_INFO(VirtualDevice,InputDevice,Input)

	/**
	 * \brief	constructor
	 * \fn 		VirtualDevice(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	VirtualDevice(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	/**
	 * \brief	initialize the device (overloaded function for each devices)
	 * \fn 		virtual void	DoInputDeviceDescription()=0;
	 */
	void	DoInputDeviceDescription() override {;}

	/**
	 * \brief	update the device
	 * \fn 		virtual void	Update();
	 */
	void	UpdateDevice() override;

protected:
	/**
	 * \brief	destructor
	 * \fn 		~VirtualDevice();
	 */
	virtual ~VirtualDevice(); 
};

#endif //_VIRTUALDEVICE_H_
