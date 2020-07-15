#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "Node3D.h"
#include "maReference.h"
#include "TecLibs/Tec3D.h"

#ifdef _3DSMAX

namespace KigsFramework
{

#endif

// ****************************************
// * Light class
// * --------------------------------------
/**
 * \file	Light.h
 * \class	Light
 * \brief	base for a light object
 * \author	ukn
 * \version ukn
 * \date	ukn
 * 
 */
// ****************************************
class Light : public Node3D 
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(Light, Node3D,Renderer)

	/**
	 * \brief	constructor
	 * \fn 		Light(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
    Light(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	/**
	 * \brief	set the diffuse color
	 * \fn 		void	SetDiffuseColor(kfloat r,kfloat g,kfloat b,kfloat a)
	 * \param	r : red color
	 * \param	g : green color
	 * \param	b : blue color
	 * \param	a : alpha value
	 */
	void	SetDiffuseColor(kfloat r,kfloat g,kfloat b)
	{
		myDiffuseColor[0]=r;
		myDiffuseColor[1]=g;
		myDiffuseColor[2]=b;
		NotifyUpdate(myDiffuseColor.getLabelID()._id);
	}

	/**
	 * \brief	set the specular color
	 * \fn 		void	SetSpecularColor(kfloat r,kfloat g,kfloat b,kfloat a)
	 * \param	r : red color
	 * \param	g : green color
	 * \param	b : blue color
	 * \param	a : alpha value
	 */
	void	SetSpecularColor(kfloat r,kfloat g,kfloat b)
	{
		mySpecularColor[0]=r;
		mySpecularColor[1]=g;
		mySpecularColor[2]=b;
		NotifyUpdate(mySpecularColor.getLabelID()._id);
	}

	/**
	 * \brief	set the ambient color
	 * \fn 		void	SetAmbientColor(kfloat r,kfloat g,kfloat b,kfloat a)
	 * \param	r : red color
	 * \param	g : green color
	 * \param	b : blue color
	 * \param	a : alpha value
	 */
	void	SetAmbientColor(kfloat r,kfloat g,kfloat b)
	{
		myAmbientColor[0]=r;
		myAmbientColor[1]=g;
		myAmbientColor[2]=b;
		NotifyUpdate(myAmbientColor.getLabelID()._id);
	}

	inline void setIsOn(bool a_value) { myIsOn = a_value; }
	inline bool getIsOn() const { return myIsOn; }

protected:

	void InitModifiable() override;

	/**
	 * \brief	destructor
	 * \fn 		~Light();
	 */
	virtual ~Light();  
   
	//! specular color
	maVect3DF	mySpecularColor;
	//! ambient color
	maVect3DF	myAmbientColor;
	//! diffuse color
	maVect3DF	myDiffuseColor;
	//! spot attenuation
	maFloat		mySpotAttenuation;
	//! spot cut off
	maFloat		mySpotCutOff;
	//! attenuation constante
	maFloat		myConstAttenuation;
	//! attenuation linear
	maFloat		myLinAttenuation;
	//! attenuation quad
	maFloat		myQuadAttenuation;
  
	//! TRUE if the light is on
	maBool		myIsOn;

	//0 for point, 1 for directional, 2 for spot
	maEnum<3>	myLightType;

}; 

#ifdef _3DSMAX

} // namespace KigsFramework

#endif

#endif //_LIGHT_H_
