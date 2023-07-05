#include "PrecompiledHeaders.h"
#include "CoreSequenceLauncher.h"
#include "ModuleCoreAnimation.h"
#include "NotificationCenter.h"
#include "CoreSequence.h"
#include "Core.h"

using namespace Kigs::Action;
/* Sample xml code :

// sequence readed from json file and launched on parent instance when receiving "StartEnterAnim" notification

<Instance Name = "AnimButton" Type = "CoreSequenceLauncher" ID = "51">
	<CoreModifiableAttribute Type = "bool" Name = "StartOnFirstUpdate" Value = "false" Dynamic = "false" / >
	<CoreModifiableAttribute Type = "string" Name = "StartMessage" Value = "StartEnterAnim" Dynamic = "false" / >
	<CoreModifiableAttribute Type = "coreitem" Name = "Sequence" Value = "#AnimButtonStart.json" Dynamic = "false" / >
< / Instance>


// sequence readed directly in the xml and auto launched on parent instance (StartOnFirstUpdate) 


*/

IMPLEMENT_CLASS_INFO(CoreSequenceLauncher)

void	CoreSequenceLauncher::checkDeadSequences()
{
	bool deadFound = true;
	while (deadFound)
	{
		deadFound = false;
		auto itstart = mSequenceMap.begin();
		auto itend = mSequenceMap.end();
		// call destroy on each sequence
		for (auto& el : mSequenceMap)
		{
			auto ptr = el.second.lock();
			if (!ptr)
			{
				mSequenceMap.erase(itstart);
				deadFound = true;
				break;
			}
		}
	}
}

void	CoreSequenceLauncher::addSequencesToParents()
{
	ModuleCoreAnimation* L_CoreAnimation = (ModuleCoreAnimation*)KigsCore::Instance()->GetMainModuleInList(CoreAnimationModuleCoreIndex);
	checkDeadSequences();
	for(auto parent : GetParents())
	{
		// check that we did not already create the sequence
		auto existing = mSequenceMap.find(parent);
		bool already_added = existing != mSequenceMap.end();
		if (already_added)
		{
			auto ptr = existing->second.lock();
			if (!ptr)
			{
				already_added = false;
			}
		}
		if (!already_added && ((CoreItemSP)mSequence))
		{
			CoreItemSP sequence = ((CoreItemSP)mSequence);
			auto L_Sequence = L_CoreAnimation->createSequenceFromCoreMap(parent->SharedFromThis(), sequence);
			L_CoreAnimation->addSequence(L_Sequence.get());
			if (mStartOnFirstUpdate)
				L_Sequence->startAtFirstUpdate();
			mSequenceMap[parent] = L_Sequence;
		}
	}
}

//! try to init window, size and rendering screen must have been set
void	CoreSequenceLauncher::InitModifiable()
{
	if (GetParents().empty()) // start after it was added to parent
	{
		return;
	}
	CoreModifiable::InitModifiable();

	addSequencesToParents();

	if (mStartMessage.length())
	{
		KigsCore::GetNotificationCenter()->addObserver(this, "Start", mStartMessage);
	}
}

void CoreSequenceLauncher::Start()
{
	if(mOnce) KigsCore::GetNotificationCenter()->removeObserver(this, mStartMessage);
	addSequencesToParents();
	for (auto& kv : mSequenceMap)
	{
		auto ptr = kv.second.lock();
		if(ptr)
			ptr->startAtFirstUpdate();
	}
}

bool CoreSequenceLauncher::IsFinished()
{
	checkDeadSequences();
	if (mSequenceMap.size() == 0)
	{
		return true;
	}
	return false;
}

void CoreSequenceLauncher::Stop()
{
	for(auto& kv : mSequenceMap)
	{
		auto ptr = kv.second.lock();
		if (ptr)
			ptr->stop();
	}
}
