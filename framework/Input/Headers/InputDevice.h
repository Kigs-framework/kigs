#ifndef _INPUTDEVICE_H_
#define _INPUTDEVICE_H_

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"

class DeviceItem;
class Window;

// ****************************************
// * InputDevice class
// * --------------------------------------
/**
 * \file	InputDevice.h
 * \class	InputDevice
 * \ingroup Input
 * \brief	base class for all input devices
 * \author	ukn
 * \version ukn
 * \date	ukn
 */
// ****************************************
class InputDevice : public CoreModifiable
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(InputDevice, CoreModifiable, Input);
	DECLARE_CONSTRUCTOR(InputDevice);

	/**
	 * \brief	initialize the device (overloaded function for each devices)
	 * \fn 		virtual void	DoInputDeviceDescription()=0;
	 */
	virtual void	DoInputDeviceDescription()=0;

	/**
	 * \brief	initialize all the items
	 * \fn 		void	InitItems(unsigned int itemCount,DeviceItem** items);
	 * \param	itemCount : number of items
	 * \param	items : list of items
	 */
	void	InitItems(unsigned int itemCount,DeviceItem** items);

	/**
	 * \brief	set aquire to TRUE
	 * \fn 		virtual bool	Aquire()
	 * \return	TRUE if not already Aquired
	 */
	virtual bool	Aquire()
	{
		myAquireCount++;
		return (myAquireCount==1);
	}

	/**
	 * \brief	set aquire old value
	 * \fn 		virtual bool	Release()
	 * \return	TRUE if really released
	 */
	virtual bool	Release()
	{
		if(myAquireCount>0)
		{
			myAquireCount--;
			return (myAquireCount == 0);
		}
		return false;
	}

	/**
	 * \brief	get aquire value
	 * \fn 		bool	IsAquired()
	 * \return	aquire value
	 */
	bool	IsAquired(){return (myAquireCount>0);}

	/**
	 * \brief	update the device
	 * \fn 		virtual void	Update();
	 */
	virtual void	UpdateDevice()=0;
	//using CoreModifiable::Update;

	/**
	 * \brief	call with a pointer of a Window to set the input window (mandatory)
	 * \fn 		virtual bool	addItem(CoreModifiable *item, ItemPosition pos=Last DECLARE_DEFAULT_LINK_NAME);
	 * \param	item : item to add
	 * \param	linkName : name of the added item
	 * \return	TRUE if the item is added, FALSE otherwise
	 */
	bool addItem(CoreModifiable *item, ItemPosition pos=Last DECLARE_DEFAULT_LINK_NAME) override;
	
	/**
	 * \brief	call with a pointer of a Window previously added to remove it
	 * \fn 		virtual bool	removeItem(CoreModifiable* item DECLARE_DEFAULT_LINK_NAME);
	 * \param	item : item to remove
	 * \param	linkName : name of the removed item
	 * \return	TRUE if the item is removed, FALSE otherwise
	 */
	bool removeItem(CoreModifiable *item DECLARE_DEFAULT_LINK_NAME) override;

	/**
	 * \brief	retreive given item state
	 * \fn 		kfloat	GetItemfState(int itemindex)
	 * \param	itemindex : index of the item
	 * \return	state of the item in float
	 */
	kfloat	GetItemfState(int itemindex);
	
	/**
	 * \brief	retreive given item state
	 * \fn 		int		GetItemiState(int itemindex)
	 * \param	itemindex : index of the item
	 * \return	state of the item in int
	 */
	int		GetItemiState(int itemindex);

	/**
	 * \brief	item count for this joystick
	 * \fn 		unsigned int GetItemsCount()
	 * \return	the number of item
	 */
	unsigned int GetItemsCount(){return myDeviceItemsCount;}
protected:
	/**
	 * \brief	destructor
	 * \fn 		~InputDevice();
	 */
	virtual ~InputDevice();

	//! list of devices
	DeviceItem**		myDeviceItems;
	//! number of devices
	unsigned int		myDeviceItemsCount;
	//! >0 if acquire (started) 
	unsigned int		myAquireCount;
	//! link to the input windows
	Window*				myInputWindow;
};

#endif //_INPUTDEVICE_H_
