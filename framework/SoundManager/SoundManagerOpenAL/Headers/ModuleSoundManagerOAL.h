#ifndef _MODULESOUNDMANAGEROAL_H_
#define _MODULESOUNDMANAGEROAL_H_

#include "ModuleBase.h"


class ModuleSoundManagerOAL;
extern ModuleSoundManagerOAL* gInstanceSoundOAL;

typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;

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
// * ModuleSoundManagerOAL class
// * --------------------------------------
/*!  \class ModuleSoundManagerOAL
      manage sound playing, and sound object in the scene using OpenAL
	  \ingroup SoundManagerOpenAL
*/
// ****************************************

class ModuleSoundManagerOAL : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(ModuleSoundManagerOAL,ModuleBase,SoundManager)
             
	ModuleSoundManagerOAL(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
    
    void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params); 
    void Close();
	virtual void Update(const Timer& timer, void* addParam);
    
protected:
	  virtual ~ModuleSoundManagerOAL();
	  ALCcontext *	mypContext;
	  ALCdevice *	mypDevice;
  
}; 

#endif //_MODULESOUNDMANAGEROAL_H_
