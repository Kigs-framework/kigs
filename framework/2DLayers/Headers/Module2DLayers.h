// Doxygen group defines

/*! \defgroup 2DLayers 2D layers module
*  2DLayers Classes
*/

#ifndef _MODULE2DLAYERS_H_
#define _MODULE2DLAYERS_H_

#include "ModuleBase.h"
#include "kstlvector.h"

// ****************************************
// * Module2DLayers class
// * --------------------------------------
/**
* \file	Module2DLayers.h
* \class	Module2DLayers
* \ingroup 2DLayers
* \ingroup Module
* \brief Generic Module for 2D drawing ( IHM, sprites...)
*
* Manage UI classes and other 2D drawing.
*/
// ****************************************
class Module2DLayers : public ModuleBase
{
public:
	DECLARE_CLASS_INFO(Module2DLayers,ModuleBase,2DLayers)

	/**
	* \brief	constructor
	* \param	name : instance name
	* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	*/
	Module2DLayers(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	/**
	* \brief		initialize module
	* \param		core : link to the core, NOT NULL
	* \param		params : list of parameters
	*/
	void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;

	/**
	* \brief	close module
	*/
	void Close() override;

	/**
	* \brief	update module
	* \param	timer : timer global
	*/
	void Update(const Timer& timer, void* addParam) override;


	/**
	* \brief		get the layers list
	* \return		the layers list
	*/
	kstl::set<CoreModifiable*> GetLayerList();

	static void	setRotate180(bool rotate)
	{
		mRotate180=rotate;
	}

	static bool	getRotate180()
	{
		return mRotate180;
	}

protected:
	/**
	* \brief	destructor
	*/
	virtual ~Module2DLayers();

	//! list of layers
//	kstl::set<CoreModifiable*> myLayerList;

	static bool	mRotate180;
};
#endif //_MODULE2DLAYERS_H_
