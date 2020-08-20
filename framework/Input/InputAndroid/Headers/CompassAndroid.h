#ifndef _CompassAndroid_H_
#define _CompassAndroid_H_

#include "CompassDevice.h"
#include "ModuleInputAndroid.h"

#include <jni.h>

// ****************************************
// * CompassAndroid class
// * --------------------------------------
/**
* \file	CompassAndroid.h
* \class	CompassAndroid
* \ingroup Input
* \brief Android compass management.
*
*/
// ****************************************

class	CompassAndroid : public CompassDevice
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

    DECLARE_CLASS_INFO(CompassAndroid,CompassDevice,Input)
	
    CompassAndroid(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	void	UpdateDevice() override;
	
	bool	Aquire() override;
	bool	Release() override;

	
	void	DoInputDeviceDescription() override;
	
    
protected:
	virtual ~CompassAndroid();  

	bool mIsAvailable;
	bool mIsRunning;


	void	Start() override;
	void	Stop() override;

	jmethodID 	mStopMethod;
	jmethodID 	mStartMethod;
	jmethodID 	mGetValue;
	jclass		mKigsCompass;
	
};    

#endif //_CompassAndroid_H_