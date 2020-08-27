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
 * \ingroup Renderer
 * \brief	Add fog to the scene.
 * 
 * ?? Obsolete ??
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
	maVect4DF	mColor;
	//! start distance
	maFloat		mStartDistance;
	//! end distance
	maFloat		mEndDistance;
	//! fog density
	maFloat		mDensity;
	//! rendering mode
	maEnum<3>	mMode;
};

#ifdef _3DSMAX

} // namespace KigsFramework

#endif

#endif //_FOG_H_
