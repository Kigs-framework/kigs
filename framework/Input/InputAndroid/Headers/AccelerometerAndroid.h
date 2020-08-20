#ifndef _ACCELEROMETERANDROID_H_
#define _ACCELEROMETERANDROID_H_

#include "AccelerometerDevice.h"
#include "ModuleInputAndroid.h"

#include <jni.h>

// ****************************************
// * AccelerometerAndroid class
// * --------------------------------------
/**
* \file	AccelerometerAndroid.h
* \class	AccelerometerAndroid
* \ingroup Input
* \brief Android Accelerometer management
*
*/
// ****************************************

class	AccelerometerAndroid : public AccelerometerDevice
{
public:
	/** get sensor data as fast as possible */
	const static int SENSOR_DELAY_FASTEST = 0;
	/** rate suitable for games */
	const static int SENSOR_DELAY_GAME = 1;
	/** rate suitable for the user interface  */
	const static int SENSOR_DELAY_UI = 2;
	/** rate (default) suitable for screen orientation changes */
	const static int SENSOR_DELAY_NORMAL = 3;

    DECLARE_CLASS_INFO(AccelerometerAndroid,AccelerometerDevice,Input)
	
    AccelerometerAndroid(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	void	UpdateDevice() override;
	
	bool	Aquire() override;
	bool	Release() override;

	
	void	DoInputDeviceDescription() override;
	
    
protected:
	virtual ~AccelerometerAndroid();  

	bool mIsAvailable;
	bool mIsWCAvailable;
	bool mIsRunning;

	void	Start() override;
	void	Stop() override;


	jmethodID 	mStopMethod;
	jmethodID 	mStartMethod;
	jmethodID 	mGetValue;

	jclass		mKigsAccelerometer;
	
};    

#endif //_ACCELEROMETERANDROID_H_