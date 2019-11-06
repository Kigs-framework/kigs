#ifndef _COLLISIONMODULE_H_
#define _COLLISIONMODULE_H_

#include "ModuleBase.h"

/*! \defgroup Collision Collision module
 *  manage intersection and collisions
*/


// ****************************************
// * CollisionModule class
// * --------------------------------------
/*!  \class CollisionModule
     this class is the module manager class. It registers the CollisionManager class
	 \ingroup Collision
*/
// ****************************************

class CollisionModule : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(CollisionModule,ModuleBase,Collision)

	//! module constructor 
    CollisionModule(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
         
	//! module init
    void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;
	
	//! module close
    void Close() override;         
                 
	//! module update
	void Update(const Timer& timer, void* addParam) override;
               
protected:
	//! destructor
    ~CollisionModule() override;    
}; 

#endif //_COLLISIONMODULE_H_
