#ifndef _FOG_H_
#define _FOG_H_

#include "Drawable.h"
#include "TecLibs/Tec3D.h"

#ifdef _3DSMAX

namespace KigsFramework
{

#endif

// ****************************************
// * Fog class
// * --------------------------------------
/**
 * \file	Fog.h
 * \class	Fog
 * \ingroup Drawable
 * \ingroup RendererDrawable
 * \brief	Base class for fog
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
 * CoreModifiable* fog=(CoreModifiable*)(KigsCore::GetInstanceOf("test","Fog"));<br>
 * </span>
 * <span class="comment"> Initialization :</span><br>
 * <span class="code">
 * kfloat fogColor[4] = {<i>R</i>, <i>G</i>, <i>B</i>, <i>A</i>};<br>
 * fog->setArrayValue("Color",fogColor, 4);<br>
 * fog->Init();
 * </span>
 * </dd></dl>
 *
 * <dl class="exported"><dt><b>Exported parameters :</b></dt><dd>
 * <table>
 * <tr><td>kfloat</td>	<td><strong>Color[4]</strong> :			</td><td>Fog color</td></tr>	
 * <tr><td>kfloat</td>	<td><strong>StartDistance</strong> :	</td><td>start fog distance</td></tr>	
 * <tr><td>kfloat</td>	<td><strong>EndDistance</strong> :		</td><td>end fog distance</td></tr>	
 * <tr><td>kfloat</td>	<td><strong>Density</strong> :			</td><td>fog density</td></tr>	
 * <tr><td>String</td>	<td><strong>Mode</strong> :				</td><td>rendering mode ("EXP","EXP2","LINEAR")</td></tr>	
 * </table>
 * </dd></dl>
 */
// ****************************************
class Fog : public Drawable 
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(Fog,Drawable,Renderer)

	/**
	 * \brief	constructor
	 * \fn 		Fog(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	Fog(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

protected:
	/**
	 * \brief	destructor
	 * \fn 		~Fog();
	 */
	virtual ~Fog(){;}

	//! fog color
	maVect4DF	myColor;
	//! start distance
	maFloat		myStartDist;
	//! end distance
	maFloat		myEndDist;
	//! fog density
	maFloat		myDensity;
	//! rendering mode
	maEnum<3>	myMode;
};

#ifdef _3DSMAX

} // namespace KigsFramework

#endif

#endif //_FOG_H_
