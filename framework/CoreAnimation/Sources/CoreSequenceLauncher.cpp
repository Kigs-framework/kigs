#include "PrecompiledHeaders.h"
#include "CoreSequenceLauncher.h"
#include "ModuleCoreAnimation.h"
#include "NotificationCenter.h"
#include "CoreSequence.h"
#include "Core.h"

/* Sample xml code :

// sequence readed from json file and launched on parent instance when receiving "StartEnterAnim" notification

<Instance Name = "AnimButton" Type = "CoreSequenceLauncher" ID = "51">
	<CoreModifiableAttribute Type = "bool" Name = "StartOnFirstUpdate" Value = "false" Dynamic = "false" / >
	<CoreModifiableAttribute Type = "string" Name = "StartMessage" Value = "StartEnterAnim" Dynamic = "false" / >
	<CoreModifiableAttribute Type = "coreitem" Name = "Sequence" Value = "#AnimButtonStart.json" Dynamic = "false" / >
< / Instance>


// sequence readed directly in the xml and auto launched on parent instance (StartOnFirstUpdate) 


<Instance Name="AnimButton" Type="CoreSequenceLauncher" ID="51">
	<CoreModifiableAttribute Type="bool" Name="StartOnFirstUpdate" Value="true" Dynamic="false"/>
	<CoreModifiableAttribute Type="string" Name="StartMessage" Value="" Dynamic="false"/>
	<CoreModifiableAttribute Type="coreitem" Name="Sequence" Dynamic="false">
	<![CDATA[
	{
	"SequenceTest":[
		{ "DoWhile" : ["Priority",
			{	"Serie" : 	[
				{
				"Linear1D":[1.000000,1.000,1.1,"PreScaleX"]
				}
				,
				{
				"Linear1D":[0.8,1.100,1.0,"PreScaleX"]
				}
				,
				{
				"Wait":[0.500000]
				}
			]
			}
			]
		}
		]
	}
	]]>
	</CoreModifiableAttribute>
</Instance>


*/

IMPLEMENT_CLASS_INFO(CoreSequenceLauncher)

void	CoreSequenceLauncher::checkDeadSequences()
{
	bool deadFound = true;
	while (deadFound)
	{
		deadFound = false;
		auto itstart = mySequenceMap.begin();
		auto itend = mySequenceMap.end();
		// call destroy on each sequence
		while (itstart != itend)
		{
			if ((*itstart).second->getRefCount() == 1) // only kept by me
			{
				mySequenceMap.erase(itstart);
				deadFound = true;
				break;
			}
			++itstart;
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
		if (mySequenceMap.find(parent) == mySequenceMap.end() && (CoreItem*)mySequence)
		{
			CoreItemSP sequence((CoreItem*)mySequence, GetRefTag{});
			auto L_Sequence = OwningRawPtrToSmartPtr(L_CoreAnimation->createSequenceFromCoreMap(parent, sequence));
			L_CoreAnimation->addSequence(L_Sequence.get());
			if (myStartOnFirstUpdate)
				L_Sequence->startAtFirstUpdate();
			mySequenceMap[parent] = L_Sequence;
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

	if (myStartMessage.const_ref().size())
	{
		KigsCore::GetNotificationCenter()->addObserver(this, "Start", myStartMessage);
	}
}

void CoreSequenceLauncher::Start()
{
	if(myOnce) KigsCore::GetNotificationCenter()->removeObserver(this, myStartMessage);
	addSequencesToParents();
	for (auto& kv : mySequenceMap)
	{
		kv.second->startAtFirstUpdate();
	}
}

bool CoreSequenceLauncher::IsFinished()
{
	checkDeadSequences();
	if (mySequenceMap.size() == 0)
	{
		return true;
	}
	return false;
}

void CoreSequenceLauncher::Stop()
{
	for(auto& kv : mySequenceMap)
	{
		kv.second->stop();
	}
}
