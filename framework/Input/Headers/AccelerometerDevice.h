#ifndef _ACCELEROMETERDEVICE_H_
#define _ACCELEROMETERDEVICE_H_

#include "InputDevice.h"

// ****************************************
// * AccelerometerDevice class
// * --------------------------------------
/**
* \file	AccelerometerDevice.h
* \class	AccelerometerDevice
* \ingroup Input
* \brief Base class for accelerometer
*
*/
// ****************************************

class AccelerometerDevice : public InputDevice
{
public:
	//! buttons list
	
	
	DECLARE_ABSTRACT_CLASS_INFO(AccelerometerDevice,InputDevice,Input)
	
	/**
	 * \brief	constructor
	 * \fn 		MouseDevice(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	AccelerometerDevice(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	

	/**
	 * \brief	retreive current position
	 * \fn 		void			getPos(kfloat& posX,kfloat& posY)
	 * \param	posX : position on x axis (in/out param)
	 * \param	posY : position on y axis (in/out param)
	 */
	void			getPos(kfloat& posX,kfloat& posY,kfloat& posZ){posX=mAccX; posY=mAccY; posZ=mAccZ;}
	
	
	/**
	 * \brief	compute current position
	 * \fn 		virtual void	Update();
	 */
	void	UpdateDevice() override;
	
protected:
	/**
	 * \brief	destructor
	 * \fn 		~MouseDevice();
	 */
	virtual ~AccelerometerDevice(); 
	
	virtual void	Start() = 0;
	virtual void	Stop() = 0;

	// world coordinate acceleration
	maFloat	mAccX;
	maFloat	mAccY;
	maFloat	mAccZ;

	// device coordinate acceleration
	maFloat	mRawAccX;
	maFloat	mRawAccY;
	maFloat	mRawAccZ;
	
	// world coordinate integrated velocity
	maFloat	mVelX;
	maFloat	mVelY;
	maFloat	mVelZ;

	// world coordinate integrated position
	maFloat	mPosX;
	maFloat	mPosY;
	maFloat	mPosZ;

	maInt	mRate;

};

#endif //_ACCELEROMETERDEVICE_H_