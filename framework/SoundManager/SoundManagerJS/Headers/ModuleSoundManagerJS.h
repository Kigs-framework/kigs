#ifndef _MODULESOUNDMANAGERJS_H_
#define _MODULESOUNDMANAGERJS_H_

#include "ModuleBase.h"

class ModuleSoundManagerJS;
extern ModuleSoundManagerJS* gInstanceSoundJS;

#ifdef _KIGS_ONLY_STATIC_LIB_
#define MODULEINITFUNC			PlatformSoundModuleInit
extern ModuleBase* PlatformSoundModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);
#else
#define MODULEINITFUNC			ModuleInit
#endif

 
/*! \defgroup SoundManagerOpenAL SoundManager OpenAL module
 *  OpenAL implementation of the sound manager
	\ingroup SoundManager
*/

// ****************************************
// * ModuleSoundManagerJS class
// * --------------------------------------
/*!  \class ModuleSoundManagerJS
      manage sound playing, and sound object in the scene using javascript
	  \ingroup SoundManagerJS
*/
// ****************************************

class ModuleSoundManagerJS : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(ModuleSoundManagerJS, ModuleBase, SoundManager)
             
	ModuleSoundManagerJS(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
    
    void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)  override;
    void Close()  override;
	virtual void Update(const Timer& timer, void* addParam)  override;
    
protected:
	virtual ~ModuleSoundManagerJS();
  
}; 

#endif //_MODULESOUNDMANAGERJS_H_
