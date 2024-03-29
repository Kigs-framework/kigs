#include "PrecompiledHeaders.h"
#include "ModuleTimer.h"
#include "Timer.h"
#include "TimeProfiler.h"
#include "ControlledTimer.h"
#include "Ticker.h"
#include "DelayedFunc.h"

using namespace Kigs::Time;

IMPLEMENT_CLASS_INFO(ModuleTimer)

ModuleTimer::ModuleTimer(const std::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
}

ModuleTimer::~ModuleTimer()
{
}    

void ModuleTimer::Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params)
{
    BaseInit(core,"Timer",params);

	core->RegisterMainModuleList(this,TimerModuleCoreIndex);

	REGISTER_UPGRADOR(TickerUpgrador);
	REGISTER_UPGRADOR(DelayedFuncUpgrador);
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), Timer, Timer, Timer);
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), TimeProfiler,TimeProfiler,Timer);
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), ControlledTimer, ControlledTimer, Timer);

}

void ModuleTimer::Close()
{
    BaseClose();
}    

void ModuleTimer::Update(const Timer& timer, void* addParam)
{
	BaseUpdate(timer,addParam);
}    

