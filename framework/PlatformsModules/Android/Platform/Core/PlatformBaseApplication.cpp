
#include "Core.h"
#include "CoreBaseApplication.h"
#include "Platform/Core/PlatformCore.h"
#include "CoreModifiable.h"
#include "Platform/Core/PlatformBaseApplication.h"
//#include "../../Renderer/Headers/TextureFileManager.h"
#include "Timer.h"
#include <unistd.h>

extern bool	gResumeReloadTexture;

void	PlatformBaseApplication::Sleep()
{
	SP<Timer> myAppTimer = ((CoreBaseApplication*)this)->GetApplicationTimer();
	myAppTimer->SetState(Timer::PAUSED);
}


const char* platformName = "Android";

const char* PlatformBaseApplication::getPlatformName()
{
	return platformName;
}

void	PlatformBaseApplication::Resume()
{
	KigsJavaIDManager::clearEnvList();
	
	// reload all textures and sprites
	// Not needed with setPreserveEGLContextOnPause(true);
	if (gResumeReloadTexture)
	{

		//RendererOpenGLES::ReinitModuleRenderer();

		kstl::set<CoreModifiable*>	instances;
		CoreModifiable::GetInstances("RenderingScreen", instances);
		(*instances.begin())->CallMethod(LABEL_TO_ID(ResetContext), NULL);

		kstl::set<CoreModifiable*>::iterator itInstances;

		// then reinit layers
		instances.clear();
		CoreModifiable::GetInstances("Base2DLayer", instances);

		for (itInstances = instances.begin(); itInstances != instances.end(); itInstances++)
		{
			CoreModifiable* current = (CoreModifiable*)(*itInstances);
			current->setValue("Reinit", true);
		}
		instances.clear();

		/*CoreModifiable::GetInstances("UIItem",instances);

		for(itInstances=instances.begin();itInstances!=instances.end();itInstances++)
		{
		CoreModifiable* current=(CoreModifiable*)(*itInstances);
		current->setValue(_S_2_ID("Reinit"),true);
		}*/

		// then reinit sprites
		instances.clear();
		CoreModifiable::GetInstances("BaseSprite", instances);

		for (itInstances = instances.begin(); itInstances != instances.end(); itInstances++)
		{
			CoreModifiable* current = (CoreModifiable*)(*itInstances);
			current->setValue("Reinit", true);
		}
	}
	
	SP<Timer> myAppTimer = ((CoreBaseApplication*)this)->GetApplicationTimer();
	myAppTimer->SetState(Timer::NORMAL);
}


// get number of core / processor
unsigned int PlatformBaseApplication::getProcessorCount()
{
	return sysconf( _SC_NPROCESSORS_ONLN );
}

//...
#include <sys/syscall.h>
//...
int PlatformBaseApplication::getCpuId() {


    unsigned cpu;
    if (syscall(__NR_getcpu, &cpu, NULL, NULL) < 0) {
        return -1;
    } else {
        return (int) cpu;
    }
}

void	 PlatformBaseApplication::setCurrentThreadAffinityMask(int mask)
{
	int syscallres;
	pid_t pid = gettid();
	syscallres = syscall(__NR_sched_setaffinity, pid, sizeof(mask), &mask);
	
}

void	 PlatformBaseApplication::setThreadAffinityMask(void* vpid,int mask)
{
	
	pid_t& pid = *(pid_t*)vpid;
	syscall(__NR_sched_setaffinity, pid, sizeof(mask), &mask);

}

void	PlatformBaseApplication::Message(int  mtype, int  Params)
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	g_env->CallStaticVoidMethod(KigsJavaIDManager::Main_class, KigsJavaIDManager::SendMessage, mtype, Params);
}

void	PlatformBaseApplication::OpenLink(const char* a_link)
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	jstring L_Link = g_env->NewStringUTF(a_link);
	g_env->CallStaticVoidMethod(KigsJavaIDManager::Main_class, KigsJavaIDManager::OpenLink, L_Link);
	g_env->DeleteLocalRef(L_Link);
}

void	PlatformBaseApplication::OpenLink(const unsigned short* al_ink, const unsigned int a_length)
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	jstring L_Link = g_env->NewString(al_ink, a_length);
	g_env->CallStaticVoidMethod(KigsJavaIDManager::Main_class, KigsJavaIDManager::OpenLink, L_Link);
	g_env->DeleteLocalRef(L_Link);
}

bool	PlatformBaseApplication::CheckBackKeyPressed()
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	jboolean backkey = g_env->CallStaticBooleanMethod(KigsJavaIDManager::Main_class, KigsJavaIDManager::GetBackKeyState);

	if (backkey == JNI_TRUE)
	{
		return true;
	}
	return false;
}

bool	PlatformBaseApplication::CheckConnexion()
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	jboolean L_result = g_env->CallStaticBooleanMethod(KigsJavaIDManager::Main_class, KigsJavaIDManager::CheckConnexion);
	return (L_result == JNI_TRUE) ? true : false;
}