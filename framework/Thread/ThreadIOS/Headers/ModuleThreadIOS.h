#ifndef _MODULETHREADIOS_H_
#define _MODULETHREADIOS_H_

#include "ModuleBase.h"

class ModuleThreadIOS;


#ifdef _KIGS_ONLY_STATIC_LIB_
#define MODULEINITFUNC			PlatformThreadModuleInit
extern ModuleBase* PlatformThreadModuleInit(Core* core, const kstl::vector<CoreModifiableAttribute*>* params);
#else
#define MODULEINITFUNC			ModuleInit
#endif

/*! \defgroup ThreadIPhone Thread IPhone module
 *  thread module for IPhone
 	 \ingroup Thread
*/

// ****************************************
// * ModuleThreadIOS class
// * --------------------------------------
/*!  \class ModuleThreadIOS
      thread module for IOS
	 \ingroup ThreadIOS
*/
// ****************************************

class ModuleThreadIOS : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(ModuleThreadIOS, ModuleBase, Thread)
             
	//! constructor
	ModuleThreadIOS(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
    
	//! init module
    void Init(Core* core, const kstl::vector<CoreModifiableAttribute*>* params); 

	//! close module
    void Close();

	//! update module
    virtual void Update(const Timer& timer, void* addParam);
    
protected:

	//! destructor
	virtual ~ModuleThreadIOS();
  
}; 

#endif //ModuleThreadIOS
