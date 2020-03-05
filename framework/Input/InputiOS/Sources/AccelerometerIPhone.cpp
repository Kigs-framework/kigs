/*
 *  AccelerometerIPhone.cpp
 *  IPhoneInputIPhone
 *
 *  Created by steph on 4/4/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "AccelerometerIPhone.h"
#include "Core.h"
#include "DeviceItem.h"
#include <pthread.h>

/**** accelerometre ****/
extern float	newAcc[3];
/***********************/

extern pthread_mutex_t	eventMutex;

IMPLEMENT_CLASS_INFO(AccelerometerIPhone)

AccelerometerIPhone::AccelerometerIPhone(const kstl::string& name,CLASS_NAME_TREE_ARG) : AccelerometerDevice(name,PASS_CLASS_NAME_TREE_ARG)
{
	myAccX = 0.0;
	myAccY = 0.0;
	myAccZ = 0.0;
}

AccelerometerIPhone::~AccelerometerIPhone()
{  
	
}  

bool	AccelerometerIPhone::Aquire()
{
	if (AccelerometerDevice::Aquire())
	{
		Start();
		return true;
	}
	return false;
}

bool	AccelerometerIPhone::Release()
{
	if (AccelerometerDevice::Release())
	{
		Stop();
		return true;
	}
	return false;
}

void	AccelerometerIPhone::UpdateDevice()
{
	pthread_mutex_lock( &eventMutex );
	
	kfloat norm=sqrtf( (newAcc[0] - myAccX) * (newAcc[0] - myAccX) +
					  (newAcc[1] - myAccY) * (newAcc[1] - myAccY) +
					  (newAcc[2] - myAccZ) * (newAcc[2] - myAccZ) );
	
	if(norm>1.0f)
	{
		norm=1.0f;
	}
	
	myAccX = newAcc[0] * norm + myAccX * (1.0f - norm);
	myAccY = newAcc[1] * norm + myAccY * (1.0f - norm);
	myAccZ = newAcc[2] * norm + myAccZ * (1.0f - norm);
	
	
	
	pthread_mutex_unlock( &eventMutex );
}

void	AccelerometerIPhone::DoInputDeviceDescription()
{
	
	
	
}

void	AccelerometerIPhone::Start()
{
	// TODO
}
void	AccelerometerIPhone::Stop()
{
	// TODO
}