#ifndef _GyroscopeDEVICE_H_
#define _GyroscopeDEVICE_H_

#include "InputDevice.h"

class GyroscopeDevice : public InputDevice
{
public:
	//! buttons list
	
	
	DECLARE_ABSTRACT_CLASS_INFO(GyroscopeDevice,InputDevice,Input)
	
	/**
	 * \brief	constructor
	 * \fn 		MouseDevice(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	GyroscopeDevice(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	

	/**
	 * \brief	retreive current rotation matrix
	 * \fn 		void			getPos(kfloat* mat )
	 * \param	posX : position on x axis (in/out param)
	 * \param	posY : position on y axis (in/out param)
	 */
	void			getRotationVelocity(kfloat& X, kfloat& Y, kfloat& Z){ X = myRotationVelocity[0]; Y = myRotationVelocity[1]; Z = myRotationVelocity[2]; }

	void			getRotationQuaternion(kfloat& x, kfloat& y, kfloat& z, kfloat& w) {
		x = myQuaternion[0];
		y = myQuaternion[1];
		z = myQuaternion[2];
		w = myQuaternion[3];
	}
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
	virtual ~GyroscopeDevice(); 
	
	virtual void	Start() = 0;
	virtual void	Stop() = 0;

	
	maVect3DF	myRotationVelocity;
	maVect4DF   myQuaternion;

	maInt		myRate;

};

#endif //_GyroscopeDEVICE_H_