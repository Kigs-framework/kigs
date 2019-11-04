#ifndef _MODULETHREADWIN32_H_
#define _MODULETHREADWIN32_H_

#include "ModuleBase.h"

class ModuleThreadWin32;
extern ModuleThreadWin32* gInstanceThreadWin32;

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
// * ModuleThreadWin32 class
// * --------------------------------------
/*!  \class ModuleThreadWin32
      thread module for windows
	 \ingroup ThreadWindows
*/
// ****************************************

class ModuleThreadWin32 : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(ModuleThreadWin32,ModuleBase,Thread)
             
	//! constructor
	ModuleThreadWin32(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
    
	//! init module
    void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);
	//! close module
    void Close();

	//! update module
	virtual void Update(const Timer& timer, void* addParam);
    
protected:

	//! destructor
	virtual ~ModuleThreadWin32();
	unsigned int myTLSIndexThreadPointer;
}; 

#endif //_MODULETHREADWIN32_H_
