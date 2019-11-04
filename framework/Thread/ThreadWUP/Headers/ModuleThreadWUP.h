#ifndef _MODULETHREADWUP_H_
#define _MODULETHREADWUP_H_

#include "ModuleBase.h"

class ModuleThreadWUP;
extern ModuleThreadWUP* gInstanceThreadWUP;

#ifdef _KIGS_ONLY_STATIC_LIB_
#define MODULEINITFUNC			PlatformThreadModuleInit
extern ModuleBase* PlatformThreadModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);
#else
#define MODULEINITFUNC			ModuleInit
#endif

/*! \defgroup ThreadWindows Thread windows module
 *  thread module for windows
 	 \ingroup Thread
*/

// ****************************************
// * ModuleThreadWUP class
// * --------------------------------------
/*!  \class ModuleThreadWUP
      thread module for windows
	 \ingroup ThreadWindows
*/
// ****************************************

class ModuleThreadWUP : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(ModuleThreadWUP,ModuleBase,Thread)
             
	//! constructor
	ModuleThreadWUP(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
    
	//! init module
    void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);
	//! close module
    void Close();

	//! update module
	virtual void Update(const Timer& timer, void* addParam);
    
protected:

	//! destructor
	virtual ~ModuleThreadWUP();
	unsigned int myTLSIndexThreadPointer;
}; 

#endif //_MODULETHREADWUP_H_
