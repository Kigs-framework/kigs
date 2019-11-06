#ifndef _ACCELEROMETERDEVICE_H_
#define _ACCELEROMETERDEVICE_H_

#include "InputDevice.h"

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
	void			getPos(kfloat& posX,kfloat& posY,kfloat& posZ){posX=myAccX; posY=myAccY; posZ=myAccZ;}
	
	
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
	maFloat	myAccX;
	maFloat	myAccY;
	maFloat	myAccZ;

	// device coordinate acceleration
	maFloat	myRawAccX;
	maFloat	myRawAccY;
	maFloat	myRawAccZ;
	
	// world coordinate integrated velocity
	maFloat	myVelX;
	maFloat	myVelY;
	maFloat	myVelZ;

	// world coordinate integrated position
	maFloat	myPosX;
	maFloat	myPosY;
	maFloat	myPosZ;

	maInt	myRate;

};

#endif //_ACCELEROMETERDEVICE_H_