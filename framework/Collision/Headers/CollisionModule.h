#ifndef _COLLISIONMODULE_H_
#define _COLLISIONMODULE_H_

#include "ModuleBase.h"

/*! \defgroup Collision Collision module
 *  manage intersection and collisions
*/


// ****************************************
// * CollisionModule class
// * --------------------------------------
/**
* \file	CollisionModule.h
* \class	CollisionModule
* \ingroup Collision
* \ingroup Module
* \brief Generic Module collision / intersection management
*/
// ****************************************


class CollisionModule : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(CollisionModule,ModuleBase,Collision)
    CollisionModule(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	~CollisionModule() override;

	//! module init
    void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;
	
	//! module close
    void Close() override;         
                 
	//! module update
	void Update(const Timer& timer, void* addParam) override;
}; 

#endif //_COLLISIONMODULE_H_
