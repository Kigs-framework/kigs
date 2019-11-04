#include "PrecompiledHeaders.h"
#include "ModuleTimer.h"
#include "Timer.h"
#include "TimeProfiler.h"
#include "ControlledTimer.h"

// ****************************************
// * ModuleTimer class
// * --------------------------------------
/**
 * \file	ModuleTimer.cpp
 * \ingroup Module
 * \brief	time and profiler module (source)
 * \author	ukn
 * \version ukn
 * \date	ukn
 */
// ****************************************
IMPLEMENT_CLASS_INFO(ModuleTimer)

ModuleTimer::ModuleTimer(const kstl::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
}

ModuleTimer::~ModuleTimer()
{
}    

void ModuleTimer::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
    BaseInit(core,"Timer",params);

	core->RegisterMainModuleList(this,TimerModuleCoreIndex);

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

	// update tickers

	kstl::map<CoreModifiable*,CoreModifiable*>::iterator	itTickers=myTickerList.begin();
	while(itTickers!=myTickerList.end())
	{
		(*itTickers).first->CallUpdate(timer,addParam);
		++itTickers;
	}
}    


bool	ModuleTimer::addItem(CoreModifiable *item, ItemPosition pos DECLARE_LINK_NAME)
{
	if(item->isSubType("TimeTicker"))
	{
		if(myTickerList.find(item)==myTickerList.end())
		{
			myTickerList[item]=item;
		}
	}

	return CoreModifiable::addItem(item,pos PASS_LINK_NAME(linkName));
}

bool	ModuleTimer::removeItem(CoreModifiable* item DECLARE_LINK_NAME)
{
	if(item->isSubType("TimeTicker"))
	{
		if(myTickerList.find(item)!=myTickerList.end())
		{
			myTickerList.erase(item);
		}
	}

	return CoreModifiable::removeItem(item PASS_LINK_NAME(linkName));
}
