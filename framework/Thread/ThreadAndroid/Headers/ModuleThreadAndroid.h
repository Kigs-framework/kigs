#ifndef _MODULETHREADANDROID_H_
#define _MODULETHREADANDROID_H_

#include "ModuleBase.h"

class ModuleThreadAndroid;


#ifdef _KIGS_ONLY_STATIC_LIB_
#define MODULEINITFUNC			PlatformThreadModuleInit
extern ModuleBase* PlatformThreadModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);
#else
#define MODULEINITFUNC			ModuleInit
#endif

// ****************************************
// * ModuleThreadAndroid class
// * --------------------------------------
/*!  \class ModuleThreadAndroid
      thread module for android
	 \ingroup ThreadAndroid
*/
// ****************************************

class ModuleThreadAndroid : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(ModuleThreadAndroid,ModuleBase,Thread)
             
	//! constructor
	ModuleThreadAndroid(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
    
	//! init module
    void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;
	//! close module
    void Close() override;

	//! update module
	void Update(const Timer& timer, void* /*addParam*/) override;
    
protected:

	//! destructor
	  virtual ~ModuleThreadAndroid();
  
}; 

#endif //_MODULETHREADANDROID_H_
