#ifndef _DEVICEITEM_H_
#define _DEVICEITEM_H_

#include "CoreModifiable.h"
#include "TecLibs/Tec3D.h"

#define GetTypedValue(a)   getValueMethod((a*)0)
#define DECL_VP_TypeValue(a)	virtual a getValueMethod(a*) const =0; \
								virtual void SetValue(a)     =0;
#define DECL_TypeValue(a)	virtual a getValueMethod(a*) const {assert(0);return *(a*)0;} \
							virtual void SetValue(a)    {assert(0);return;}

// ****************************************
// * DeviceItemBaseState class
// * --------------------------------------
/**
 * \file	DeviceItem.h
 * \class	DeviceItemBaseState
 * \ingroup Input
 * \brief	abstract base class for device item state
 * \author	ukn
 * \version ukn
 * \date	ukn
 */
// ****************************************
class DeviceItemBaseState
{
public:
	friend class DeviceItem;

	/**
	 * \brief	coppy this DeviceItemBaseState
	 * \fn 		virtual DeviceItemBaseState*	MakeCopy()const =0 ;
	 * \return	the copy of this DeviceItemBaseState
	 */
	virtual DeviceItemBaseState*	MakeCopy()const =0 ;

	DECL_VP_TypeValue(float);
	DECL_VP_TypeValue(int);
	DECL_VP_TypeValue(Point3D);

	/**
	 * \brief	destructor
	 * \fn 		~DeviceItemBaseState();
	 */
	virtual ~DeviceItemBaseState(){};

protected:
	/**
	 * \brief	constructor
	 * \fn 		DeviceItemBaseState();
	 */
	DeviceItemBaseState(){};
};

// ****************************************
// * DeviceItemState class
// * --------------------------------------
/**
 * \class	DeviceItemState
 * \ingroup Input
 * \brief	base class for typed device item state
 * \author	ukn
 * \version ukn
 * \date	ukn
 */
// ****************************************
template<typename valueType>
class DeviceItemState : public DeviceItemBaseState
{
public:
	friend class DeviceItem;

	/**
	 * \brief	constructor
	 * \fn 		DeviceItemState(valueType value) 
	 * \param	value : state of the device
	 */
	DeviceItemState(valueType value) : myValue(value)
	{
	}

	/**
	 * \brief	destructor
	 * \fn 		~DeviceItemState();
	 */
	virtual ~DeviceItemState()
	{
	}

	/**
	 * \brief	copy a DeviceItemBaseState
	 * \fn 		DeviceItemBaseState*	MakeCopy() const
	 * \return	a copy of the DeviceItemBaseState
	 */
	DeviceItemBaseState*	MakeCopy() const
	{
		DeviceItemState<valueType>*	localstate=new DeviceItemState<valueType>(myValue);
		return localstate;
	}

	DECL_TypeValue(float);
	DECL_TypeValue(int);
	DECL_TypeValue(Point3D);

protected:
	//! state of the device
	valueType	myValue; 
};

template<>
inline float DeviceItemState<float>::getValueMethod(float*) const {return myValue;}
template<>
inline int   DeviceItemState<float>::getValueMethod(int*)   const {return (int)myValue;}
template<>
inline void  DeviceItemState<float>::SetValue(float v) {myValue = v;}
template<>
inline void  DeviceItemState<float>::SetValue(int v)   {myValue = (float)v;}

template<>
inline float DeviceItemState<int>::getValueMethod(float*)  const {return (float)myValue;}
template<>
inline int   DeviceItemState<int>::getValueMethod(int*)    const {return myValue;}
template<>
inline void  DeviceItemState<int>::SetValue(float v) {myValue = (int)v;}
template<>
inline void  DeviceItemState<int>::SetValue(int v)   {myValue = v;}

template<>
inline Point3D DeviceItemState<Point3D>::getValueMethod(Point3D*)  const {return myValue;}
template<>
inline void  DeviceItemState<Point3D>::SetValue(Point3D v)   {myValue = v;}

// ****************************************
// * DeviceItem class
// * --------------------------------------
/**
 * \class	DeviceItem
 * \ingroup Input
 * \brief	manage device item
 * \author	ukn
 * \version ukn
 * \date	ukn
 */
// ****************************************
class DeviceItem 
{
	friend class InputDevice;
public:
	/**
	 * \brief	constructor
	 * \fn 		DeviceItem(const DeviceItemBaseState& state);
	 * \param	state : states of the device
	 */
	DeviceItem(const DeviceItemBaseState& state);

	/**
	 * \brief	constructor by copy
	 * \fn 		DeviceItem(const DeviceItem& item)
	 * \param	item : DeviceItem to copy
	 */
	DeviceItem(const DeviceItem& item)
	{
		myStates=item.myStates->MakeCopy();
	}

	/**
	 * \brief	coppy this deviceItem
	 * \fn 		DeviceItem*	MakeCopy() const
	 * \return	the copy of this deviceItem
	 */
	DeviceItem*	MakeCopy() const
	{
		DeviceItem*	localitem=new DeviceItem(*myStates);
		return localitem;
	}

	/**
	 * \brief	destructor
	 * \fn 		~DeviceItem();
	 */
	virtual ~DeviceItem();  

	/**
	 * \brief	get the states of the device
	 * \fn 		inline DeviceItemBaseState*	getState()
	 * \return	a lilst of DeviceItemBaseState
	 */
	inline DeviceItemBaseState*	getState(){return myStates; }

protected:
	//! list of state
	DeviceItemBaseState*	myStates;
};	


/**
 * \brief	constructor by copy
 * \fn 		inline DeviceItem::DeviceItem(const DeviceItemBaseState& state) 
 * \param	item : DeviceItem to copy
 */
inline DeviceItem::DeviceItem(const DeviceItemBaseState& state) 
{
	myStates=state.MakeCopy();	
}

/**
 * \brief	destructor
 * \fn 		inline DeviceItem::~DeviceItem()
 */
inline DeviceItem::~DeviceItem()
{
	delete myStates;
}

#endif //_DEVICEITEM_H_
