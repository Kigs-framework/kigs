#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "Drawable.h"
#include "TravState.h"
#include "SceneGraphDefines.h"

#include "MaterialStage.h"
#include "ModuleRenderer.h"


#ifdef _3DSMAX

namespace KigsFramework
{

#endif
	
/*! 
 * \defgroup Maretial Internal material
 * \ingroup Renderer
 * Internal material classes
*/

// ****************************************
// * Material class
// * --------------------------------------
/**
 * \file	Material.h
 * \class	Material
 * \ingroup Renderer
 * \brief	material class
 * \author	ukn
 * \version ukn
 * \date	ukn
 *
 * Exported parameters :<br>
 * <ul>
 * <li>
 *		int <strong>Facing</strong> :
 * </li>
 * <li>
 *		int <strong>BlendFuncSource</strong> : blend function source type
 * </li>
 * <li>
 *		int <strong>BlendFuncDest</strong> : blend function destination type
 * </li>
 * <li>
 *		bool <strong>BlendEnabled</strong> : TRUE to enable blending
 * </li>
 * <li>
 *		bool <strong>MaterialColorEnabled</strong> : TRUE to enable material color
 * </li>
 * <li>
 *		kfloat <strong>AmbientColor[4]</strong> : ambient color
 * </li>
 * <li>
 *		kfloat <strong>DiffuseColor[4]</strong> : diffuse color
 * </li>
 * <li>
 *		kfloat <strong>SpecularColor[4]</strong> : specular color
 * </li>
 * <li>
 *		kfloat <strong>EmissionColor[4]</strong> : emissive color
 * </li>
 * <li>
 *		kfloat <strong>Shininess</strong> : shininess
 * </li>
 * <li>
 *		kfloat <strong>Transparency</strong> : transparency
 * </li>
 * </ul>
 */
// ****************************************
class Material : public Drawable 
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(Material,Drawable,Renderer)
	
	/**
	 * \brief	constructor
	 * \fn 		Material(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
    Material(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);


	/**
	 * \brief	initialise pre draw method
	 * \fn 		virtual bool PreDraw(TravState*);
	 * \param	travstate : camera state
	 * \return	TRUE if a could PreDraw
	 */
	bool	PreDraw(TravState* travstate) override
	{
		if (travstate->myCurrentMaterial == this)
			return false;
		
		return Drawable::PreDraw(travstate);
	}

	/**
	 * \brief	pre draw method
	 * \fn 		virtual void	DoPreDraw(TravState*);
	 * \param	TravState : camera state
	 */
	void	DoPreDraw(TravState* travstate) override;
	
	/**
	 * \brief	post draw method
	 * \fn 		virtual void	DoPostDraw(TravState* travstate);
	 * \param	travstate : camera state
	 */
	void	DoPostDraw(TravState* travstate) override;

	/**
	 * \brief	set the ambient color
	 * \fn 		void	SetAmbientColor(kfloat r,kfloat g,kfloat b,kfloat a=KFLOAT_CONST(1.0f))
	 * \param	r : red color
	 * \param	g : green color
	 * \param	b : blue color
	 * \param	a : alpha value
	 */
	void	SetAmbientColor(kfloat r,kfloat g,kfloat b,kfloat a=KFLOAT_CONST(1.0f))
	{
		myAmbientColor[0]=r;
		myAmbientColor[1]=g;
		myAmbientColor[2]=b;
		myAmbientColor[3]=a;
	}

	/**
	 * \brief	get the ambient color
	 * \fn 		void	GetAmbientColor(kfloat& r,kfloat& g,kfloat& b)
	 * \param	r : red color (in/out param)
	 * \param	g : green color (in/out param)
	 * \param	b : blue color (in/out param)
	 */
	void	GetAmbientColor(kfloat& r,kfloat& g,kfloat& b)
	{
		r=myAmbientColor[0];
		g=myAmbientColor[1];
		b=myAmbientColor[2];
	}

	/**
	 * \brief	set the diffuse color
	 * \fn 		void	SetDiffuseColor(kfloat r,kfloat g,kfloat b,kfloat a=KFLOAT_CONST(-1.0f))
	 * \param	r : red color
	 * \param	g : green color
	 * \param	b : blue color
	 * \param	a : alpha value
	 */
	void	SetDiffuseColor(kfloat r,kfloat g,kfloat b,kfloat a=KFLOAT_CONST(-1.0f))
	{
		if(a != KFLOAT_CONST(-1.0))
		{
			myTransparency=a;
		}
		myDiffuseColor[0]=r;
		myDiffuseColor[1]=g;
		myDiffuseColor[2]=b;
		myDiffuseColor[3]=myTransparency;
	}

	/**
	 * \brief	get the diffuse color
	 * \fn 		void	GetDiffuseColor(kfloat& r,kfloat& g,kfloat& b)
	 * \param	r : red color (in/out param)
	 * \param	g : green color (in/out param)
	 * \param	b : blue color (in/out param)
	 */
	void	GetDiffuseColor(kfloat& r,kfloat& g,kfloat& b)
	{
		r=myDiffuseColor[0];
		g=myDiffuseColor[1];
		b=myDiffuseColor[2];
	}

