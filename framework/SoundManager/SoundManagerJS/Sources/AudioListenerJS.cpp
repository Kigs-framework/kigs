#include "PrecompiledHeaders.h"
#include "AudioListenerJS.h"
#include "Core.h"
#include "TravState.h"
//#include "al.h"

IMPLEMENT_CLASS_INFO(AudioListenerJS)


AudioListenerJS::AudioListenerJS(const kstl::string& name,CLASS_NAME_TREE_ARG) : AudioListener(name,PASS_CLASS_NAME_TREE_ARG)
{

}

AudioListenerJS::~AudioListenerJS()
{  
}


void	AudioListenerJS::Activate()
{
}


void	AudioListenerJS::Desactivate()
{
}



