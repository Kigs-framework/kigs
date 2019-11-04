#ifndef _COORDINATESYSTEM_H_
#define _COORDINATESYSTEM_H_

#include "Node3D.h"

class CoordinateSystem : public Node3D
{
public:
	DECLARE_CLASS_INFO(CoordinateSystem,Node3D,SceneGraph)
	DECLARE_CONSTRUCTOR(CoordinateSystem)


	v3f GetPosition() const { v3f result; getPosition(result.x, result.y, result.z); return result; }
	void SetPosition(v3f pos) { setPosition(pos.x, pos.y, pos.z); }
	void setPosition(float posX,float posY,float posZ) ;
	void getPosition(float& posX,float& posY,float& posZ) const;
	
	v3f GetRotation() const { v3f result; getRotation(result.x, result.y, result.z); return result; }
	void SetRotation(v3f pos) { setRotation(pos.x, pos.y, pos.z); }
	void setRotation(float rot1,float rot2,float rot3);
	void getRotation(float& rotX,float& rotY,float& rotZ) const;
	
	void setScale(float sca);
	float GetScale() const { return myScale; }


	//bool HasScale(float & sca) override {sca=myScale; return true;}
	void toEuler(float x, float y, float z, float angle, float& heading, float& attitude, float& bank);

protected:
	void	InitModifiable() override;
	void NotifyUpdate(const unsigned int /* labelid */) override;

	maFloat	myPosX;
	maFloat	myPosY;
	maFloat	myPosZ;

	maFloat	myRotX;
	maFloat	myRotY;
	maFloat	myRotZ;

	//! scale
	maFloat myScale;
}; 

#endif //_COORDINATESYSTEM_H_
