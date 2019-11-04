#include "PrecompiledHeaders.h"

#include "CoordinateSystem.h"
#include "TecLibs/Tec3D.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(CoordinateSystem, CoordinateSystem, SceneGraph);
IMPLEMENT_CLASS_INFO(CoordinateSystem)

IMPLEMENT_CONSTRUCTOR(CoordinateSystem)
, myPosX(*this,false,"PositionX",0.0f)
, myPosY(*this,false,"PositionY",0.0f)
, myPosZ(*this,false,"PositionZ",0.0f)
, myRotX(*this,false,"RotationX",0.0f)
, myRotY(*this,false,"RotationY",0.0f)
, myRotZ(*this,false,"RotationZ",0.0f)
, myScale(*this,false,"Scale",1.0f)
{
}     


void CoordinateSystem::setPosition(float posX, float posY, float posZ)
{
	myPosX = posX;
	myPosY = posY;
	myPosZ = posZ;
	Matrix3x4 matrix = GetLocal();
	matrix.SetTranslation({ posX, posY, posZ });
	ChangeMatrix(matrix);
}

void CoordinateSystem::getPosition(float& posX,float& posY,float& posZ) const
{
	posX = myPosX;
	posY = myPosY;
	posZ = myPosZ;
}

void CoordinateSystem::toEuler(float x,float y,float z,float angle,float& heading,float& attitude,float& bank) 
{
	float s=sinf(angle);
	float c=cosf(angle);
	float t=1.0f-c;
	//  if axis is not already normalised then uncomment this
	// double magnitude = Math.sqrt(x*x + y*y + z*z);
	// if (magnitude==0) throw error;
	// x /= magnitude;
	// y /= magnitude;
	// z /= magnitude;
	if ((x*y*t + z*s) > 0.998f) { // north pole singularity detected
		heading = 2.0f*atan2f(x*sinf(angle*0.5f),cosf(angle*0.5f));
		attitude = fPI*0.5f;
		bank = 0.0f;
		return;
	}
	if ((x*y*t + z*s) < -0.998f) { // south pole singularity detected
		heading = -2.0f*atan2f(x*sinf(angle*0.5f),cosf(angle*0.5f));
		attitude = -fPI*0.5f;
		bank = 0.0f;
		return;
	}
	heading = atan2f(y * s- x * z * t , 1.0f - (y*y+ z*z ) * t);
	attitude = asinf(x * y * t + z * s) ;
	bank = atan2f(x * s - y * z * t , 1.0f - (x*x + z*z) * t);
}

void	CoordinateSystem::setRotation(float rot1,float rot2,float rot3)
{
	myRotX = rot1;
	myRotY = rot2;
	myRotZ = rot3;
	Matrix3x4 matrix = GetLocal();
	matrix.SetRotationXYZ(rot1, rot2, rot3);
	matrix.PreScale(myScale, myScale, myScale);
	ChangeMatrix(matrix);
}

void CoordinateSystem::getRotation(float& rotX,float& rotY,float& rotZ) const
{
	rotX = myRotX;
	rotY = myRotY;
	rotZ = myRotZ;
}

void	CoordinateSystem::setScale(float sca)
{
	if(sca)
	{
		float updateScale=sca/(float)myScale;
		myScale = sca;
		Matrix3x4 matrix = GetLocal();
		matrix.PreScale(updateScale,updateScale,updateScale);
		ChangeMatrix(matrix);
	}
}

void	CoordinateSystem::InitModifiable()
{
	Node3D::InitModifiable();
	Matrix3x4 matrix = Matrix3x4::IdentityMatrix();
	matrix.SetTranslation(v3f{ myPosX, myPosY, myPosZ });
	matrix.SetRotationXYZ(myRotX, myRotY, myRotZ);
	matrix.PreScale(myScale, myScale, myScale);
	ChangeMatrix(matrix);

	myPosX.changeNotificationLevel(Owner);
	myPosY.changeNotificationLevel(Owner);
	myPosZ.changeNotificationLevel(Owner);
	myRotX.changeNotificationLevel(Owner);
	myRotY.changeNotificationLevel(Owner);
	myRotZ.changeNotificationLevel(Owner);
	myScale.changeNotificationLevel(Owner);
}

void CoordinateSystem::NotifyUpdate(const unsigned int  labelid )
{
	setPosition(myPosX,myPosY,myPosZ);
	setRotation(myRotX,myRotY,myRotZ);
	Node3D::NotifyUpdate(labelid);
}



