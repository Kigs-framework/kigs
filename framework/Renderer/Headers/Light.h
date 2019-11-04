#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "Drawable.h"
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
 * \ingroup Drawable
 * \ingroup RendererDrawable
 * \brief	base for a light object
 * \author	ukn
 * \version ukn
 * \date	ukn
 * 
 * <dl class="dependency"><dt><b>Dependency:</b></dt><dd>ModuleRenderer</dd></dl>
 * <dl class="exemple"><dt><b>Exemple:</b></dt><dd>
 * <span class="comment"> Load the module :</span><br>
 * <span class="code">
 * theRenderer=new ModuleRenderer(<i>instance_name</i>);<br>
 * theRenderer->Init(KigsCore::Instance(),0);<br>
 * </span>
 * <span class="comment"> Create the object :</span><br>
 * <span class="code">
 * Light*	light=(Light*)(KigsCore::GetInstanceOf(<i>instance_name</i>,"Light"));<br>
 * </span>
 * <span class="comment"> Initialization :</span><br>
 * <span class="code">
 * light->Init();<br>
 * </span>
 * </dd></dl>
 *
 * <dl class="exported"><dt><b>Exported parameters :</b></dt><dd>
 * <table>
 * <tr><td>kfloat</td>	<td><strong>SpecularColor[4]</strong> :</td>	<td>specular color</td></tr>	
 * <tr><td>kfloat</td>	<td><strong>AmbientColor[4]</strong> :</td>		<td>ambient color</td></tr>
 * <tr><td>kfloat</td>	<td><strong>DiffuseColor[4]</strong> :</td>		<td>diffuse color</td></tr>
 * <tr><td>kfloat</td>	<td><strong>Position[3]</strong> :</td>			<td>spot position</td></tr>
 * <tr><td>kfloat</td>	<td><strong>SpotDirection[3]</strong> :</td>	<td>spot direction</td></tr>
 * <tr><td>kfloat</td>	<td><strong>SpotAttenuation</strong> :</td>		<td>spot attenuation</td></tr>
 * <tr><td>kfloat</td>	<td><strong>SpotCutOff</strong> :</td>			<td>spot cut off</td></tr>
 * <tr><td>kfloat</td>	<td><strong>ConstAttenuation</strong> :</td>	<td>constante attenuation</td></tr>
 * <tr><td>kfloat</td>	<td><strong>LinAttenuation</strong> :</td>		<td>linear attenuation</td></tr>
 * <tr><td>kfloat</td>	<td><strong>QuadAttenuation</strong> :</td>		<td>quad attenuation</td></tr>
 * <tr><td>bool</td>	<td><strong>IsOn</strong> :</td>				<td>TRUE if the light is on</td></tr>
 * <tr><td>bool</td>	<td><strong>IsDirectional</strong> :</td>		<td>TRUE if the light is directional</td></tr>
 * </table>
 * </dd></dl>
 */
// ****************************************
class Light : public Drawable 
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(Light,Drawable,Renderer)

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
	void	SetDiffuseColor(kfloat r,kfloat g,kfloat b,kfloat a)
	{
		myDiffuseColor[0]=r;
		myDiffuseColor[1]=g;
		myDiffuseColor[2]=b;
		myDiffuseColor[3]=a;
	}

	/**
	 * \brief	set the specular color
	 * \fn 		void	SetSpecularColor(kfloat r,kfloat g,kfloat b,kfloat a)
	 * \param	r : red color
	 * \param	g : green color
	 * \param	b : blue color
	 * \param	a : alpha value
	 */
	void	SetSpecularColor(kfloat r,kfloat g,kfloat b,kfloat a)
	{
		mySpecularColor[0]=r;
		mySpecularColor[1]=g;
		mySpecularColor[2]=b;
		mySpecularColor[3]=a;
	}

	/**
	 * \brief	set the ambient color
	 * \fn 		void	SetAmbientColor(kfloat r,kfloat g,kfloat b,kfloat a)
	 * \param	r : red color
	 * \param	g : green color
	 * \param	b : blue color
	 * \param	a : alpha value
	 */
	void	SetAmbientColor(kfloat r,kfloat g,kfloat b,kfloat a)
	{
		myAmbientColor[0]=r;
		myAmbientColor[1]=g;
		myAmbientColor[2]=b;
		myAmbientColor[3]=a;
	}

	/**
	 * \brief	set the position
	 * \fn 		void	SetPosition(kfloat x,kfloat y,kfloat z)
	 * \param	x : coordinate on x axis
	 * \param	y : coordinate on y axis
	 * \param	z : coordinate on z axis
	 */
	void	SetPosition(kfloat x,kfloat y,kfloat z)
	{
		myPosition[0]=x;
		myPosition[1]=y;
		myPosition[2]=z;
		//myPosition[3]=KFLOAT_CONST(1.0);
	}

	/**
	 * \brief	set the spot direction
	 * \fn 		void	SetSpotDirection(kfloat x,kfloat y,kfloat z)
	 * \param	x : coordinate on x axis
	 * \param	y : coordinate on y axis
	 * \param	z : coordinate on z axis
	 */
	void	SetSpotDirection(kfloat a_x,kfloat a_y, kfloat a_z)
	{
		mySpotDirection[0] = a_x;
		mySpotDirection[1] = a_y;
		mySpotDirection[2] = a_z;
	}

	/**
	 * \brief	specify if the light is directional or not
	 * \fn 		void	SetDirectional(bool isdir)
	 * \param	isdir : TRUE if the light is directional, FALSE otherwise
	 */
	void	SetDirectional(bool isdir)
	{
		myIsDirectional=isdir;
	}
          
	unsigned int	GetSelfDrawingNeeds() override
	{
#ifdef _DEBUG
		return ((unsigned int)Need_Predraw)|((unsigned int)Need_Draw)|((unsigned int)Need_Postdraw);
#else
		return ((unsigned int)Need_Predraw)|((unsigned int)Need_Postdraw);
#endif
	}

	inline void setIsOn(bool a_value){myIsOn = a_value;}


protected:
	/**
	 * \brief	initialise pre draw method
	 * \fn 		virtual bool PreDraw(TravState*);
	 * \param	travstate : camera state
	 * \return	TRUE if a could PreDraw
	 */
	bool	PreDraw(TravState*) override;

	/**
	 * \brief	initialise PostDraw method
	 * \fn 		virtual bool PostDraw(TravState*);
	 * \param	travstate : camera state
	 * \return	TRUE if a could PostDraw
	 */
	bool	PostDraw(TravState* travstate) override;
	
	/**
	 * \brief	destructor
	 * \fn 		~Light();
	 */
	virtual ~Light();  
   
	//! specular color
	maVect4DF	mySpecularColor;
	//! ambient color
	maVect4DF	myAmbientColor;
	//! diffuse color
	maVect4DF	myDiffuseColor;
	//! spot position
	maVect4DF	myPosition;
	//! spot direction
	maVect3DF	mySpotDirection;
	//! spot attenuation
	maFloat		mySpotAttentuation;
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
	//! TRUE if the light is directional
	maBool		myIsDirectional;

	maReference			myPositionNode3D;

	//! index of light
	unsigned int	myLightIndex;
	//! list off available light type
	static	bool	myAvailableLightArray[8];
}; 

#ifdef _3DSMAX

} // namespace KigsFramework

#endif

#endif //_LIGHT_H_
