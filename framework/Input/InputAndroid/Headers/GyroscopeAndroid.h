#ifndef _GyroscopeANDROID_H_
#define _GyroscopeANDROID_H_

#include "GyroscopeDevice.h"
#include "ModuleInputAndroid.h"

#include <jni.h>

// ****************************************
// * GyroscopeAndroid class
// * --------------------------------------
/**
* \file	GyroscopeAndroid.h
* \class	GyroscopeAndroid
* \ingroup Input
* \brief Android Gyroscope management.
*
*/
// ****************************************
class	GyroscopeAndroid : public GyroscopeDevice
{
public:
    DECLARE_CLASS_INFO(GyroscopeAndroid,GyroscopeDevice,Input)
	
    GyroscopeAndroid(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	void	UpdateDevice() override;
	
	bool	Aquire() override;
	bool	Release() override;


	void	DoInputDeviceDescription() override;
		virtual ~GyroscopeAndroid();  

    
protected:

	bool	mIsAvailable;
	bool	mIsRunning;

	void	Start() override;
	void	Stop() override;


	jmethodID 	mStopMethod;
	jmethodID 	mStartMethod;
	jmethodID 	mGetVelocity;
	jmethodID 	mGetQuat;
	jclass		mKigsGyroscope;
	
};    

#endif //_GyroscopeANDROID_H_