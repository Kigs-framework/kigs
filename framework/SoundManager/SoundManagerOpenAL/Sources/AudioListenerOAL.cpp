#include "PrecompiledHeaders.h"
#include "AudioListenerOAL.h"
#include "Core.h"
#include "TravState.h"
#include "al.h"

IMPLEMENT_CLASS_INFO(AudioListenerOAL)


AudioListenerOAL::AudioListenerOAL(const kstl::string& name,CLASS_NAME_TREE_ARG) : AudioListener(name,PASS_CLASS_NAME_TREE_ARG)
{

}

AudioListenerOAL::~AudioListenerOAL()
{  
}


void	AudioListenerOAL::Activate()
{
}


void	AudioListenerOAL::Desactivate()
{
}