	/**
	 * \brief	set the specular color
	 * \fn 		void	SetSpecularColor(kfloat r,kfloat g,kfloat b,kfloat a=KFLOAT_CONST(1.0f))
	 * \param	r : red color
	 * \param	g : green color
	 * \param	b : blue color
	 * \param	a : alpha value
	 */
  void	SetSpecularColor(kfloat r,kfloat g,kfloat b,kfloat a=KFLOAT_CONST(1.0f))
	{
		mySpecularColor[0]=r;
		mySpecularColor[1]=g;
		mySpecularColor[2]=b;
		mySpecularColor[3]=a;
	}

	/**
	 * \brief	get the specular color
	 * \fn 		void	GetSpecularColor(kfloat& r,kfloat& g,kfloat& b)
	 * \param	r : red color (in/out param)
	 * \param	g : green color (in/out param)
	 * \param	b : blue color (in/out param)
	 */
	void	GetSpecularColor(kfloat& r,kfloat& g,kfloat& b)
	{
		r=mySpecularColor[0];
		g=mySpecularColor[1];
		b=mySpecularColor[2];
	}

	/**
	 * \brief	set the emissive color
	 * \fn 		void	SetEmissionColor(kfloat r,kfloat g,kfloat b,kfloat a=KFLOAT_CONST(1.0f))
	 * \param	r : red color
	 * \param	g : green color
	 * \param	b : blue color
	 * \param	a : alpha value
	 */
  void	SetEmissionColor(kfloat r,kfloat g,kfloat b,kfloat a=KFLOAT_CONST(1.0f))
	{
		myEmissionColor[0]=r;
		myEmissionColor[1]=g;
		myEmissionColor[2]=b;
		myEmissionColor[3]=a;
	}

	/**
	 * \brief	get the emissive color
	 * \fn 		void	GetEmissionColor(kfloat& r,kfloat& g,kfloat& b)
	 * \param	r : red color (in/out param)
	 * \param	g : green color (in/out param)
	 * \param	b : blue color (in/out param)
	 */
	void	GetEmissionColor(kfloat& r,kfloat& g,kfloat& b)
	{
		r=myEmissionColor[0];
		g=myEmissionColor[1];
		b=myEmissionColor[2];
	}

	/**
	* \brief	get the shininess property
	* \fn 		void	GetShininess(kfloat& s)(kfloat& r,kfloat& g,kfloat& b)
	* \param	s : shininess property (in/out param)
	*/
	void GetShininess(kfloat& s)
	{
		s = myShininess;
	}

	/**
	 * \brief	compare with another coreModifiable
	 * \fn 		bool	Equal(CoreModifiable& other);
	 * \param	other : coreModifiable to compare with
	 * \return	TRUE if the 2 coreModifiable is equal
	 */
	bool	Equal(CoreModifiable& other) override;

	//! list of blend function source
	enum BlendFuncSource
	{
		S_ZERO,
		S_ONE,
		S_DST_COLOR, 
		S_ONE_MINUS_DST_COLOR, 
		S_SRC_ALPHA, 
		S_ONE_MINUS_SRC_ALPHA, 
		S_DST_ALPHA, 
		S_ONE_MINUS_DST_ALPHA, 
		S_SRC_ALPHA_SATURATE
	};

	//! list of blend function destination
	enum BlendFuncDest
	{
		D_ZERO, 
		D_ONE, 
		D_SRC_COLOR, 
		D_ONE_MINUS_SRC_COLOR,
		D_SRC_ALPHA, 
		D_ONE_MINUS_SRC_ALPHA,
		D_DST_ALPHA, 
		D_ONE_MINUS_DST_ALPHA
	};

	unsigned int	GetSelfDrawingNeeds() override
	{
		return ((unsigned int)Need_Predraw)|((unsigned int)Need_Postdraw);
	}


protected:
	/**
	 * \brief	destructor
	 * \fn 		~Material();
	 */
    virtual ~Material();  

	//! 1 front, 2 back, 3 both
	maInt			myFacing;
	//! blend function source
	maInt			myBlendFuncSource;
	//! blend function destination
	maInt			myBlendFuncDest;
	//! TRUE if the blend is enabled
	maBool			myBlendEnabled;
	//! TRUE if the material color is enabled
	maBool			myMaterialColorEnabled;
	//! ambient color
	maVect4DF		myAmbientColor;
	//! diffuse color
	maVect4DF		myDiffuseColor;
	//! specular color
	maVect4DF		mySpecularColor;
	//! emission color
	maVect4DF		myEmissionColor;
	//! shininess
	maFloat			myShininess;
	//! transparency
	maFloat			myTransparency;

}; 

#ifdef _3DSMAX

} // namespace KigsFramework

#endif

#endif //_MATERIAL_H_
