#ifndef _GyroscopeANDROID_H_
#define _GyroscopeANDROID_H_

#include "GyroscopeDevice.h"
#include "ModuleInputAndroid.h"

#include <jni.h>
class	GyroscopeAndroid : public GyroscopeDevice
{
public:
    DECLARE_CLASS_INFO(GyroscopeAndroid,GyroscopeDevice,Input)
	
    GyroscopeAndroid(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	void	UpdateDevice() override;
	
	bool	Aquire() override;
	bool	Release() override;


	void	DoInputDeviceDescription() override;
	
    
protected:
	virtual ~GyroscopeAndroid();  

	bool	isAvailable;
	bool	isRunning;

	void	Start() override;
	void	Stop() override;

	 
	/*jmethodID 	getVelX;
	jmethodID 	getVelY;
	jmethodID 	getVelZ;

	jmethodID	getMatrixElem;
	jmethodID	getQuatElem;*/

	jmethodID 	StopMethod;
	jmethodID 	StartMethod;
	jmethodID 	getVelocity;
	jmethodID 	getQuat;
	jclass		myKigsGyroscope;
	
};    

#endif //_GyroscopeANDROID_H_