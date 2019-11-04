
#include "AudioBufferOAL.h"
//#include "AudioBufferOggOAL.h"
#include "AudioBufferModOAL.h"
#include "AudioSourceOAL.h"
#include "AudioListenerOAL.h"
#include "Core.h"
#include "alc.h"
#include "ModuleSoundManagerOAL.h"

ModuleSoundManagerOAL* gInstanceSoundOAL=0;

IMPLEMENT_CLASS_INFO(ModuleSoundManagerOAL)

ModuleSoundManagerOAL::ModuleSoundManagerOAL(const kstl::string& name, CLASS_NAME_TREE_ARG) :
ModuleBase(name, PASS_CLASS_NAME_TREE_ARG)
, mypContext(NULL)
, mypDevice(NULL)
{
}
    
ModuleSoundManagerOAL::~ModuleSoundManagerOAL()
{
    
}    


void ModuleSoundManagerOAL::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
    BaseInit(core,"ModuleSoundManagerOAL",params);
    DECLARE_FULL_CLASS_INFO(core,AudioBufferOAL,AudioBuffer,SoundManager)
    //DECLARE_FULL_CLASS_INFO(core,AudioBufferOggOAL,AudioBufferOgg,SoundManager)
	#ifdef USEMIKMOD
    DECLARE_FULL_CLASS_INFO(core,AudioBufferModOAL,AudioBufferMod,SoundManager)
	#endif
    DECLARE_FULL_CLASS_INFO(core,AudioSourceOAL,AudioSource,SoundManager)
    DECLARE_FULL_CLASS_INFO(core,AudioListenerOAL,AudioListener,SoundManager)


    // init OpenAL
	mypDevice = alcOpenDevice(NULL); // this is supposed to select the "preferred device"

	if (mypDevice)
    {
	    //Create context(s)
		mypContext = alcCreateContext(mypDevice, NULL);
		if (mypContext)
	    {
			//Set active context
			alcGetError(mypDevice);
			alcMakeContextCurrent(mypContext);
	    }

	    // Clear Error Code
	    alGetError();
		alcGetError(mypDevice);
    }
	
}       

void ModuleSoundManagerOAL::Close()
{
	alcMakeContextCurrent(NULL);
	alcDestroyContext(mypContext);
	alcCloseDevice(mypDevice);
    BaseClose();
}    

void ModuleSoundManagerOAL::Update(const Timer& timer, void* addParam)
{

}    

ModuleBase*  MODULEINITFUNC(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	KigsCore::ModuleStaticInit(core);
 
    ModuleBase* gInstanceSoundOAL=new ModuleSoundManagerOAL("ModuleSoundManagerOAL");
    gInstanceSoundOAL->Init(core,params);
    return gInstanceSoundOAL;
}    
