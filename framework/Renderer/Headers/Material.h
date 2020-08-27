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

// ****************************************
// * Material class
// * --------------------------------------
/**
* \file	Material.h
* \class	Material
* \ingroup Renderer
* \brief	Base class, generic material object.
*
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
		if (travstate->mCurrentMaterial == this)
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
		mAmbientColor[0]=r;
		mAmbientColor[1]=g;
		mAmbientColor[2]=b;
		mAmbientColor[3]=a;
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
		r=mAmbientColor[0];
		g=mAmbientColor[1];
		b=mAmbientColor[2];
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
			mTransparency=a;
		}
		mDiffuseColor[0]=r;
		mDiffuseColor[1]=g;
		mDiffuseColor[2]=b;
		mDiffuseColor[3]=mTransparency;
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
		r=mDiffuseColor[0];
		g=mDiffuseColor[1];
		b=mDiffuseColor[2];
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
		mSpecularColor[0]=r;
		mSpecularColor[1]=g;
		mSpecularColor[2]=b;
		mSpecularColor[3]=a;
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
		r=mSpecularColor[0];
		g=mSpecularColor[1];
		b=mSpecularColor[2];
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
		mEmissionColor[0]=r;
		mEmissionColor[1]=g;
		mEmissionColor[2]=b;
		mEmissionColor[3]=a;
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
		r=mEmissionColor[0];
		g=mEmissionColor[1];
		b=mEmissionColor[2];
	}

	/**
	* \brief	get the shininess property
	* \fn 		void	GetShininess(kfloat& s)(kfloat& r,kfloat& g,kfloat& b)
	* \param	s : shininess property (in/out param)
	*/
	void GetShininess(kfloat& s)
	{
		s = mShininess;
	}

	/**
	 * \brief	compare with another coreModifiable
	 * \fn 		bool	Equal(CoreModifiable& other);
	 * \param	other : coreModifiable to compare with
	 * \return	TRUE if the 2 coreModifiable is equal
	 */
	bool	Equal(const CoreModifiable& other) override;

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
	maInt			mFacing;
	//! blend function source
	maInt			mBlendFuncSource;
	//! blend function destination
	maInt			mBlendFuncDest;
	//! TRUE if the blend is enabled
	maBool			mBlendEnabled;
	//! TRUE if the material color is enabled
	maBool			mMaterialColorEnabled;
	//! ambient color
	maVect4DF		mAmbientColor;
	//! diffuse color
	maVect4DF		mDiffuseColor;
	//! specular color
	maVect4DF		mSpecularColor;
	//! emission color
	maVect4DF		mEmissionColor;
	//! shininess
	maFloat			mShininess;
	//! transparency
	maFloat			mTransparency;

}; 

#ifdef _3DSMAX

} // namespace KigsFramework

#endif

#endif //_MATERIAL_H_
