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
* \ingroup Renderer
* \brief	Base class, generic light object.
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
		mDiffuseColor[0]=r;
		mDiffuseColor[1]=g;
		mDiffuseColor[2]=b;
		NotifyUpdate(mDiffuseColor.getLabelID()._id);
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
		mSpecularColor[0]=r;
		mSpecularColor[1]=g;
		mSpecularColor[2]=b;
		NotifyUpdate(mSpecularColor.getLabelID()._id);
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
		mAmbientColor[0]=r;
		mAmbientColor[1]=g;
		mAmbientColor[2]=b;
		NotifyUpdate(mAmbientColor.getLabelID()._id);
	}

	inline void setIsOn(bool a_value) { mIsOn = a_value; }
	inline bool getIsOn() const { return mIsOn; }

protected:

	void InitModifiable() override;

	/**
	 * \brief	destructor
	 * \fn 		~Light();
	 */
	virtual ~Light();  
   
	//! specular color
	maVect3DF	mSpecularColor;
	//! ambient color
	maVect3DF	mAmbientColor;
	//! diffuse color
	maVect3DF	mDiffuseColor;
	//! spot attenuation
	maFloat		mSpotAttenuation;
	//! spot cut off
	maFloat		mSpotCutOff;
	//! attenuation constante
	maFloat		mConstAttenuation;
	//! attenuation linear
	maFloat		mLinAttenuation;
	//! attenuation quad
	maFloat		mQuadAttenuation;
  
	//! TRUE if the light is on
	maBool		mIsOn;

	//0 for point, 1 for directional, 2 for spot
	maEnum<3>	mLightType;

}; 

#ifdef _3DSMAX

} // namespace KigsFramework

#endif

#endif //_LIGHT_H_
